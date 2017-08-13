#include "message.h"

#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <xlocale.h>

#include "message_format.h"
#include "str.h"


static int
str_realloc_append_date(char **str);

static int
str_realloc_append_message_id(char **str);


struct message *
message_alloc(char const *from,
              char const *to,
              char const *subject,
              char const *body)
{
    if (!from || !to) {
        errno = EINVAL;
        return NULL;
    }
    
    struct message *message = calloc(1, sizeof(struct message));
    if (!message) return NULL;
    
    message->from = strdup(from);
    if (!message->from) {
        message_free(message);
        return NULL;
    }
    
    message->to = strdup(to);
    if (!message->to) {
        message_free(message);
        return NULL;
    }
    
    if (subject) {        
        message->subject = strdup(subject);
        if (!message->subject) {
            message_free(message);
            return NULL;
        }
    }
    
    if (body) {
        message->body = strdup(body);
        if (!message->body) {
            message_free(message);
            return NULL;
        }
    }
    
    return message;
}


struct message *
message_alloc_from_file(char const *from,
                        char const *to,
                        char const *subject,
                        FILE *body_file)
{
    struct message *message = message_alloc(from, to, subject, NULL);
    if (!message) return NULL;
    
    size_t available = 4096 * 8;
    size_t used = 0;
    
    do {
        char *resized = realloc(message->body, available + 1);
        if (!resized) {
            message_free(message);
            return NULL;
        }
        message->body = resized;
        used += fread(message->body, sizeof(char), available, body_file);
        available = available * 3 / 2;
    } while (!feof(body_file) && !ferror(body_file));
    if (ferror(body_file)) {
        message_free(message);
        return NULL;
    }
    
    message->body[used] = '\0';
    return message;
}


void
message_free(struct message *message)
{
    if (!message) return;
    free(message->body);
    free(message->from);
    free(message->subject);
    free(message->to);
    free(message);
}


char *
message_alloc_serialized_message(struct message const *message)
{
    if (!message) {
        errno = EINVAL;
        return NULL;
    }
    
    char *serialized = str_alloc_empty();
    if (!serialized) return NULL;
    
    int result = str_realloc_append_date(&serialized);
    if (-1 == result) {
        free(serialized);
        return NULL;
    }
    
    result = str_realloc_append_message_id(&serialized);
    if (-1 == result) {
        free(serialized);
        return NULL;
    }
    
    result = str_realloc_append_formatted(&serialized, "From: %s\r\n", message->from);
    if (-1 == result) {
        free(serialized);
        return NULL;
    }
    
    result = str_realloc_append_formatted(&serialized, "To: %s\r\n", message->to);
    if (-1 == result) {
        free(serialized);
        return NULL;
    }
    
    if (message->subject) {        
        result = str_realloc_append_formatted(&serialized, "Subject: %s\r\n", message->subject);
        if (-1 == result) {
            free(serialized);
            return NULL;
        }
    }
    
    result = str_realloc_append_formatted(&serialized, "\r\n%s", message->body);
    if (-1 == result) {
        free(serialized);
        return NULL;
    }
    
    return serialized;
}


static int
str_realloc_append_date(char **str)
{
    time_t now = time(NULL);
    if ((time_t)-1 == now) return -1;
    
    char *date = message_format_alloc_date(now);
    if (!date) return -1;
    
    int result = str_realloc_append_formatted(str, "Date: %s\r\n", date);
    free(date);
    return result;
}


static int
str_realloc_append_message_id(char **str)
{
    long host_name_max = sysconf(_SC_HOST_NAME_MAX);
    if (-1 == host_name_max) return -1;
    
    size_t hostname_size = host_name_max + 1;
    char hostname[hostname_size];
    int result = gethostname(hostname, hostname_size);
    if (-1 == result) return -1;
    
    char *message_id = message_format_alloc_message_id(hostname);
    if (!message_id) return -1;
    
    result = str_realloc_append_formatted(str, "Message-ID: <%s>\r\n", message_id);
    free(message_id);
    return result;
}
