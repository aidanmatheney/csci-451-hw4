#pragma once

#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

FILE *safeFopen(char const *filePath, char const *modes, char const *callerDescription);

unsigned int safeFprintf(
    FILE *file,
    char const *callerDescription,
    char const *format,
    ...
);
unsigned int safeVfprintf(
    FILE *file,
    char const *format,
    va_list formatArgs,
    char const *callerDescription
);

bool safeFgets(char *buffer, size_t bufferLength, FILE *file, char const *callerDescription);

int safeFscanf(
    FILE *file,
    char const *callerDescription,
    char const *format,
    ...
);
int safeVfscanf(
    FILE *file,
    char const *format,
    va_list formatArgs,
    char const *callerDescription
);

bool scanFileExact(
    FILE *file,
    unsigned int expectedMatchCount,
    char const *format,
    ...
);
bool scanFileExactVA(
    FILE *file,
    unsigned int expectedMatchCount,
    char const *format,
    va_list formatArgs
);
