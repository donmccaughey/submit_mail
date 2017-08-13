#include <assert.h>
#include <libgen.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <curl/curl.h>

#include <submit_mail/submit_mail.h>

#include "options.h"


int
main(int argc, char *argv[])
{
    struct options *options = options_alloc(argc, argv);
    if (!options) {
        perror(basename(argv[0]));
        exit(EXIT_FAILURE);
    }
    if (options->is_usage) {
        options_usage_and_exit(options);
    }
    
    struct message *message = message_alloc_from_file(options->from,
                                                      options->to,
                                                      options->subject,
                                                      options->body_file);
    if (!message) {
        perror(options->command_name);
        exit(EXIT_FAILURE);
    }
    
    curl_global_init(CURL_GLOBAL_ALL);
    
    struct server_error error;
    struct server *server = server_alloc(options->host, options->port, &error);
    if (!server) {
        fprintf(stderr, "%s: unable to connect to server: %s\n",
                options->command_name, server_error_get_message(error));
        exit(EXIT_FAILURE);
    }
    
    server_set_credentials(server, options->username, options->password);
    server_set_debug(server, options->is_debug);
    if (server_error_is_set(server->error)) {
        fprintf(stderr, "%s: unable to connect to server: %s\n",
                options->command_name, server_error_get_message(server->error));
        exit(EXIT_FAILURE);
    }
    
    server_submit_message(server, message);
    if (server_error_is_set(server->error)) {
        fprintf(stderr, "%s: message submission failed: %s\n",
                options->command_name, server_error_get_message(server->error));
        exit(EXIT_FAILURE);
    }
    
    server_free(server);
    message_free(message);
    options_free(options);
    curl_global_cleanup();
    
    return EXIT_SUCCESS;
}
