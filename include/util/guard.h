#pragma once

#include <stdbool.h>
#include <stdarg.h>

void guard(bool expression, char const *errorMessage);
void guardFmt(bool expression, char const *errorMessageFormat, ...);
void guardFmtVA(bool expression, char const *errorMessageFormat, va_list errorMessageFormatArgs);

void guardNotNull(void const *object, char const *paramName, char const *callerName);
