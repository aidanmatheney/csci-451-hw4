#pragma once

#include <stdlib.h>
#include <stdarg.h>

struct StringBuilder;
typedef struct StringBuilder * StringBuilder;
typedef struct StringBuilder const * ConstStringBuilder;

StringBuilder StringBuilder_create(void);
StringBuilder StringBuilder_fromChars(char const *value, size_t count);
StringBuilder StringBuilder_fromString(char const *value);
void StringBuilder_destroy(StringBuilder builder);

char const *StringBuilder_chars(ConstStringBuilder builder);
size_t StringBuilder_length(ConstStringBuilder builder);

void StringBuilder_appendChar(StringBuilder builder, char value);
void StringBuilder_appendChars(StringBuilder builder, char const *value, size_t count);
void StringBuilder_append(StringBuilder builder, char const *value);
void StringBuilder_appendFmt(StringBuilder builder, char const *valueFormat, ...);
void StringBuilder_appendFmtVA(StringBuilder builder, char const *valueFormat, va_list valueFormatArgs);
void StringBuilder_appendLine(StringBuilder builder, char const *value);
void StringBuilder_appendLineFmt(StringBuilder builder, char const *valueFormat, ...);
void StringBuilder_appendLineFmtVA(StringBuilder builder, char const *valueFormat, va_list valueFormatArgs);

void StringBuilder_insertChar(StringBuilder builder, size_t index, char value);
void StringBuilder_insertChars(StringBuilder builder, size_t index, char const *value, size_t count);
void StringBuilder_insert(StringBuilder builder, size_t index, char const *value);
void StringBuilder_insertFmt(StringBuilder builder, size_t index, char const *valueFormat, ...);
void StringBuilder_insertFmtVA(StringBuilder builder, size_t index, char const *valueFormat, va_list valueFormatArgs);

void StringBuilder_removeAt(StringBuilder builder, size_t index);
void StringBuilder_removeManyAt(StringBuilder builder, size_t startIndex, size_t count);

char *StringBuilder_toString(ConstStringBuilder builder);
char *StringBuilder_toStringAndDestroy(StringBuilder builder);
