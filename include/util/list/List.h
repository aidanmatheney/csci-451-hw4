#pragma once

#include "../macro.h"
#include "../callback.h"
#include "../result.h"
#include "../memory.h"
#include "../guard.h"
#include "../error.h"

#include "./ListEnumerator.h"

#include <stdbool.h>
#include <assert.h>

/**
 * Declare (.h file) a generic List class.
 *
 * @param TList The name of the new type.
 * @param TItem The item type.
 */
#define DECLARE_LIST(TList, TItem) \
    struct TList; \
    typedef struct TList * TList; \
    typedef struct TList const * Const##TList; \
    \
    DECLARE_ACTION(TList##ForEachCallback, void *, size_t, TItem) \
    DECLARE_FUNC(TList##FindCallback, bool, void *, size_t, TItem) \
    DECLARE_RESULT(TList##FindItemResult, TItem, void *) \
    \
    DECLARE_LIST_ENUMERATOR(TList, TItem) \
    \
    TList TList##_create(void); \
    TList TList##_fromItems(TItem const *items, size_t count); \
    TList TList##_fromList(Const##TList list); \
    void TList##_destroy(TList list); \
    \
    TItem const *TList##_items(Const##TList list); \
    size_t TList##_count(Const##TList list); \
    \
    TItem TList##_get(Const##TList list, size_t index); \
    \
    void TList##_add(TList list, TItem item); \
    void TList##_addMany(TList list, TItem const *items, size_t count); \
    void TList##_insert(TList list, size_t index, TItem item); \
    void TList##_insertMany(TList list, size_t index, TItem const *items, size_t count); \
    void TList##_set(TList list, size_t index, TItem item); \
    \
    void TList##_removeAt(TList list, size_t index); \
    void TList##_removeManyAt(TList list, size_t startIndex, size_t count); \
    void TList##_clear(TList list); \
    \
    void TList##_forEach(Const##TList list, void *state, TList##ForEachCallback callback); \
    void TList##_forEachReverse(Const##TList list, void *state, TList##ForEachCallback callback); \
    bool TList##_has(Const##TList list, TItem item); \
    size_t TList##_indexOf(Const##TList list, TItem item); \
    size_t TList##_lastIndexOf(Const##TList list, TItem item); \
    bool TList##_findHas(Const##TList list, void *state, TList##FindCallback callback); \
    TList##FindItemResult TList##_find(Const##TList list, void *state, TList##FindCallback callback); \
    size_t TList##_findIndex(Const##TList list, void *state, TList##FindCallback callback); \
    TList##FindItemResult TList##_findLast(Const##TList list, void *state, TList##FindCallback callback); \
    size_t TList##_findLastIndex(Const##TList list, void *state, TList##FindCallback callback); \
    \
    TList##Enumerator TList##_enumerate(Const##TList list); \
    TList##Enumerator TList##_enumerateReverse(Const##TList list); \
    \
    void TList##_fillArray(Const##TList list, TItem *array, size_t startIndex, size_t count);

/**
 * Define (.c file) a generic List class.
 *
 * @param TList The name of the new type.
 * @param TItem The item type.
 */
#define DEFINE_LIST(TList, TItem) \
    DECLARE_LIST(TList, TItem) \
    \
    static void TList##_ensureCapacity(TList list, size_t targetCapacity); \
    static void TList##_guardIndexInRange(Const##TList list, size_t index, char const *callerName); \
    static void TList##_guardIndexInInsertRange(Const##TList list, size_t index, char const *callerName); \
    static void TList##_guardStartIndexAndCountInRange(Const##TList list, size_t startIndex, size_t count, char const *callerName); \
    \
    DEFINE_RESULT(TList##FindItemResult, TItem, void *) \
    \
    DEFINE_LIST_ENUMERATOR(TList, TItem) \
    \
    struct TList { \
        TItem *items; \
        size_t count; \
        size_t capacity; \
    }; \
    \
    TList TList##_create(void) { \
        TList const list = safeMalloc(sizeof *list, STRINGIFY(TList##_create)); \
        list->items = NULL; \
        list->count = 0; \
        list->capacity = 0; \
        return list; \
    } \
    \
    TList TList##_fromItems(TItem const * const items, size_t const count) { \
        guardNotNull(items, "items", STRINGIFY(TList##_fromItems)); \
        \
        TList const list = TList##_create(); \
        TList##_addMany(list, items, count); \
        return list; \
    } \
    \
    TList TList##_fromList(Const##TList const list) { \
        guardNotNull(list, "list", STRINGIFY(TList##_fromList)); \
        return TList##_fromItems(list->items, list->count); \
    } \
    \
    void TList##_destroy(TList const list) { \
        guardNotNull(list, "list", STRINGIFY(TList##_destroy)); \
        \
        free(list->items); \
        free(list); \
    } \
    \
    TItem const *TList##_items(Const##TList const list) { \
        guardNotNull(list, "list", STRINGIFY(TList##_items)); \
        return list->items; \
    } \
    \
    size_t TList##_count(Const##TList const list) { \
        guardNotNull(list, "list", STRINGIFY(TList##_count)); \
        return list->count; \
    } \
    \
    TItem TList##_get(Const##TList const list, size_t const index) { \
        guardNotNull(list, "list", STRINGIFY(TList##_get)); \
        TList##_guardIndexInRange(list, index, STRINGIFY(TList##_get)); \
        \
        TItem const item = list->items[index]; \
        return item; \
    } \
    \
    void TList##_add(TList const list, TItem const item) { \
        guardNotNull(list, "list", STRINGIFY(TList##_add)); \
        \
        TList##_ensureCapacity(list, list->count + 1); \
        list->items[list->count] = item; \
        list->count += 1; \
    } \
    \
    void TList##_addMany(TList const list, TItem const * const items, size_t const count) { \
        guardNotNull(list, "list", STRINGIFY(TList##_addMany)); \
        guardNotNull(items, "items", STRINGIFY(TList##_addMany)); \
        \
        TList##_ensureCapacity(list, list->count + count); \
        for (size_t i = 0; i < count; i += 1) { \
            TItem const item = items[i]; \
            list->items[list->count + i] = item; \
        } \
        list->count += count; \
    } \
    \
    void TList##_insert(TList const list, size_t const index, TItem const item) { \
        guardNotNull(list, "list", STRINGIFY(TList##_insert)); \
        TList##_guardIndexInInsertRange(list, index, STRINGIFY(TList##_insert)); \
        \
        TList##_ensureCapacity(list, list->count + 1); \
        for (int i = (int)list->count - 1; i >= (int)index; i -= 1) { \
            /* Shift each item at an index >= the target index one to the right */ \
            list->items[i + 1] = list->items[i]; \
        } \
        list->items[index] = item; \
        list->count += 1; \
    } \
    \
    void TList##_insertMany(TList const list, size_t const index, TItem const * const items, size_t const count) { \
        guardNotNull(list, "list", STRINGIFY(TList##_insertMany)); \
        TList##_guardIndexInInsertRange(list, index, STRINGIFY(TList##_insertMany)); \
        guardNotNull(items, "items", STRINGIFY(TList##_insertMany)); \
        \
        TList##_ensureCapacity(list, list->count + count); \
        for (int i = (int)list->count - 1; i >= (int)index; i -= 1) { \
            /* Shift each item at an index >= the target index count to the right */ \
            list->items[(size_t)i + count] = list->items[i]; \
        } \
        for (size_t i = 0; i < count; i += 1) { \
            TItem const item = items[i]; \
            list->items[index + i] = item; \
        } \
        list->count += count; \
    } \
    \
    void TList##_set(TList const list, size_t const index, TItem const item) { \
        guardNotNull(list, "list", STRINGIFY(TList##_set)); \
        TList##_guardIndexInRange(list, index, STRINGIFY(TList##_set)); \
        \
        list->items[index] = item; \
    } \
    \
    void TList##_removeAt(TList const list, size_t const index) { \
        guardNotNull(list, "list", STRINGIFY(TList##_removeAt)); \
        TList##_guardIndexInRange(list, index, STRINGIFY(TList##_removeAt)); \
        \
        for (size_t i = index; i < list->count; i += 1) { \
            /* Shift each item at an index > the target index one to the left */ \
            list->items[i] = list->items[i + 1]; \
        } \
        list->count -= 1; \
    } \
    \
    void TList##_removeManyAt(TList const list, size_t const startIndex, size_t const count) { \
        guardNotNull(list, "list", STRINGIFY(TList##_removeManyAt)); \
        TList##_guardStartIndexAndCountInRange(list, startIndex, count, STRINGIFY(TList##_removeManyAt)); \
        \
        for (size_t i = startIndex; i < list->count; i += 1) { \
            /* Shift each item at an index > the start index count to the left */ \
            list->items[i] = list->items[i + count]; \
        } \
        list->count -= count; \
    } \
    \
    void TList##_clear(TList const list) { \
        guardNotNull(list, "list", STRINGIFY(TList##_clear)); \
        list->count = 0; \
    } \
    \
    void TList##_forEach(Const##TList const list, void * const state, TList##ForEachCallback const callback) { \
        guardNotNull(list, "list", STRINGIFY(TList##_forEach)); \
        \
        for (size_t i = 0; i < list->count; i += 1) { \
            TItem const item = list->items[i]; \
            callback(state, i, item); \
        } \
    } \
    \
    void TList##_forEachReverse(Const##TList const list, void * const state, TList##ForEachCallback const callback) { \
        guardNotNull(list, "list", STRINGIFY(TList##_forEachReverse)); \
        \
        for (int i = (int)list->count - 1; i >= 0; i -= 1) { \
            TItem const item = list->items[i]; \
            callback(state, (size_t)i, item); \
        } \
    } \
    \
    bool TList##_has(Const##TList const list, TItem const item) { \
        guardNotNull(list, "list", STRINGIFY(TList##_has)); \
        \
        for (size_t i = 0; i < list->count; i += 1) { \
            TItem const someItem = list->items[i]; \
            if (someItem == item) { \
                return true; \
            } \
        } \
        \
        return false; \
    } \
    \
    size_t TList##_indexOf(Const##TList const list, TItem const item) { \
        guardNotNull(list, "list", STRINGIFY(TList##_indexOf)); \
        \
        for (size_t i = 0; i < list->count; i += 1) { \
            TItem const someItem = list->items[i]; \
            if (someItem == item) { \
                return i; \
            } \
        } \
        \
        return (size_t)-1; \
    } \
    \
    size_t TList##_lastIndexOf(Const##TList const list, TItem const item) { \
        guardNotNull(list, "list", STRINGIFY(TList##_lastIndexOf)); \
        \
        for (int i = (int)list->count - 1; i >= 0; i -= 1) { \
            TItem const someItem = list->items[i]; \
            if (someItem == item) { \
                return (size_t)i; \
            } \
        } \
        \
        return (size_t)-1; \
    } \
    \
    bool TList##_findHas(Const##TList const list, void * const state, TList##FindCallback const callback) { \
        guardNotNull(list, "list", STRINGIFY(TList##_findHas)); \
        \
        for (size_t i = 0; i < list->count; i += 1) { \
            TItem const item = list->items[i]; \
            bool const found = callback(state, i, item); \
            if (found) { \
                return true; \
            } \
        } \
        \
        return false; \
    } \
    \
    TList##FindItemResult TList##_find(Const##TList const list, void * const state, TList##FindCallback const callback) { \
        guardNotNull(list, "list", STRINGIFY(TList##_find)); \
        \
        for (size_t i = 0; i < list->count; i += 1) { \
            TItem const item = list->items[i]; \
            bool const found = callback(state, i, item); \
            if (found) { \
                return TList##FindItemResult_success(item); \
            } \
        } \
        \
        return TList##FindItemResult_failure(NULL); \
    } \
    \
    size_t TList##_findIndex(Const##TList const list, void * const state, TList##FindCallback const callback) { \
        guardNotNull(list, "list", STRINGIFY(TList##_findIndex)); \
        \
        for (size_t i = 0; i < list->count; i += 1) { \
            TItem const item = list->items[i]; \
            bool const found = callback(state, i, item); \
            if (found) { \
                return i; \
            } \
        } \
        \
        return (size_t)-1; \
    } \
    \
    TList##FindItemResult TList##_findLast(Const##TList const list, void * const state, TList##FindCallback const callback) { \
        guardNotNull(list, "list", STRINGIFY(TList##_findLast)); \
        \
        for (int i = (int)list->count - 1; i >= 0; i -= 1) { \
            TItem const item = list->items[i]; \
            bool const found = callback(state, (size_t)i, item); \
            if (found) { \
                return TList##FindItemResult_success(item); \
            } \
        } \
        \
        return TList##FindItemResult_failure(NULL); \
    } \
    \
    size_t TList##_findLastIndex(Const##TList const list, void * const state, TList##FindCallback const callback) { \
        guardNotNull(list, "list", STRINGIFY(TList##_findLastIndex)); \
        \
        for (int i = (int)list->count - 1; i >= 0; i -= 1) { \
            TItem const item = list->items[i]; \
            bool const found = callback(state, (size_t)i, item); \
            if (found) { \
                return (size_t)i; \
            } \
        } \
        \
        return (size_t)-1; \
    } \
    \
    TList##Enumerator TList##_enumerate(Const##TList const list) { \
        guardNotNull(list, "list", STRINGIFY(TList##_enumerate)); \
        return TList##Enumerator_create(list, 1); \
    } \
    \
    TList##Enumerator TList##_enumerateReverse(Const##TList const list) { \
        guardNotNull(list, "list", STRINGIFY(TList##_enumerateReverse)); \
        return TList##Enumerator_create(list, -1); \
    } \
    \
    void TList##_fillArray(Const##TList const list, TItem * const array, size_t const startIndex, size_t const count) { \
        guardNotNull(list, "list", STRINGIFY(TList##_fillArray)); \
        guardNotNull(array, "array", STRINGIFY(TList##_fillArray)); \
        TList##_guardStartIndexAndCountInRange(list, startIndex, count, STRINGIFY(TList##_fillArray)); \
        \
        for (size_t i = 0; i < count; i += 1) { \
            TItem const item = list->items[i + startIndex]; \
            array[i] = item; \
        } \
    } \
    \
    static void TList##_ensureCapacity(TList const list, size_t const requiredCapacity) { \
        assert(list != NULL); \
        \
        if (requiredCapacity <= list->capacity) { \
            return; \
        } \
        \
        size_t newCapacity = list->capacity == 0 ? 4 : (list->capacity * 2); \
        while (newCapacity < requiredCapacity) { \
            newCapacity *= 2; \
        } \
        \
        list->items = safeRealloc(list->items, sizeof *list->items * newCapacity, STRINGIFY(TList##_ensureCapacity)); \
        list->capacity = newCapacity; \
    } \
    \
    static void TList##_guardIndexInRange(Const##TList const list, size_t const index, char const * const callerName) { \
        assert(list != NULL); \
        assert(callerName != NULL); \
        \
        guardFmt( \
            index < list->count, \
            "%s: Index (%zu) must be in range (count: %zu)", \
            callerName, \
            index, \
            list->count \
        ); \
    } \
    \
    static void TList##_guardIndexInInsertRange(Const##TList const list, size_t const index, char const * const callerName) { \
        assert(list != NULL); \
        assert(callerName != NULL); \
        \
        guardFmt( \
            index <= list->count, \
            "%s: Index (%zu) must be in range (count: %zu) or the next available index", \
            callerName, \
            index, \
            list->count \
        ); \
    } \
    \
    static void TList##_guardStartIndexAndCountInRange(Const##TList const list, size_t const startIndex, size_t const count, char const * const callerName) { \
        assert(list != NULL); \
        assert(callerName != NULL); \
        \
        TList##_guardIndexInRange(list, startIndex, callerName); \
        \
        size_t const endIndex = startIndex + count - 1; \
        guardFmt( \
            endIndex < list->count, \
            "%s: End index (%zu) must be within range (count: %zu)", \
            callerName, \
            endIndex, \
            list->count \
        ); \
    }
