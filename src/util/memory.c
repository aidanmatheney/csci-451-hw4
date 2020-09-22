#include "../../include/util/memory.h"

#include "../../include/util/string.h"
#include "../../include/util/guard.h"
#include "../../include/util/error.h"

#include <stdlib.h>
#include <string.h>
#include <errno.h>

/**
 * Allocate memory of the given size using malloc. If the allocation fails, abort the program with an error message.
 *
 * @param size The size of the memory, in bytes.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns The allocated memory.
 */
void *safeMalloc(size_t const size, char const * const callerDescription) {
    guardNotNull(callerDescription, "callerDescription", "safeMalloc");

    void * const memory = malloc(size);
    if (memory == NULL) {
        int const mallocErrorCode = errno;
        char const * const mallocErrorMessage = strerror(mallocErrorCode);

        abortWithErrorFmt(
            "%s: Failed to allocate %zu bytes of memory using malloc (error code: %d; error message: \"%s\")",
            callerDescription,
            size,
            mallocErrorCode,
            mallocErrorMessage
        );
        return NULL;
    }

    return memory;
}

/**
 * Resize the given memory using realloc. If the reallocation fails, abort the program with an error message.
 *
 * @param memory The existing memory, or null.
 * @param newSize The new size of the memory, in bytes.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns The reallocated memory.
 */
void *safeRealloc(void * const memory, size_t const newSize, char const * const callerDescription) {
    guardNotNull(callerDescription, "callerDescription", "safeRealloc");

    void * const newMemory = realloc(memory, newSize);
    if (newMemory == NULL) {
        int const reallocErrorCode = errno;
        char const * const reallocErrorMessage = strerror(reallocErrorCode);

        abortWithErrorFmt(
            "%s: Failed to reallocate memory to %zu bytes using realloc (error code: %d; error message: \"%s\")",
            callerDescription,
            newSize,
            reallocErrorCode,
            reallocErrorMessage
        );
        return NULL;
    }

    return newMemory;
}
