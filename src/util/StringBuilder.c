#include "../../include/util/StringBuilder.h"

#include "../../include/util/string.h"
#include "../../include/util/lists.h"
#include "../../include/util/memory.h"
#include "../../include/util/guard.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/**
 * Represents a mutable string of characters with convenience methods for string manipulation.
 */
struct StringBuilder {
    CharList chars;
};

/**
 * Create an empty StringBuilder.
 *
 * @returns The newly allocated StringBuilder. The caller is responsible for freeing this memory.
 */
StringBuilder StringBuilder_create(void) {
    StringBuilder const builder = safeMalloc(sizeof *builder, "StringBuilder_create");
    builder->chars = CharList_create();
    return builder;
}

/**
 * Create a StringBuilder initialized with the given characters.
 *
 * @param value The characters.
 * @param count The number of characters.
 *
 * @returns The newly allocated StringBuilder. The caller is responsible for freeing this memory.
 */
StringBuilder StringBuilder_fromChars(char const * const value, size_t const count) {
    guardNotNull(value, "value", "StringBuilder_fromChars");

    StringBuilder const builder = safeMalloc(sizeof *builder, "StringBuilder_fromChars");
    builder->chars = CharList_fromItems(value, count);
    return builder;
}

/**
 * Create a StringBuilder initialized with the given string.
 *
 * @param value The string.
 *
 * @returns The newly allocated StringBuilder. The caller is responsible for freeing this memory.
 */
StringBuilder StringBuilder_fromString(char const * const value) {
    guardNotNull(value, "value", "StringBuilder_fromString");
    return StringBuilder_fromChars(value, strlen(value));
}

/**
 * Free the memory associated with the StringBuilder.
 *
 * @param builder The StringBuilder instance.
 */
void StringBuilder_destroy(StringBuilder const builder) {
    guardNotNull(builder, "builder", "StringBuilder_destroy");

    CharList_destroy(builder->chars);
    free(builder);
}

/**
 * Get the characters that compose the current value.
 *
 * @param builder The StringBuilder instance.
 *
 * @returns The current value as a character array. This array is not null-terminated.
 */
char const *StringBuilder_chars(ConstStringBuilder const builder) {
    guardNotNull(builder, "builder", "StringBuilder_chars");
    return CharList_items(builder->chars);
}

/**
 * Get the length of the current value.
 *
 * @param builder The StringBuilder instance.
 *
 * @returns The string length of the current value.
 */
size_t StringBuilder_length(ConstStringBuilder const builder) {
    guardNotNull(builder, "builder", "StringBuilder_length");
    return CharList_count(builder->chars);
}

/**
 * Append the given character to the current value.
 *
 * @param builder The StringBuilder instance.
 * @param value The character.
 */
void StringBuilder_appendChar(StringBuilder const builder, char const value) {
    guardNotNull(builder, "builder", "StringBuilder_appendChar");
    CharList_add(builder->chars, value);
}

/**
 * Append the given characters to the current value.
 *
 * @param builder The StringBuilder instance.
 * @param value The characters.
 * @param count The number of characters.
 */
void StringBuilder_appendChars(StringBuilder const builder, char const * const value, size_t const count) {
    guardNotNull(builder, "builder", "StringBuilder_appendChars");
    guardNotNull(value, "value", "StringBuilder_appendChars");

    CharList_addMany(builder->chars, value, count);
}

/**
 * Append the given string to the current value.
 *
 * @param builder The StringBuilder instance.
 * @param value The string.
 */
void StringBuilder_append(StringBuilder const builder, char const * const value) {
    guardNotNull(value, "value", "StringBuilder_append");
    StringBuilder_appendChars(builder, value, strlen(value));
}

/**
 * Append the string specified by the given format and format args to the current value.
 *
 * @param builder The StringBuilder instance.
 * @param valueFormat The string format (printf).
 * @param ... The string format arguments (printf).
 */
void StringBuilder_appendFmt(StringBuilder const builder, char const * const valueFormat, ...) {
    va_list valueFormatArgs;
    va_start(valueFormatArgs, valueFormat);
    StringBuilder_appendFmtVA(builder, valueFormat, valueFormatArgs);
    va_end(valueFormatArgs);
}

/**
 * Append the string specified by the given format and format args to the current value.
 *
 * @param builder The StringBuilder instance.
 * @param valueFormat The string format (printf).
 * @param valueFormatArgs The string format arguments (printf).
 */
void StringBuilder_appendFmtVA(StringBuilder const builder, char const * const valueFormat, va_list valueFormatArgs) {
    guardNotNull(valueFormat, "valueFormat", "StringBuilder_appendFmtVA");

    char * const value = formatStringVA(valueFormat, valueFormatArgs);
    StringBuilder_append(builder, value);
    free(value);
}

/**
 * Append the given string, followed by a newline, to the current value.
 *
 * @param builder The StringBuilder instance.
 * @param value The string.
 */
void StringBuilder_appendLine(StringBuilder const builder, char const * const value) {
    guardNotNull(builder, "builder", "StringBuilder_appendLine");
    guardNotNull(value, "value", "StringBuilder_appendLine");

    CharList_addMany(builder->chars, value, strlen(value));
    CharList_add(builder->chars, '\n');
}

/**
 * Append the string specified by the given format and format args, followed by a newline, to the current value.
 *
 * @param builder The StringBuilder instance.
 * @param valueFormat The string format (printf).
 * @param ... The string format arguments (printf).
 */
void StringBuilder_appendLineFmt(StringBuilder const builder, char const * const valueFormat, ...) {
    va_list valueFormatArgs;
    va_start(valueFormatArgs, valueFormat);
    StringBuilder_appendLineFmtVA(builder, valueFormat, valueFormatArgs);
    va_end(valueFormatArgs);
}

/**
 * Append the string specified by the given format and format args, followed by a newline, to the current value.
 *
 * @param builder The StringBuilder instance.
 * @param valueFormat The string format (printf).
 * @param valueFormatArgs The string format arguments (printf).
 */
void StringBuilder_appendLineFmtVA(
    StringBuilder const builder,
    char const * const valueFormat,
    va_list valueFormatArgs
) {
    guardNotNull(valueFormat, "valueFormat", "StringBuilder_appendLineFmtVA");

    char * const value = formatStringVA(valueFormat, valueFormatArgs);
    StringBuilder_appendLine(builder, value);
    free(value);
}

/**
 * Insert the given character into the current value at the given index.
 *
 * @param builder The StringBuilder instance.
 * @param index The index.
 * @param value The character.
 */
void StringBuilder_insertChar(StringBuilder const builder, size_t const index, char const value) {
    guardNotNull(builder, "builder", "StringBuilder_insertChar");
    CharList_insert(builder->chars, index, value);
}

/**
 * Insert the given characters into the current value at the given index.
 *
 * @param builder The StringBuilder instance.
 * @param index The index.
 * @param value The characters.
 * @param count The number of characters.
 */
void StringBuilder_insertChars(
    StringBuilder const builder,
    size_t const index,
    char const * const value,
    size_t const count
) {
    guardNotNull(builder, "builder", "StringBuilder_insertChars");
    guardNotNull(value, "value", "StringBuilder_insertChars");

    CharList_insertMany(builder->chars, index, value, count);
}

/**
 * Insert the given string into the current value at the given index.
 *
 * @param builder The StringBuilder instance.
 * @param index The index.
 * @param value The string.
 */
void StringBuilder_insert(StringBuilder const builder, size_t const index, char const * const value) {
    guardNotNull(value, "value", "StringBuilder_insert");
    StringBuilder_insertChars(builder, index, value, strlen(value));
}

/**
 * Insert the string specified by the given format and format args into the current value at the given index.
 *
 * @param builder The StringBuilder instance.
 * @param index The index.
 * @param valueFormat The string format (printf).
 * @param ... The string format arguments (printf).
 */
void StringBuilder_insertFmt(StringBuilder const builder, size_t const index, char const * const valueFormat, ...) {
    va_list valueFormatArgs;
    va_start(valueFormatArgs, valueFormat);
    StringBuilder_insertFmtVA(builder, index, valueFormat, valueFormatArgs);
    va_end(valueFormatArgs);
}

/**
 * Insert the string specified by the given format and format args into the current value at the given index.
 *
 * @param builder The StringBuilder instance.
 * @param index The index.
 * @param valueFormat The string format (printf).
 * @param valueFormatArgs The string format arguments (printf).
 */
void StringBuilder_insertFmtVA(
    StringBuilder const builder,
    size_t const index,
    char const * const valueFormat,
    va_list valueFormatArgs
) {
    guardNotNull(valueFormat, "valueFormat", "StringBuilder_insertFmtVA");

    char * const value = formatStringVA(valueFormat, valueFormatArgs);
    StringBuilder_insert(builder, index, value);
    free(value);
}

/**
 * Remove the character at the given index from the current value.
 *
 * @param builder The StringBuilder instance.
 * @param index The index.
 */
void StringBuilder_removeAt(StringBuilder const builder, size_t const index) {
    guardNotNull(builder, "builder", "StringBuilder_removeAt");
    CharList_removeAt(builder->chars, index);
}

/**
 * Remove a series of characters starting at the given index from the current value.
 *
 * @param builder The StringBuilder instance.
 * @param startIndex The index at which to begin removal.
 * @param count The number of characters to remove.
 */
void StringBuilder_removeManyAt(StringBuilder const builder, size_t const startIndex, size_t const count) {
    guardNotNull(builder, "builder", "StringBuilder_removeManyAt");
    CharList_removeManyAt(builder->chars, startIndex, count);
}

/**
 * Convert the current value to a string.
 *
 * @param builder The StringBuilder instance.
 *
 * @returns A newly allocated string containing the value. The caller is responsible for freeing this memory.
 */
char *StringBuilder_toString(ConstStringBuilder const builder) {
    guardNotNull(builder, "builder", "StringBuilder_toString");

    size_t const length = CharList_count(builder->chars);
    char * const value = safeMalloc(sizeof *value * (length + 1), "StringBuilder_toString");
    CharList_fillArray(builder->chars, value, 0, length);
    value[length] = '\0';
    return value;
}

/**
 * Convert the current value to a string, then destroy the StringBuilder.
 *
 * @param builder The StringBuilder instance.
 *
 * @returns A newly allocated string containing the value. The caller is responsible for freeing this memory.
 */
char *StringBuilder_toStringAndDestroy(StringBuilder const builder) {
    guardNotNull(builder, "builder", "StringBuilder_toStringAndDestroy");

    char * const valueString = StringBuilder_toString(builder);
    StringBuilder_destroy(builder);
    return valueString;
}
