#include "../../include/util/file.h"

#include "../../include/util/guard.h"
#include "../../include/util/error.h"

#include "../../include/util/StringBuilder.h"

#include <stdbool.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

/**
 * Open the file using fopen. If the operation fails, abort the program with an error message.
 *
 * @param filePath The file path.
 * @param modes The fopen modes string.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns The opened file.
 */
FILE *safeFopen(char const * const filePath, char const * const modes, char const * const callerDescription) {
    guardNotNull(filePath, "filePath", "safeFopen");
    guardNotNull(modes, "modes", "safeFopen");
    guardNotNull(callerDescription, "callerDescription", "safeFopen");

    FILE * const file = fopen(filePath, modes);
    if (file == NULL) {
        int const fopenErrorCode = errno;
        char const * const fopenErrorMessage = strerror(fopenErrorCode);

        abortWithErrorFmt(
            "%s: Failed to open file \"%s\" with modes \"%s\" using fopen (error code: %d; error message: \"%s\")",
            callerDescription,
            filePath,
            modes,
            fopenErrorCode,
            fopenErrorMessage
        );
        return NULL;
    }

    return file;
}

/**
 * Read characters from the given file into the given buffer. Stop as soon as one of the following conditions has been
 * met: (A) `bufferLength - 1` characters have been read, (B) a newline is encountered, or (C) the end of the file is
 * reached. The string read into the buffer will end with a terminating character. If the operation fails, abort the
 * program with an error message.
 *
 * @param buffer The buffer into which to read the string.
 * @param bufferLength The length of the buffer.
 * @param file The file to read from.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns Whether unread characters remain.
 */
bool safeFgets(
    char * const buffer,
    size_t const bufferLength,
    FILE * const file,
    char const * const callerDescription
) {
    guardNotNull(buffer, "buffer", "safeFgets");
    guardNotNull(file, "file", "safeFgets");
    guardNotNull(callerDescription, "callerDescription", "safeFgets");

    char * const fgetsResult = fgets(buffer, (int)bufferLength, file);
    bool const fgetsError = ferror(file);
    if (fgetsError) {
        int const fgetsErrorCode = errno;
        char const * const fgetsErrorMessage = strerror(fgetsErrorCode);

        abortWithErrorFmt(
            "%s: Failed to read %zu chars from file using fgets (error code: %d; error message: \"%s\")",
            callerDescription,
            bufferLength,
            fgetsErrorCode,
            fgetsErrorMessage
        );
        return false;
    }

    if (fgetsResult == NULL || feof(file)) {
        return false;
    }

    return true;
}

/**
 * Open a text file, read all the text in the file into a string, and then close the file.
 *
 * @param filePath The path to the file.
 *
 * @returns A string containing all text in the file. The caller is responsible for freeing this memory.
 */
char *readAllFileText(char const * const filePath) {
    guardNotNull(filePath, "filePath", "readAllFileText");

    StringBuilder const fileTextBuilder = StringBuilder_create();

    FILE * const file = safeFopen(filePath, "r", "readAllFileText");
    char fgetsBuffer[100];
    while (safeFgets(fgetsBuffer, 100, file, "readAllFileText")) {
        StringBuilder_append(fileTextBuilder, fgetsBuffer);
    }
    fclose(file);

    char * const fileText = StringBuilder_toStringAndDestroy(fileTextBuilder);

    return fileText;
}
