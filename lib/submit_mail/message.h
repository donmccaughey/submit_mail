#ifndef SUBMIT_MAIL_MESSAGE_H_INCLUDED
#define SUBMIT_MAIL_MESSAGE_H_INCLUDED


#include <stdio.h>
#include <time.h>


struct message {
    char *body;
    char *from;
    char *subject;
    char *to;
};


struct message *
message_alloc(char const *from,
              char const *to,
              char const *subject,
              char const *body);

struct message *
message_alloc_from_file(char const *from,
                        char const *to,
                        char const *subject,
                        FILE *body_file);

void
message_free(struct message *message);

char *
message_alloc_serialized_message(struct message const *message);


#endif
