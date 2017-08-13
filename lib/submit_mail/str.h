#ifndef SUBMIT_MAIL_STR_H_INCLUDED
#define SUBMIT_MAIL_STR_H_INCLUDED


#include <stdarg.h>
#include <stddef.h>
#include <time.h>


char *
str_alloc_empty(void);

char *
str_alloc_formatted(char const *format, ...);

char *
str_alloc_formatted_from_va_list(char const *format, va_list arguments);

size_t
str_formatted_length(char const *format, ...);

size_t
str_formatted_length_from_va_list(char const *format, va_list arguments);

int
str_realloc_append_formatted(char **str, char const *format, ...);

int
str_realloc_append_formatted_from_va_list(char **str,
                                          char const *format,
                                          va_list arguments);


#endif
