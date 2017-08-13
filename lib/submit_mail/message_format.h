#ifndef SUBMIT_MAIL_MESSAGE_FORMAT_H_INCLUDED
#define SUBMIT_MAIL_MESSAGE_FORMAT_H_INCLUDED


#include <time.h>


char *
message_format_alloc_date(time_t date);

char *
message_format_alloc_message_id(char const *domain);


#endif
