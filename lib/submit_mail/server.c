#include "server.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

#include "message.h"
#include "str.h"


struct outgoing_message {
    char const *message;
    size_t size;
    size_t bytes_read;
};


static size_t
read_function(void *buffer,
              size_t buffer_item_size,
              size_t buffer_item_count,
              void *user_data)
{
    struct outgoing_message *outgoing_message = (struct outgoing_message *)user_data;
    size_t buffer_size = buffer_item_count * buffer_item_size;
    size_t bytes_unread = outgoing_message->size - outgoing_message->bytes_read;
    if (!bytes_unread) return 0;
    
    size_t bytes_available = buffer_size > bytes_unread ? bytes_unread : buffer_size;
    memcpy(buffer, outgoing_message->message + outgoing_message->bytes_read, bytes_available);
    outgoing_message->bytes_read += bytes_available;
    
    return bytes_available;
}


static int
server_enable_ssl(struct server *server)
{
    CURLcode curl_code = CURLE_OK;
    
    curl_code = curl_easy_setopt(server->curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
    if (CURLE_OK != curl_code) goto curl_error;
    
    curl_code = curl_easy_setopt(server->curl, CURLOPT_SSL_VERIFYPEER, 0L);
    if (CURLE_OK != curl_code) goto curl_error;
    
    curl_code = curl_easy_setopt(server->curl, CURLOPT_SSL_VERIFYHOST, 0L);
    if (CURLE_OK != curl_code) goto curl_error;
    
    return 0;
    
curl_error:
    server->error = server_error_make_curl_error(curl_code);
    return -1;
}


struct server *
server_alloc(char const *host, in_port_t port, struct server_error *error)
{
    struct server *server = NULL;
    CURLcode curl_code = CURLE_OK;
    
    if (!error) {
        errno = EINVAL;
        return NULL;
    }
    
    if (!host) {
        *error = server_error_set_system_error(EINVAL);
        return NULL;
    }
    
    server = calloc(1, sizeof(struct server));
    if (!server) goto system_error;
    
    server->curl = curl_easy_init();
    if (!server->curl) goto system_error;
    
    char *url = str_alloc_formatted("smtp://%s:%u", host, (unsigned)port);
    if (!url) goto system_error;
    curl_code = curl_easy_setopt(server->curl, CURLOPT_URL, url);
    free(url);
    if (CURLE_OK != curl_code) goto curl_error;
    
    int result = server_enable_ssl(server);
    if (-1 == result) {
        *error = server->error;
        server_free(server);
        return NULL;
    }
    
    curl_code = curl_easy_setopt(server->curl, CURLOPT_READFUNCTION, read_function);
    if (CURLE_OK != curl_code) goto curl_error;
    
    curl_code = curl_easy_setopt(server->curl, CURLOPT_UPLOAD, 1L);
    if (CURLE_OK != curl_code) goto curl_error;
    
    return server;
    
curl_error:
    *error = server_error_make_curl_error(curl_code);
    server_free(server);
    return NULL;
    
system_error:
    *error = server_error_make_system_error();
    server_free(server);
    return NULL;
}


void
server_free(struct server *server)
{
    if (!server) return;
    curl_easy_cleanup(server->curl);
    free(server);
}


int
server_set_credentials(struct server *server,
                       char const *username,
                       char const *password)
{
    CURLcode curl_code = CURLE_OK;
    
    if (!server) {
        errno = EINVAL;
        return -1;
    }
    
    if (server_error_is_set(server->error)) return -1;
    
    if (!username || !password) {
        server->error = server_error_set_system_error(EINVAL);
        return -1;
    }
    
    curl_code = curl_easy_setopt(server->curl, CURLOPT_USERNAME, username);
    if (CURLE_OK != curl_code) goto curl_error;
    
    curl_code = curl_easy_setopt(server->curl, CURLOPT_PASSWORD, password);
    if (CURLE_OK != curl_code) goto curl_error;
    
    return 0;

curl_error:
    server->error = server_error_make_curl_error(curl_code);
    return -1;
}


int
server_set_debug(struct server *server, bool debug)
{
    if (!server) {
        errno = EINVAL;
        return -1;
    }
    
    if (server_error_is_set(server->error)) return -1;
    
    CURLcode curl_code = curl_easy_setopt(server->curl, CURLOPT_VERBOSE, debug ? 1 : 0);
    if (CURLE_OK != curl_code) {
        server->error = server_error_make_curl_error(curl_code);
        return -1;
    }
    
    return 0;
}


int
server_submit_message(struct server *server, struct message const *message)
{
    if (!server) {
        errno = EINVAL;
        return -1;
    }
    
    if (server_error_is_set(server->error)) return -1;
    
    if (!message) {
        server->error = server_error_set_system_error(EINVAL);
        return -1;
    }
    
    char *from = str_alloc_formatted("<%s>", message->from);
    if (!from) {
        server->error = server_error_make_system_error();
        return -1;
    }
    CURLcode curl_code = curl_easy_setopt(server->curl, CURLOPT_MAIL_FROM, from);
    free(from);
    if (CURLE_OK != curl_code) {
        server->error = server_error_make_curl_error(curl_code);
        return -1;
    }
    
    char *to = str_alloc_formatted("<%s>", message->to);
    if (!to) {
        server->error = server_error_make_system_error();
        return -1;
    }
    struct curl_slist *recipients = curl_slist_append(NULL, to);
    free(to);
    if (!recipients) {
        server->error = server_error_make_system_error();
        return -1;
    }
    
    curl_code = curl_easy_setopt(server->curl, CURLOPT_MAIL_RCPT, recipients);
    if (CURLE_OK != curl_code) {
        curl_slist_free_all(recipients);
        server->error = server_error_make_curl_error(curl_code);
        return -1;
    }
    
    char *serialized_message = message_alloc_serialized_message(message);
    if (!serialized_message) {
        curl_slist_free_all(recipients);
        server->error = server_error_make_system_error();
        return -1;
    }
    struct outgoing_message outgoing_message = {
        .message=serialized_message,
        .size=strlen(serialized_message),
        .bytes_read=0
    };
    
    curl_code = curl_easy_setopt(server->curl, CURLOPT_READDATA, &outgoing_message);
    if (CURLE_OK != curl_code) {
        curl_slist_free_all(recipients);
        free(serialized_message);
        server->error = server_error_make_curl_error(curl_code);
        return -1;
    }
    
    curl_code = curl_easy_perform(server->curl);
    free(serialized_message);
    curl_slist_free_all(recipients);
    if (CURLE_OK != curl_code) {
        server->error = server_error_make_curl_error(curl_code);
        return -1;
    }
    
    return 0;
}
