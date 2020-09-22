#pragma once

#include <stdlib.h>
#include <stdarg.h>

size_t safeSnprintf(
    char *buffer,
    size_t bufferLength,
    char const *callerDescription,
    char const *format,
    ...
);
size_t safeVsnprintf(
    char *buffer,
    size_t bufferLength,
    char const *format,
    va_list formatArgs,
    char const *callerDescription
);
size_t safeSprintf(
    char *buffer,
    char const *callerDescription,
    char const *format,
    ...
);
size_t safeVsprintf(
    char *buffer,
    char const *format,
    va_list formatArgs,
    char const *callerDescription
);

char *formatString(char const *format, ...);
char *formatStringVA(char const *format, va_list formatArgs);
