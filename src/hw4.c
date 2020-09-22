#include "../include/hw4.h"

#include "../include/util/thread.h"
#include "../include/util/file.h"
#include "../include/util/guard.h"
#include "../include/util/error.h"

#include <stdbool.h>
#include <pthread.h>
#include <stdio.h>
#include <assert.h>

struct ReadIntegersThreadStartArg {
    char const *inFilePath;
    int *integerOutPtr;
    bool *finishedPtr;

    pthread_mutex_t *syncMutexPtr;
    pthread_cond_t *integerReadConditionPtr;
    pthread_cond_t *integerWroteConditionPtr;
};

struct WriteIntegersThreadStartArg {
    FILE *outFile;
    int *integerInPtr;
    bool *finishedPtr;

    pthread_mutex_t *syncMutexPtr;
    pthread_cond_t *integerReadConditionPtr;
    pthread_cond_t *integerWroteConditionPtr;
};

static void *readIntegersThreadStart(void *argAsVoidPtr);
static void *writeIntegersThreadStart(void *argAsVoidPtr);

/**
 * Run CSCI 451 HW4. This reads integers from the given input file and writes to the given output file. For each read
 * integer, if it is even, it will be written twice to the output file, and if it is odd, it will be written once to the
 * output file. The reading and writing will be split into two threads, where after the reading thread reads an integer,
 * it waits for the writing thread to process it.
 *
 * @param inFilePath The path to the input file containing integers deliminated by newline characters.
 * @param outFilePath The path to the output file.
 */
void hw4(char const * const inFilePath, char const * const outFilePath) {
    guardNotNull(inFilePath, "inFilePath", "hw4");
    guardNotNull(outFilePath, "outFilePath", "hw4");

    FILE * const outFile = safeFopen(outFilePath, "w", "hw4");

    int readInteger;
    bool finished = false;

    pthread_mutex_t syncMutex;
    safeMutexInit(&syncMutex, NULL, "hw4");
    pthread_cond_t integerReadCondition;
    safeConditionInit(&integerReadCondition, NULL, "hw4");
    pthread_cond_t integerWroteCondition;
    safeConditionInit(&integerWroteCondition, NULL, "hw4");

    pthread_t const readIntegersThreadId = safePthreadCreate(
        NULL,
        readIntegersThreadStart,
        &(struct ReadIntegersThreadStartArg){
            .inFilePath = inFilePath,
            .integerOutPtr = &readInteger,
            .finishedPtr = &finished,

            .syncMutexPtr = &syncMutex,
            .integerReadConditionPtr = &integerReadCondition,
            .integerWroteConditionPtr = &integerWroteCondition
        },
        "hw4"
    );
    pthread_t const writeIntegersThreadId = safePthreadCreate(
        NULL,
        writeIntegersThreadStart,
        &(struct WriteIntegersThreadStartArg){
            .outFile = outFile,
            .integerInPtr = &readInteger,
            .finishedPtr = &finished,

            .syncMutexPtr = &syncMutex,
            .integerReadConditionPtr = &integerReadCondition,
            .integerWroteConditionPtr = &integerWroteCondition
        },
        "hw4"
    );

    safePthreadJoin(readIntegersThreadId, "hw4");
    safePthreadJoin(writeIntegersThreadId, "hw4");

    safeMutexDestroy(&syncMutex, "hw4");
    safeConditionDestroy(&integerReadCondition, "hw4");
    safeConditionDestroy(&integerWroteCondition, "hw4");

    fclose(outFile);
}

static void *readIntegersThreadStart(void * const argAsVoidPtr) {
    assert(argAsVoidPtr != NULL);
    struct ReadIntegersThreadStartArg const * const argPtr = argAsVoidPtr;

    FILE * const inFile = safeFopen(argPtr->inFilePath, "r", "readIntegersThreadStart");

    safeMutexLock(argPtr->syncMutexPtr, "readIntegersThreadStart");
    while (scanFileExact(inFile, 1, "%d\n", argPtr->integerOutPtr)) {
        safeConditionSignal(argPtr->integerReadConditionPtr, "readIntegersThreadStart");
        safeConditionWait(argPtr->integerWroteConditionPtr, argPtr->syncMutexPtr, "readIntegersThreadStart");
    }
    *argPtr->finishedPtr = true;
    safeConditionSignal(argPtr->integerReadConditionPtr, "readIntegersThreadStart");
    safeMutexUnlock(argPtr->syncMutexPtr, "readIntegersThreadStart");

    fclose(inFile);

    return NULL;
}

static void *writeIntegersThreadStart(void * const argAsVoidPtr) {
    assert(argAsVoidPtr != NULL);
    struct WriteIntegersThreadStartArg const * const argPtr = argAsVoidPtr;

    safeMutexLock(argPtr->syncMutexPtr, "writeIntegersThreadStart");
    while (true) {
        safeConditionWait(argPtr->integerReadConditionPtr, argPtr->syncMutexPtr, "writeIntegersThreadStart");

        if (*argPtr->finishedPtr) {
            // Reading thread reached end of input file
            break;
        }

        int const readInteger = *argPtr->integerInPtr;
        if (readInteger % 2 == 0) {
            // Even, so write the value twice
            safeFprintf(argPtr->outFile, "writeIntegersThreadStart", "%d\n%d\n", readInteger, readInteger);
        } else {
            // Odd, so write the value once
            safeFprintf(argPtr->outFile, "writeIntegersThreadStart", "%d\n", readInteger);
        }

        safeConditionSignal(argPtr->integerWroteConditionPtr, "writeIntegersThreadStart");
    }
    safeMutexUnlock(argPtr->syncMutexPtr, "writeIntegersThreadStart");

    return NULL;
}
