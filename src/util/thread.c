#include "../include/util/thread.h"

#include "../include/util/guard.h"
#include "../include/util/error.h"

#include <string.h>
#include <pthread.h>

/**
 * Create a new thread. If the operation fails, abort the program with an error message.
 *
 * @param attributes The attributes with which to create the thread, or null to use the default attributes.
 * @param startRoutine The function to run in the new thread. This function will be called with startRoutineArg as its
 *                     sole argument. If this function returns, the effect is as if there was an implicit call to
 *                     pthread_exit() using the return value of startRoutine as the exit status.
 * @param startRoutineArg The argument to pass to startRoutine.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns The ID of the newly created thread.
 */
pthread_t safePthreadCreate(
    pthread_attr_t const * const attributes,
    PthreadCreateStartRoutine const startRoutine,
    void * const startRoutineArg,
    char const * const callerDescription
) {
    guardNotNull(callerDescription, "callerDescription", "safePthreadCreate");

    pthread_t threadId;
    int const pthreadCreateErrorCode = pthread_create(&threadId, attributes, startRoutine, startRoutineArg);
    if (pthreadCreateErrorCode != 0) {
        char const * const pthreadCreateErrorMessage = strerror(pthreadCreateErrorCode);

        abortWithErrorFmt(
            "%s: Failed to create new thread using pthread_create (error code: %d; error message: \"%s\")",
            callerDescription,
            pthreadCreateErrorCode,
            pthreadCreateErrorMessage
        );
    }

    return threadId;
}

/**
 * Wait for the given thread to terminate. If the operation fails, abort the program with an error message.
 *
 * @param threadId The thread ID.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns The thread's return value.
 */
void *safePthreadJoin(pthread_t const threadId, char const * const callerDescription) {
    guardNotNull(callerDescription, "callerDescription", "safePthreadJoin");

    void *threadReturnValue;
    int const pthreadJoinErrorCode = pthread_join(threadId, &threadReturnValue);
    if (pthreadJoinErrorCode != 0) {
        char const * const pthreadJoinErrorMessage = strerror(pthreadJoinErrorCode);

        abortWithErrorFmt(
            "%s: Failed to join threads using pthread_join (error code: %d; error message: \"%s\")",
            callerDescription,
            pthreadJoinErrorCode,
            pthreadJoinErrorMessage
        );
    }

    return threadReturnValue;
}
