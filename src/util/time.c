#include "../include/util/time.h"

#include "../include/util/error.h"

#include <string.h>
#include <time.h>
#include <errno.h>

/**
 * Get the current time. If the operation fails, abort the program with an error message.
 *
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns The current time.
 */
time_t safeTime(char const * const callerDescription) {
    time_t const timeResult = time(NULL);
    if (timeResult == -1) {
        int const timeErrorCode = errno;
        char const * const timeErrorMessage = strerror(timeErrorCode);

        abortWithErrorFmt(
            "%s: Failed to get current time using time (error code: %d; error message: \"%s\")",
            callerDescription,
            timeErrorCode,
            timeErrorMessage
        );
        return -1;
    }

    return timeResult;
}
