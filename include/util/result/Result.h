#pragma once

#include "../macro.h"
#include "../memory.h"
#include "../error.h"

#include <stdlib.h>
#include <stdbool.h>

/**
 * Declare (.h file) a generic Result class which can hold either a sucess value or a failure error.
 *
 * @param TResult The name of the new type.
 * @param TValue The type of the success value.
 * @param TError The type of the failure error
*/
#define DECLARE_RESULT(TResult, TValue, TError) \
    struct TResult; \
    typedef struct TResult * TResult; \
    typedef struct TResult const * Const##TResult; \
    \
    TResult TResult##_success(TValue value); \
    TResult TResult##_failure(TError error); \
    void TResult##_destroy(TResult result); \
    \
    bool TResult##_isSuccess(Const##TResult result); \
    TValue TResult##_getValue(Const##TResult result); \
    TValue TResult##_getValueAndDestroy(TResult result); \
    TError TResult##_getError(Const##TResult result); \
    TError TResult##_getErrorAndDestroy(TResult result);

/**
 * Define (.c file) a generic Result class which can hold either a sucess value or a failure error.
 *
 * @param TResult The name of the new type.
 * @param TValue The type of the success value.
 * @param TError The type of the failure error
*/
#define DEFINE_RESULT(TResult, TValue, TError) \
    DECLARE_RESULT(TResult, TValue, TError) \
    \
    struct TResult { \
        bool success; \
        TValue value; \
        TError error; \
    }; \
    \
    TResult TResult##_success(TValue const value) { \
        TResult const result = safeMalloc(sizeof *result, STRINGIFY(TResult##_success)); \
        result->success = true; \
        result->value = value; \
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
    TValue TResult##_getValue(Const##TResult const result) { \
        if (!result->success) { \
            abortWithErrorFmt("%s: Cannot get result value. Result is failure", STRINGIFY(TResult##_getValue)); \
        } \
        \
        return result->value; \
    } \
    \
    TValue TResult##_getValueAndDestroy(TResult const result) { \
        TValue const value = TResult##_getValue(result); \
        TResult##_destroy(result); \
        return value; \
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
