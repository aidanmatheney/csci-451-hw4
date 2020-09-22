#pragma once

#include <stdlib.h>

void *safeMalloc(size_t size, char const *callerDescription);
void *safeRealloc(void *memory, size_t newSize, char const *callerDescription);
