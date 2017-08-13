#include "server_error.h"

#include <errno.h>
#include <string.h>


char const *
server_error_get_message(struct server_error error)
{
    return error.curl_code
         ? curl_easy_strerror(error.curl_code)
         : strerror(error.error_number);
}


bool
server_error_is_clear(struct server_error error)
{
    return !server_error_is_set(error);
}


bool
server_error_is_set(struct server_error error)
{
    return error.curl_code || error.error_number;
}


struct server_error
server_error_make_curl_error(CURLcode curl_code)
{
    return (struct server_error){
        .curl_code=curl_code,
        .error_number=0
    };
}


struct server_error
server_error_make_system_error(void)
{
    return (struct server_error){
        .curl_code=CURLE_OK,
        .error_number=errno
    };
}


struct server_error
server_error_set_system_error(int error_number)
{
    errno = error_number;
    return server_error_make_system_error();
}
