#pragma once

#include <regex.h>

regex_t *safeRegcomp(char const *pattern, int flags, char const *callerDescription);
