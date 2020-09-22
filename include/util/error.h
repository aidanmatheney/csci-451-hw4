#pragma once

#include <stdarg.h>

void abortWithError(char const *errorMessage);
void abortWithErrorFmt(char const *errorMessageFormat, ...);
void abortWithErrorFmtVA(char const *errorMessageFormat, va_list errorMessageFormatArgs);
