#ifndef SUBMIT_MAIL_SERVER_ERROR_H_INCLUDED
#define SUBMIT_MAIL_SERVER_ERROR_H_INCLUDED


#include <stdbool.h>

#include <curl/curl.h>


struct server_error {
    CURLcode curl_code;
    int error_number;
};


struct server_error
server_error_make_curl_error(CURLcode curl_code);

struct server_error
server_error_make_system_error(void);

struct server_error
server_error_set_system_error(int error_number);

char const *
server_error_get_message(struct server_error error);

bool
server_error_is_clear(struct server_error error);

bool
server_error_is_set(struct server_error error);


#endif
