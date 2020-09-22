#pragma once

#include "../memory.h"
#include "../macro.h"
#include "../error.h"

#include <stdlib.h>
#include <stdbool.h>

/**
 * Declare (.h file) a generic Result class which holds no success value but can hold a failure error.
 *
 * @param TResult The name of the new type.
 * @param TValue The type of the success value.
 * @param TError The type of the failure error
*/
#define DECLARE_VOID_RESULT(TResult, TError) \
    struct TResult; \
    typedef struct TResult * TResult; \
    typedef struct TResult const * Const##TResult; \
    \
    TResult TResult##_success(void); \
    TResult TResult##_failure(TError error); \
    void TResult##_destroy(TResult result); \
    \
    bool TResult##_isSuccess(Const##TResult result); \
    TError TResult##_getError(Const##TResult result); \
    TError TResult##_getErrorAndDestroy(TResult result);

/**
 * Define (.c file) a generic Result class which holds no success value but can hold a failure error.
 *
 * @param TResult The name of the new type.
 * @param TValue The type of the success value.
 * @param TError The type of the failure error
*/
#define DEFINE_VOID_RESULT(TResult, TError) \
    DECLARE_VOID_RESULT(TResult, TError) \
    \
    struct TResult { \
        bool success; \
        TError error; \
    }; \
    \
    TResult TResult##_success(void) { \
        TResult const result = safeMalloc(sizeof *result, STRINGIFY(TResult##_success)); \
        result->success = true; \
        return result; \
    } \
    \
    TResult TResult##_failure(TError const error) { \
        TResult const result = safeMalloc(sizeof *result, STRINGIFY(TResult##_success)); \
        result->success = false; \
        result->error = error; \
        return result; \
    } \
    \
    void TResult##_destroy(TResult const result) { \
        free(result); \
    } \
    \
    bool TResult##_isSuccess(Const##TResult const result) { \
        return result->success; \
    } \
    \
    TError TResult##_getError(Const##TResult const result) { \
        if (result->success) { \
            abortWithErrorFmt("%s: Cannot get result error. Result is success", STRINGIFY(TResult##_getError)); \
        } \
        \
        return result->error; \
    } \
    \
    TError TResult##_getErrorAndDestroy(TResult const result) { \
        TError const error = TResult##_getError(result); \
        TResult##_destroy(result); \
        return error; \
    }
