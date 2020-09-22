#include "../include/util/path.h"

#include "../include/util/memory.h"
#include "../include/util/guard.h"
#include "../include/util/error.h"

#include <stdio.h>
#include <string.h>
#include <errno.h>

/**
 * Generate a unique file path that does not name a currently existing file.
 *
 * @param filePath Pointer to a character array capable of holding at least L_tmpnam characters, to be used as a result
 *                 buffer. If a null pointer is passed, a pointer to an internal static buffer is returned.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns filePath if filePath was not a null pointer. Otherwise, a pointer to an internal static buffer is returned.
 */
char *safeTmpnam(char * const filePath, char const * const callerDescription) {
    guardNotNull(callerDescription, "callerDescription", "safeTmpnam");

    char * const tmpnamResult = tmpnam(filePath);
    if (tmpnamResult == NULL) {
        int const tmpnamErrorCode = errno;
        char const * const tmpnamErrorMessage = strerror(tmpnamErrorCode);

        abortWithErrorFmt(
            "%s: Failed to generate a temporary file path using tmpnam (error code: %d; error message: \"%s\")",
            callerDescription,
            tmpnamErrorCode,
            tmpnamErrorMessage
        );
        return NULL;
    }

    return tmpnamResult;
}

/**
 * Generate a unique file path that does not name a currently existing file.
 *
 * @returns The generated file path. The caller is responsible for freeing this memory.
 */
char *generateTempFilePath(void) {
    char * const filePath = safeMalloc(sizeof *filePath * L_tmpnam, "generateTempFilePath");
    safeTmpnam(filePath, "generateTempFilePath");
    return filePath;
}
