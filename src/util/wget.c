#include "../include/util/wget.h"

#include "../include/util/result.h"
#include "../include/util/string.h"
#include "../include/util/path.h"
#include "../include/util/file.h"
#include "../include/util/guard.h"

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

DEFINE_VOID_RESULT(WgetResult, int)

/**
 * Execute wget in quiet mode to download the file at the specified URL to the specified file path.
 *
 * @param sourceFileUrl The URL of the file to download.
 * @param destinationFilePath The file path to which to download.
 *
 * @returns A result where failure indicates a wget error. The failure error contains the wget exit code. The caller is
 *          is responsible for freeing the result.
 */
WgetResult wget(char const * const sourceFileUrl, char const * const destinationFilePath) {
    guardNotNull(sourceFileUrl, "sourceFileUrl", "wget");
    guardNotNull(destinationFilePath, "destinationFilePath", "wget");

    char * const wgetCommand = formatString(
        "wget --quiet --output-document=\"%s\" \"%s\"",
        destinationFilePath,
        sourceFileUrl
    );
    int const wgetExitCode = system(wgetCommand);
    free(wgetCommand);

    return wgetExitCode == 0 ? WgetResult_success() : WgetResult_failure(wgetExitCode);
}

DEFINE_RESULT(WgetGetStringResult, char *, int)

/**
 * Use wget to download the file at the specified URL and read its contents as text.
 *
 * @param sourceFileUrl The URL of the file whose text to be downloaded.
 *
 * @returns A result where failure indicates a wget error. The success value contains the downloaded string (the caller
 *          is responsible for freeing this memory). The failure error contains the wget exit code. The caller is
 *          responsible for freeing the result.
 */
WgetGetStringResult wgetGetString(char const * const sourceFileUrl) {
    guardNotNull(sourceFileUrl, "sourceFileUrl", "wgetGetString");

    char * const destinationFilePath = generateTempFilePath();
    WgetResult const wgetResult = wget(sourceFileUrl, destinationFilePath);
    if (!WgetResult_isSuccess(wgetResult)) {
        unlink(destinationFilePath);
        free(destinationFilePath);
        return WgetGetStringResult_failure(WgetResult_getErrorAndDestroy(wgetResult));
    }
    WgetResult_destroy(wgetResult);

    char * const destinationFileText = readAllFileText(destinationFilePath);
    unlink(destinationFilePath);
    free(destinationFilePath);

    return WgetGetStringResult_success(destinationFileText);
}
