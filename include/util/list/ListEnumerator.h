#pragma once

#include "../macro.h"
#include "../Enumerator.h"
#include "../memory.h"
#include "../guard.h"
#include "../error.h"

#include <stdbool.h>

/**
 * Declare (.h file) a generic ListEnumerator class.
 *
 * @param TList The name of the new type.
 * @param TItem The item type.
 */
#define DECLARE_LIST_ENUMERATOR(TList, TItem) \
    DECLARE_ENUMERATOR(TList##Enumerator, TItem) \
    \
    TList##Enumerator TList##Enumerator##_create(Const##TList list, int direction);

/**
 * Define (.c file) a generic ListEnumerator class.
 *
 * @param TList The name of the new type.
 * @param TItem The item type.
 */
#define DEFINE_LIST_ENUMERATOR(TList, TItem) \
    DECLARE_LIST_ENUMERATOR(TList, TItem) \
    \
    struct TList##Enumerator { \
        Const##TList list; \
        int direction; \
        int currentIndex; \
    }; \
    \
    TList##Enumerator TList##Enumerator##_create(Const##TList const list, int const direction) { \
        guardNotNull(list, "list", STRINGIFY(TList##Enumerator##_create)); \
        \
        TList##Enumerator const enumerator = safeMalloc(sizeof *enumerator, STRINGIFY(TList##Enumerator##_create)); \
        enumerator->list = list; \
        enumerator->direction = direction; \
        enumerator->currentIndex = direction == 1 ? -1 : (int)TList##_count(list); \
        return enumerator; \
    } \
    \
    void TList##Enumerator##_destroy(TList##Enumerator const enumerator) { \
        guardNotNull(enumerator, "enumerator", STRINGIFY(TList##Enumerator##_destroy)); \
        free(enumerator); \
    } \
    \
    bool TList##Enumerator##_moveNext(TList##Enumerator const enumerator) { \
        guardNotNull(enumerator, "enumerator", STRINGIFY(TList##Enumerator##_moveNext)); \
        \
        enumerator->currentIndex += enumerator->direction; \
        return enumerator->currentIndex >= 0 && enumerator->currentIndex < (int)TList##_count(enumerator->list); \
    } \
    \
    TItem TList##Enumerator##_current(Const##TList##Enumerator const enumerator) { \
        guardNotNull(enumerator, "enumerator", STRINGIFY(TList##Enumerator##_current)); \
        \
        if (enumerator->currentIndex < 0 || enumerator->currentIndex >= (int)TList##_count(enumerator->list)) { \
            abortWithErrorFmt( \
                "%s: Cannot get current item when current index (%d) is out of range (list count: %zu)", \
                STRINGIFY(TList##Enumerator##_current), \
                enumerator->currentIndex, \
                TList##_count(enumerator->list) \
            ); \
        } \
        \
        return TList##_get(enumerator->list, (size_t)enumerator->currentIndex); \
    } \
    \
    void TList##Enumerator##_reset(TList##Enumerator const enumerator) { \
        guardNotNull(enumerator, "enumerator", STRINGIFY(TList##Enumerator##_reset)); \
        enumerator->currentIndex = enumerator->direction == 1 ? -1 : (int)TList##_count(enumerator->list); \
    }
