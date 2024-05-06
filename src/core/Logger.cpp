#include "Logger.h"

#include "Account.h"
#include "StringUtils.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#define OUT_FORMAT_SIZE 256
#define PRINT_BUFFER_SIZE 8192
#define TIME_STRING_SIZE 9

void bc_log(const char *format, ...) {
    FILE *file = NULL;
    file = fopen("betacraft.log", "a");

    va_list args = {};
    va_list args2 = {};

    va_start(args, format);
    va_copy(args2, args);

    char timeString[TIME_STRING_SIZE];
    struct tm *tm_info = NULL;
    time_t now = 0;

    time_t _cur_calendar_time = time(&now);
    tm_info = localtime(&now);

    size_t _copied_chars_len =
        strftime(timeString, sizeof(timeString), "%H:%M:%S", tm_info);

    char outFormat[OUT_FORMAT_SIZE];
    snprintf(outFormat, sizeof(outFormat), "[%s] %s", timeString, format);

    char printbuffer[PRINT_BUFFER_SIZE];
    vsprintf(printbuffer, outFormat, args);

    for (int i = 0; i < forbidden_accesstokens_size; i++) {
        repl_str(printbuffer, forbidden_accesstokens[i], "<ACCESS TOKEN>");
    }

    for (int i = 0; i < forbidden_profileids_size; i++) {
        repl_str(printbuffer, forbidden_profileids[i], "<PROFILE ID>");
    }

    // TODO: make a list of forbidden strings at startup and exclude them from
    // logging
    printf("%s", printbuffer);
    int _written_chars_len = fprintf(file, "%s", printbuffer);

    va_end(args);
    va_end(args2);

    if (fclose(file) != 0) {
        printf("%s\n", "LOGGER ERROR");
    }
}
