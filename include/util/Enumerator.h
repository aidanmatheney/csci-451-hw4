#pragma once

#include <stdbool.h>

#define DECLARE_ENUMERATOR(TEnumerator, TItem) \
    struct TEnumerator; \
    typedef struct TEnumerator * TEnumerator; \
    typedef struct TEnumerator const * Const##TEnumerator; \
    \
    void TEnumerator##_destroy(TEnumerator enumerator); \
    \
    bool TEnumerator##_moveNext(TEnumerator enumerator); \
    TItem TEnumerator##_current(Const##TEnumerator enumerator); \
    void TEnumerator##_reset(TEnumerator enumerator);
