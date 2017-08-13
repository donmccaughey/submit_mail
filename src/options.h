#ifndef SUBMIT_MAIL_OPTIONS_H_INCLUDED
#define SUBMIT_MAIL_OPTIONS_H_INCLUDED


#include <stdbool.h>
#include <stdio.h>

#include <arpa/inet.h>


struct options {
    FILE *body_file;
    char *command_name;
    char *from;
    char *host;
    bool is_debug;
    bool is_help;
    bool is_usage;
    bool is_verbose;
    char *password;
    in_port_t port;
    char *subject;
    char *to;
    char *username;
};


struct options *
options_alloc(int argc, char *argv[]);

void
options_free(struct options *options);

void
options_usage_and_exit(struct options const *options);


#endif
