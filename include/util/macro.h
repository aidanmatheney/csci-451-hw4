#pragma once

/**
 * Turn the given macro token into a string literal.
 *
 * @param macroToken The macro token.
 *
 * @returns The string literal.
 */
#define STRINGIFY(macroToken) #macroToken

/**
 * Get the length of the given compile-time array.
 *
 * @param array The array.
 *
 * @returns The length number literal.
 */
#define ARRAY_LENGTH(array) (sizeof (array) / sizeof (array)[0])

/**
 * Get a stack-allocated mutable string from the given string literal.
 *
 * @param stringLiteral The string literal.
 *
 * @returns The `char [length + 1]`-typed stack-allocated string.
*/
#define MUTABLE_STRING(stringLiteral) ((char [ARRAY_LENGTH(stringLiteral)]){ stringLiteral })
