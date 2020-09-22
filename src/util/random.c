#include "../include/util/random.h"

#include "../include/util/time.h"
#include "../include/util/guard.h"

#include <stdlib.h>
#include <stdbool.h>

static bool randomInitialized = false;

static void ensureRandomInitialized(void);

/**
 * Initialize the random number generator using the given seed value. If this is never called, the random number
 * generator will automatically be initialized with the time it is first used.
 *
 * @param seed A number used to calculate a starting value for the pseudo-random number sequence.
 */
void initializeRandom(unsigned int const seed) {
    srand(seed);
    randomInitialized = true;
}

/**
 * Generate the next random integer from within the given range.
 *
 * @param minInclusive The inclusive lower bound of the random number returned.
 * @param maxExclusive The exclusive upper bound of the random number returned. maxExclusive must be greater than
 *                     minInclusive.
 *
 * @returns The random integer.
 */
int randomInt(int const minInclusive, int const maxExclusive) {
    guardFmt(
        maxExclusive > minInclusive,
        "randomInt: maxExclusive (%d) must be greater than minInclusive (%d)",
        maxExclusive,
        minInclusive
    );

    ensureRandomInitialized();
    return rand() % (maxExclusive - minInclusive) + minInclusive;
}

static void ensureRandomInitialized(void) {
    if (randomInitialized) {
        return;
    }

    initializeRandom((unsigned int)safeTime("ensureRandomInitialized"));
    randomInitialized = true;
}
