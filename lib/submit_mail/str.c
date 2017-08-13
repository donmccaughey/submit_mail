#include "str.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


char *
str_alloc_empty(void)
{
    return calloc(1, 1);
}


char *
str_alloc_formatted(char const *format, ...)
{
    if (!format) {
        errno = EINVAL;
        return NULL;
    }
    
    va_list arguments;
    va_start(arguments, format);
    char *str = str_alloc_formatted_from_va_list(format, arguments);
    va_end(arguments);
    return str;
}


char *
str_alloc_formatted_from_va_list(char const *format, va_list arguments)
{
    if (!format) {
        errno = EINVAL;
        return NULL;
    }
    
    char *str;
    int chars_printed = vasprintf(&str, format, arguments);
    if (-1 == chars_printed) return NULL;
    return str;
}


size_t
str_formatted_length(char const *format, ...)
{
    if (!format) {
        errno = EINVAL;
        return 0;
    }
    
    va_list arguments;
    va_start(arguments, format);
    size_t length = str_formatted_length_from_va_list(format, arguments);
    va_end(arguments);
    return length;
}


size_t
str_formatted_length_from_va_list(char const *format, va_list arguments)
{
    if (!format) {
        errno = EINVAL;
        return 0;
    }
    
    va_list arguments_copy;
    va_copy(arguments_copy, arguments);
    int length = vsnprintf(NULL, 0, format, arguments_copy);
    return length < 0 ? 0 : length;
}


int
str_realloc_append_formatted(char **str, char const *format, ...)
{
    if (!str || !format) {
        errno = EINVAL;
        return -1;
    }
    
    va_list arguments;
    va_start(arguments, format);
    int result = str_realloc_append_formatted_from_va_list(str, format, arguments);
    va_end(arguments);
    return result;
}


int
str_realloc_append_formatted_from_va_list(char **str,
                                          char const *format,
                                          va_list arguments)
{
    if (!str || !format) {
        errno = EINVAL;
        return -1;
    }
    
    size_t append_length = str_formatted_length_from_va_list(format, arguments);
    if (!append_length) return 0;
    
    size_t original_length = *str ? strlen(*str) : 0;
    size_t size = original_length + append_length + 1;
    char *resized_str = realloc(*str, size);
    if (!resized_str) return -1;
    
    *str = resized_str;
    va_list arguments_copy;
    va_copy(arguments_copy, arguments);
    vsprintf(*str + original_length, format, arguments);
    return 0;
}
