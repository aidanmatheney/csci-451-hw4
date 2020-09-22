#pragma once

#include "./result.h"

DECLARE_VOID_RESULT(WgetResult, int)
WgetResult wget(char const *sourceFileUrl, char const *destinationFilePath);

DECLARE_RESULT(WgetGetStringResult, char *, int)
WgetGetStringResult wgetGetString(char const *sourceFileUrl);
