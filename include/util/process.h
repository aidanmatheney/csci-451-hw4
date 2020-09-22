#pragma once

#include <sys/types.h>

pid_t safeFork(char const *callerDescription);

void safeExecvp(
    char const *filePath,
    char * const *argv,
    char const *callerDescription
);
