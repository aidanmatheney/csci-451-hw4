/*
 * Aidan Matheney
 * aidan.matheney@und.edu
 *
 * CSCI 451 HW4
 */

#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Declare (.h file) a typedef to a function pointer for a function with the specified args that returns void.
 *
 * @param TAction The name of the new type.
 * @param ... The function's parameter types.
 */
#define DECLARE_ACTION(TAction, ...) typedef void (*TAction)(__VA_ARGS__);

/**
 * Declare (.h file) a typedef to a function pointer for a function with the specified args and return type.
 *
 * @param TFunc The name of the new type.
 * @param TResult The function's return type.
 * @param ... The function's parameter types.
 */
#define DECLARE_FUNC(TFunc, TResult, ...) typedef TResult (*TFunc)(__VA_ARGS__);

DECLARE_FUNC(PthreadCreateStartRoutine, void *, void *)

pthread_t safePthreadCreate(
    pthread_attr_t const *attributes,
    PthreadCreateStartRoutine startRoutine,
    void *startRoutineArg,
    char const *callerDescription
);
void *safePthreadJoin(pthread_t threadId, char const *callerDescription);

void safeMutexInit(
    pthread_mutex_t *mutexOutPtr,
    pthread_mutexattr_t const *attributes,
    char const *callerDescription
);
void safeMutexLock(pthread_mutex_t *mutexPtr, char const *callerDescription);
void safeMutexUnlock(pthread_mutex_t *mutexPtr, char const *callerDescription);
void safeMutexDestroy(pthread_mutex_t *mutexPtr, char const *callerDescription);

void safeConditionInit(
    pthread_cond_t *conditionOutPtr,
    pthread_condattr_t const *attributes,
    char const *callerDescription
);
void safeConditionSignal(pthread_cond_t *conditionPtr, char const *callerDescription);
void safeConditionWait(
    pthread_cond_t *conditionPtr,
    pthread_mutex_t *mutexPtr,
    char const *callerDescription
);
void safeConditionDestroy(pthread_cond_t *conditionPtr, char const *callerDescription);

size_t safeSnprintf(
    char *buffer,
    size_t bufferLength,
    char const *callerDescription,
    char const *format,
    ...
);
size_t safeVsnprintf(
    char *buffer,
    size_t bufferLength,
    char const *format,
    va_list formatArgs,
    char const *callerDescription
);
size_t safeSprintf(
    char *buffer,
    char const *callerDescription,
    char const *format,
    ...
);
size_t safeVsprintf(
    char *buffer,
    char const *format,
    va_list formatArgs,
    char const *callerDescription
);

char *formatString(char const *format, ...);
char *formatStringVA(char const *format, va_list formatArgs);

void *safeMalloc(size_t size, char const *callerDescription);
void *safeRealloc(void *memory, size_t newSize, char const *callerDescription);

void guard(bool expression, char const *errorMessage);
void guardFmt(bool expression, char const *errorMessageFormat, ...);
void guardFmtVA(bool expression, char const *errorMessageFormat, va_list errorMessageFormatArgs);

void guardNotNull(void const *object, char const *paramName, char const *callerName);

FILE *safeFopen(char const *filePath, char const *modes, char const *callerDescription);

unsigned int safeFprintf(
    FILE *file,
    char const *callerDescription,
    char const *format,
    ...
);
unsigned int safeVfprintf(
    FILE *file,
    char const *format,
    va_list formatArgs,
    char const *callerDescription
);

bool safeFgets(char *buffer, size_t bufferLength, FILE *file, char const *callerDescription);

int safeFscanf(
    FILE *file,
    char const *callerDescription,
    char const *format,
    ...
);
int safeVfscanf(
    FILE *file,
    char const *format,
    va_list formatArgs,
    char const *callerDescription
);

bool scanFileExact(
    FILE *file,
    unsigned int expectedMatchCount,
    char const *format,
    ...
);
bool scanFileExactVA(
    FILE *file,
    unsigned int expectedMatchCount,
    char const *format,
    va_list formatArgs
);

void abortWithError(char const *errorMessage);
void abortWithErrorFmt(char const *errorMessageFormat, ...);
void abortWithErrorFmtVA(char const *errorMessageFormat, va_list errorMessageFormatArgs);

void hw4(char const *inFilePath, char const *outFilePath);

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

/**
 * Abort program execution after printing the specified error message to stderr.
 *
 * @param errorMessage The error message, not terminated by a newline.
 */
void abortWithError(char const * const errorMessage) {
    guardNotNull(errorMessage, "errorMessage", "abortWithError");

    fputs(errorMessage, stderr);
    fputc('\n', stderr);
    abort();
}

/**
 * Abort program execution after formatting and printing the specified error message to stderr.
 *
 * @param errorMessage The error message format (printf), not terminated by a newline.
 * @param ... The error message format arguments (printf).
 */
void abortWithErrorFmt(char const * const errorMessageFormat, ...) {
    va_list errorMessageFormatArgs;
    va_start(errorMessageFormatArgs, errorMessageFormat);
    abortWithErrorFmtVA(errorMessageFormat, errorMessageFormatArgs);
    va_end(errorMessageFormatArgs);
}

/**
 * Abort program execution after formatting and printing the specified error message to stderr.
 *
 * @param errorMessage The error message format (printf), not terminated by a newline.
 * @param errorMessageFormatArgs The error message format arguments (printf).
 */
void abortWithErrorFmtVA(char const * const errorMessageFormat, va_list errorMessageFormatArgs) {
    guardNotNull(errorMessageFormat, "errorMessageFormat", "abortWithErrorFmtVA");

    char * const errorMessage = formatStringVA(errorMessageFormat, errorMessageFormatArgs);
    abortWithError(errorMessage);
    free(errorMessage);
}

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
 * Print a formatted string to the given file. If the operation fails, abort the program with an error message.
 *
 * @param file The file.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 * @param format The format (printf).
 * @param ... The format arguments (printf).
 *
 * @returns The number of charactes printed (no string terminator character).
 */
unsigned int safeFprintf(
    FILE * const file,
    char const * const callerDescription,
    char const * const format,
    ...
) {
    va_list formatArgs;
    va_start(formatArgs, format);
    unsigned int const printedCharCount = safeVfprintf(file, format, formatArgs, callerDescription);
    va_end(formatArgs);
    return printedCharCount;
}

/**
 * Print a formatted string to the given file. If the operation fails, abort the program with an error message.
 *
 * @param file The file.
 * @param format The format (printf).
 * @param formatArgs The format arguments (printf).
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns The number of charactes printed (no string terminator character).
 */
unsigned int safeVfprintf(
    FILE * const file,
    char const * const format,
    va_list formatArgs,
    char const * const callerDescription
) {
    guardNotNull(file, "file", "safeVfprintf");
    guardNotNull(format, "format", "safeVfprintf");
    guardNotNull(callerDescription, "callerDescription", "safeVfprintf");

    int const printedCharCount = vfprintf(file, format, formatArgs);
    if (printedCharCount < 0) {
        int const vfprintfErrorCode = errno;
        char const * const vfprintfErrorMessage = strerror(vfprintfErrorCode);

        abortWithErrorFmt(
            "%s: Failed to print format \"%s\" to file using vfprintf (error code: %d; error message: \"%s\")",
            callerDescription,
            format,
            vfprintfErrorCode,
            vfprintfErrorMessage
        );
        return (unsigned int)-1;
    }

    return (unsigned int)printedCharCount;
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
 * Read values from the given file using the given format. Values are stored in the locations pointed to by formatArgs.
 * If the operation fails, abort the program with an error message.
 *
 * @param file The file.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 * @param format The format (scanf).
 * @param ... The format arguments (scanf).
 *
 * @returns The number of input items successfully matched and assigned, which can be fewer than provided for, or even
 *          zero in the event of an early matching failure. EOF is returned if the end of input is reached before either
 *          the first successful conversion or a matching failure occurs.
 */
int safeFscanf(
    FILE * const file,
    char const * const callerDescription,
    char const * const format,
    ...
) {
    va_list formatArgs;
    va_start(formatArgs, format);
    int const matchCount = safeVfscanf(file, format, formatArgs, callerDescription);
    va_end(formatArgs);
    return matchCount;
}

/**
 * Read values from the given file using the given format. Values are stored in the locations pointed to by formatArgs.
 * If the operation fails, abort the program with an error message.
 *
 * @param file The file.
 * @param format The format (scanf).
 * @param formatArgs The format arguments (scanf).
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns The number of input items successfully matched and assigned, which can be fewer than provided for, or even
 *          zero in the event of an early matching failure. EOF is returned if the end of input is reached before either
 *          the first successful conversion or a matching failure occurs.
 */
int safeVfscanf(
    FILE * const file,
    char const * const format,
    va_list formatArgs,
    char const * const callerDescription
) {
    guardNotNull(file, "file", "safeVfscanf");
    guardNotNull(format, "format", "safeVfscanf");
    guardNotNull(callerDescription, "callerDescription", "safeVfscanf");

    int const matchCount = vfscanf(file, format, formatArgs);
    bool const vfscanfError = ferror(file);
    if (vfscanfError) {
        int const vfscanfErrorCode = errno;
        char const * const vfscanfErrorMessage = strerror(vfscanfErrorCode);

        abortWithErrorFmt(
            "%s: Failed to read format \"%s\" from file using vfscanf (error code: %d; error message: \"%s\")",
            callerDescription,
            format,
            vfscanfErrorCode,
            vfscanfErrorMessage
        );
        return -1;
    }

    return matchCount;
}

/**
 * Read values from the given file using the given format. Values are stored in the locations pointed to by formatArgs.
 * If the number of matched items does not match the expected count or if the operation fails, abort the program with an
 * error message.
 *
 * @param file The file.
 * @param expectedMatchCount The number of items in the format expected to be matched.
 * @param format The format (scanf).
 * @param ... The format arguments (scanf).
 *
 * @returns Whether unread characters remain.
 */
bool scanFileExact(
    FILE * const file,
    unsigned int const expectedMatchCount,
    char const * const format,
    ...
) {
    va_list formatArgs;
    va_start(formatArgs, format);
    bool const hasUnreadCharacters = scanFileExactVA(file, expectedMatchCount, format, formatArgs);
    va_end(formatArgs);
    return hasUnreadCharacters;
}

/**
 * Read values from the given file using the given format. Values are stored in the locations pointed to by formatArgs.
 * If the number of matched items does not match the expected count or if the operation fails, abort the program with an
 * error message.
 *
 * @param file The file.
 * @param expectedMatchCount The number of items in the format expected to be matched.
 * @param format The format (scanf).
 * @param formatArgs The format arguments (scanf).
 *
 * @returns Whether unread characters remain.
 */
bool scanFileExactVA(
    FILE * const file,
    unsigned int const expectedMatchCount,
    char const * const format,
    va_list formatArgs
) {
    guardNotNull(file, "file", "scanFileExactVA");
    guardNotNull(format, "format", "scanFileExactVA");

    int const matchCount = safeVfscanf(file, format, formatArgs, "scanFileExactVA");
    if (matchCount == EOF) {
        return false;
    }

    if ((unsigned int)matchCount != expectedMatchCount) {
        abortWithErrorFmt(
            "scanFileExactVA: Failed to parse exact format \"%s\" from file"
            " (expected match count: %u; actual match count: %d)",
            format,
            expectedMatchCount,
            matchCount
        );
        return false;
    }

    return true;
}

/**
 * Ensure that the given expression involving a parameter is true. If it is false, abort the program with an error
 * message.
 *
 * @param expression The expression to verify is true.
 * @param errorMessage The error message.
 */
void guard(bool const expression, char const * const errorMessage) {
    if (errorMessage == NULL) {
        abortWithError("guard: errorMessage must not be null");
        return;
    }

    if (expression) {
        return;
    }

    abortWithError(errorMessage);
}

/**
 * Ensure that the given expression involving a parameter is true. If it is false, abort the program with an error
 * message.
 *
 * @param expression The expression to verify is true.
 * @param errorMessageFormat The error message format (printf).
 * @param ... The error message format arguments (printf).
 */
void guardFmt(bool const expression, char const * const errorMessageFormat, ...) {
    va_list errorMessageFormatArgs;
    va_start(errorMessageFormatArgs, errorMessageFormat);
    guardFmtVA(expression, errorMessageFormat, errorMessageFormatArgs);
    va_end(errorMessageFormatArgs);
}

/**
 * Ensure that the given expression involving a parameter is true. If it is false, abort the program with an error
 * message.
 *
 * @param expression The expression to verify is true.
 * @param errorMessageFormat The error message format (printf).
 * @param errorMessageFormatArgs The error message format arguments (printf).
 */
void guardFmtVA(bool const expression, char const * const errorMessageFormat, va_list errorMessageFormatArgs) {
    if (errorMessageFormat == NULL) {
        abortWithError("guardFmtVA: errorMessageFormat must not be null");
        return;
    }

    if (expression) {
        return;
    }

    abortWithErrorFmtVA(errorMessageFormat, errorMessageFormatArgs);
}

/**
 * Ensure that the given object supplied by a parameter is not null. If it is null, abort the program with an error
 * message.
 *
 * @param object The object to verify is not null.
 * @param paramName The name of the parameter supplying the object.
 * @param callerName The name of the calling function.
 */
void guardNotNull(void const * const object, char const * const paramName, char const * const callerName) {
    guard(paramName != NULL, "guardNotNull: paramName must not be null");
    guard(callerName != NULL, "guardNotNull: callerName must not be null");

    guardFmt(object != NULL, "%s: %s must not be null", callerName, paramName);
}

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

/**
 * If the given buffer is non-null, format the string into the buffer. If the buffer is null, simply calculate the
 * number of characters that would have been written if the buffer had been sufficiently large. If the operation fails,
 * abort the program with an error message.
 *
 * @param buffer The buffer into which to write, or null if only the formatted string length is desired.
 * @param bufferLength The length of the buffer, or 0 if the buffer is null.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 * @param format The string format (printf).
 * @param ... The string format arguments (printf).
 *
 * @returns The number of characters that would have been written if the buffer had been sufficiently large, not
 *          counting the terminating null character.
 */
size_t safeSnprintf(
    char * const buffer,
    size_t const bufferLength,
    char const * const callerDescription,
    char const * const format,
    ...
) {
    va_list formatArgs;
    va_start(formatArgs, format);
    size_t const length = safeVsnprintf(buffer, bufferLength, format, formatArgs, callerDescription);
    va_end(formatArgs);
    return length;
}

/**
 * If the given buffer is non-null, format the string into the buffer. If the buffer is null, simply calculate the
 * number of characters that would have been written if the buffer had been sufficiently large. If the operation fails,
 * abort the program with an error message.
 *
 * @param buffer The buffer into which to write, or null if only the formatted string length is desired.
 * @param bufferLength The length of the buffer, or 0 if the buffer is null.
 * @param format The string format (printf).
 * @param formatArgs The string format arguments (printf).
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns The number of characters that would have been written if the buffer had been sufficiently large, not
 *          counting the terminating null character.
 */
size_t safeVsnprintf(
    char * const buffer,
    size_t const bufferLength,
    char const * const format,
    va_list formatArgs,
    char const * const callerDescription
) {
    guardNotNull(format, "format", "safeVsnprintf");
    guardNotNull(callerDescription, "callerDescription", "safeVsnprintf");

    int const vsnprintfResult = vsnprintf(buffer, bufferLength, format, formatArgs);
    if (vsnprintfResult < 0) {
        abortWithErrorFmt(
            "%s: Failed to format string using vsnprintf (format: \"%s\"; result: %d)",
            callerDescription,
            format,
            vsnprintfResult
        );
        return (size_t)-1;
    }

    return (size_t)vsnprintfResult;
}

/**
 * Format the string into the buffer. If the operation fails, abort the program with an error message.
 *
 * @param buffer The buffer into which to write.
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 * @param format The string format (printf).
 * @param ... The string format arguments (printf).
 *
 * @returns The number of characters written.
 */
size_t safeSprintf(
    char * const buffer,
    char const * const callerDescription,
    char const * const format,
    ...
) {
    va_list formatArgs;
    va_start(formatArgs, format);
    size_t const length = safeVsprintf(buffer, format, formatArgs, callerDescription);
    va_end(formatArgs);
    return length;
}

/**
 * Format the string into the buffer. If the operation fails, abort the program with an error message.
 *
 * @param buffer The buffer into which to write.
 * @param format The string format (printf).
 * @param formatArgs The string format arguments (printf).
 * @param callerDescription A description of the caller to be included in the error message. This could be the name of
 *                          the calling function, plus extra information if useful.
 *
 * @returns The number of characters written.
 */
size_t safeVsprintf(
    char * const buffer,
    char const * const format,
    va_list formatArgs,
    char const * const callerDescription
) {
    guardNotNull(buffer, "buffer", "safeVsprintf");
    guardNotNull(format, "format", "safeVsprintf");
    guardNotNull(callerDescription, "callerDescription", "safeVsprintf");

    int const vsprintfResult = vsprintf(buffer, format, formatArgs);
    if (vsprintfResult < 0) {
        abortWithErrorFmt(
            "%s: Failed to format string using vsprintf (format: \"%s\"; result: %d)",
            callerDescription,
            format,
            vsprintfResult
        );
        return (size_t)-1;
    }

    return (size_t)vsprintfResult;
}

/**
 * Create a string using the specified format and format args.
 *
 * @param format The string format (printf).
 * @param ... The string format arguments (printf).
 *
 * @returns The formatted string. The caller is responsible for freeing the memory.
 */
char *formatString(char const * const format, ...) {
    va_list formatArgs;
    va_start(formatArgs, format);
    char * const formattedString = formatStringVA(format, formatArgs);
    va_end(formatArgs);
    return formattedString;
}

/**
 * Create a string using the specified format and format args.
 *
 * @param format The string format (printf).
 * @param formatArgs The string format arguments (printf).
 *
 * @returns The formatted string. The caller is responsible for freeing the memory.
 */
char *formatStringVA(char const * const format, va_list formatArgs) {
    guardNotNull(format, "format", "formatStringVA");

    va_list formatArgsForVsprintf;
    va_copy(formatArgsForVsprintf, formatArgs);

    size_t const formattedStringLength = safeVsnprintf(NULL, 0, format, formatArgs, "formatStringVA");
    char * const formattedString = safeMalloc(sizeof *formattedString * (formattedStringLength + 1), "formatStringVA");

    safeVsprintf(formattedString, format, formatArgsForVsprintf, "formatStringVA");
    va_end(formatArgsForVsprintf);

    return formattedString;
}

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

/*
 * Aidan Matheney
 * aidan.matheney@und.edu
 *
 * CSCI 451 HW4
 */

int main(int const argc, char ** const argv) {
    hw4("hw4.in", "hw4.out");
    return EXIT_SUCCESS;
}

