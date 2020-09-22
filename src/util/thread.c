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
        return (pthread_t)-1;
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
        return NULL;
    }

    return threadReturnValue;
}

/**
 * Initialize the given mutex memory. If the operation fails, abort the program with an error message.
 *
 * @param mutexOutPtr A pointer to the memory where the mutex should be initialized. This pointer must be used directly
 *                    in all mutex-related functions (no copies).
 * @param attributes The attributes with which to create the mutex, or null to use the default attributes.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 */
void safeMutexInit(
    pthread_mutex_t * const mutexOutPtr,
    pthread_mutexattr_t const * const attributes,
    char const * const callerDescription
) {
    guardNotNull(callerDescription, "callerDescription", "safeMutexInit");

    int const mutexInitErrorCode = pthread_mutex_init(mutexOutPtr, attributes);
    if (mutexInitErrorCode != 0) {
        char const * const mutexInitErrorMessage = strerror(mutexInitErrorCode);

        abortWithErrorFmt(
            "%s: Failed to create mutex using pthread_mutex_init (error code: %d; error message: \"%s\")",
            callerDescription,
            mutexInitErrorCode,
            mutexInitErrorMessage
        );
    }
}

/**
 * Lock the given mutex. If the operation fails, abort the program with an error message.
 *
 * @param mutexPtr A pointer to the mutex.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 */
void safeMutexLock(pthread_mutex_t * const mutexPtr, char const * const callerDescription) {
    guardNotNull(mutexPtr, "mutexPtr", "safeMutexLock");
    guardNotNull(callerDescription, "callerDescription", "safeMutexLock");

    int const mutexLockErrorCode = pthread_mutex_lock(mutexPtr);
    if (mutexLockErrorCode != 0) {
        char const * const mutexLockErrorMessage = strerror(mutexLockErrorCode);

        abortWithErrorFmt(
            "%s: Failed to lock mutex using pthread_mutex_lock (error code: %d; error message: \"%s\")",
            callerDescription,
            mutexLockErrorCode,
            mutexLockErrorMessage
        );
    }
}

/**
 * Unlock the given mutex. If the operation fails, abort the program with an error message.
 *
 * @param mutexPtr A pointer to the mutex.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 */
void safeMutexUnlock(pthread_mutex_t * const mutexPtr, char const * const callerDescription) {
    guardNotNull(mutexPtr, "mutexPtr", "safeMutexUnlock");
    guardNotNull(callerDescription, "callerDescription", "safeMutexUnlock");

    int const mutexUnlockErrorCode = pthread_mutex_unlock(mutexPtr);
    if (mutexUnlockErrorCode != 0) {
        char const * const mutexUnlockErrorMessage = strerror(mutexUnlockErrorCode);

        abortWithErrorFmt(
            "%s: Failed to unlock mutex using pthread_mutex_unlock (error code: %d; error message: \"%s\")",
            callerDescription,
            mutexUnlockErrorCode,
            mutexUnlockErrorMessage
        );
    }
}

/**
 * Destroy the given mutex. If the operation fails, abort the program with an error message.
 *
 * @param mutexPtr A pointer to the mutex.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 */
void safeMutexDestroy(pthread_mutex_t * const mutexPtr, char const * const callerDescription) {
    guardNotNull(mutexPtr, "mutexPtr", "safeMutexDestroy");
    guardNotNull(callerDescription, "callerDescription", "safeMutexDestroy");

    int const mutexDestroyErrorCode = pthread_mutex_destroy(mutexPtr);
    if (mutexDestroyErrorCode != 0) {
        char const * const mutexDestroyErrorMessage = strerror(mutexDestroyErrorCode);

        abortWithErrorFmt(
            "%s: Failed to destroy mutex using pthread_mutex_destroy (error code: %d; error message: \"%s\")",
            callerDescription,
            mutexDestroyErrorCode,
            mutexDestroyErrorMessage
        );
    }
}

/**
 * Initialize the given condition memory. If the operation fails, abort the program with an error message.
 *
 * @param conditionOutPtr A pointer to the memory where the condition should be initialized. This pointer must be used
 *                        directly in all condition-related functions (no copies).
 * @param attributes The attributes with which to create the condition, or null to use the default attributes.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 */
void safeConditionInit(
    pthread_cond_t * const conditionOutPtr,
    pthread_condattr_t const * const attributes,
    char const * const callerDescription
) {
    guardNotNull(callerDescription, "callerDescription", "safeConditionInit");

    int const condInitErrorCode = pthread_cond_init(conditionOutPtr, attributes);
    if (condInitErrorCode != 0) {
        char const * const condInitErrorMessage = strerror(condInitErrorCode);

        abortWithErrorFmt(
            "%s: Failed to create condition using pthread_cond_init (error code: %d; error message: \"%s\")",
            callerDescription,
            condInitErrorCode,
            condInitErrorMessage
        );
    }
}

/**
 * Signal the given condition. If the operation fails, abort the program with an error message.
 *
 * @param conditionPtr A pointer to the condition.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 */
void safeConditionSignal(pthread_cond_t * const conditionPtr, char const * const callerDescription) {
    guardNotNull(conditionPtr, "conditionPtr", "safeConditionSignal");
    guardNotNull(callerDescription, "callerDescription", "safeConditionSignal");

    int const condSignalErrorCode = pthread_cond_signal(conditionPtr);
    if (condSignalErrorCode != 0) {
        char const * const condSignalErrorMessage = strerror(condSignalErrorCode);

        abortWithErrorFmt(
            "%s: Failed to signal condition using pthread_cond_signal (error code: %d; error message: \"%s\")",
            callerDescription,
            condSignalErrorCode,
            condSignalErrorMessage
        );
    }
}

/**
 * Wait for the given condition. If the operation fails, abort the program with an error message.
 *
 * @param conditionPtr A pointer to the condition.
 * @param mutexPtr A pointer to the mutex. The mutex must be locked.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 */
void safeConditionWait(
    pthread_cond_t * const conditionPtr,
    pthread_mutex_t * const mutexPtr,
    char const * const callerDescription
) {
    guardNotNull(conditionPtr, "conditionPtr", "safeConditionWait");
    guardNotNull(mutexPtr, "mutexPtr", "safeConditionWait");
    guardNotNull(callerDescription, "callerDescription", "safeConditionWait");

    int const condWaitErrorCode = pthread_cond_wait(conditionPtr, mutexPtr);
    if (condWaitErrorCode != 0) {
        char const * const condWaitErrorMessage = strerror(condWaitErrorCode);

        abortWithErrorFmt(
            "%s: Failed to wait for condition using pthread_cond_wait (error code: %d; error message: \"%s\")",
            callerDescription,
            condWaitErrorCode,
            condWaitErrorMessage
        );
    }
}

/**
 * Destroy the given condition. If the operation fails, abort the program with an error message.
 *
 * @param conditionPtr A pointer to the condition.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 */
void safeConditionDestroy(pthread_cond_t * const conditionPtr, char const * const callerDescription) {
    guardNotNull(conditionPtr, "conditionPtr", "safeConditionDestroy");
    guardNotNull(callerDescription, "callerDescription", "safeConditionDestroy");

    int const condDestroyErrorCode = pthread_cond_destroy(conditionPtr);
    if (condDestroyErrorCode != 0) {
        char const * const condDestroyErrorMessage = strerror(condDestroyErrorCode);

        abortWithErrorFmt(
            "%s: Failed to destroy condition using pthread_cond_destroy (error code: %d; error message: \"%s\")",
            callerDescription,
            condDestroyErrorCode,
            condDestroyErrorMessage
        );
    }
}
