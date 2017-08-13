#ifndef SUBMIT_MAIL_SERVER_H_INCLUDED
#define SUBMIT_MAIL_SERVER_H_INCLUDED


#include <stdbool.h>

#include <arpa/inet.h>

#include <curl/curl.h>

#include "server_error.h"


struct message;


struct server {
    CURL *curl;
    struct server_error error;
};


struct server *
server_alloc(char const *host, in_port_t port, struct server_error *error);

void
server_free(struct server *server);

int
server_set_credentials(struct server *server,
                       char const *username,
                       char const *password);

int
server_set_debug(struct server *server, bool debug);

int
server_submit_message(struct server *server, struct message const *message);


#endif
