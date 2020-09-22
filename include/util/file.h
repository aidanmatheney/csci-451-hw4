#pragma once

#include <stdbool.h>
#include <stdio.h>

FILE *safeFopen(char const *filePath, char const *modes, char const *callerDescription);
bool safeFgets(char *buffer, size_t bufferLength, FILE *file, char const *callerDescription);
char *readAllFileText(char const *filePath);
