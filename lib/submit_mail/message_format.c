#include "message_format.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <xlocale.h>

#include <uuid/uuid.h>

#include "str.h"


char *
message_format_alloc_date(time_t date)
{
    size_t const date_size = 32;
    char *buffer = malloc(date_size);
    if (!buffer) return NULL;
    
    locale_t c_locale = newlocale(LC_ALL_MASK, NULL, NULL);
    if (!c_locale) {
        free(buffer);
        return NULL;
    }
    
    size_t chars_formatted = strftime_l(buffer,
                                        date_size,
                                        "%a, %d %b %Y %H:%M:%S %z",
                                        localtime(&date),
                                        c_locale);
    freelocale(c_locale);
    if (!chars_formatted) {
        free(buffer);
        return NULL;
    }
    
    return buffer;
}


char *
message_format_alloc_message_id(char const *domain)
{
    if (!domain) {
        errno = EINVAL;
        return NULL;
    }
    
    uuid_t uuid;
    uuid_generate_random(uuid);
    uuid_string_t uuid_string;
    uuid_unparse_lower(uuid, uuid_string);
    
    return str_alloc_formatted("%s@%s", uuid_string, domain);
}
