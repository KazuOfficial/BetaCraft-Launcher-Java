#include "StringUtils.h"
#include "Betacraft.h"
#include "Logger.h"

#include <assert.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void repl_str(char *source, char *substring, char *with) {
    char *substring_source = strstr(source, substring);

    if (substring_source == NULL) {
        return;
    }

    memmove(substring_source + strlen(with),
            substring_source + strlen(substring),
            strlen(substring_source) - strlen(substring) + 1);

    memcpy(substring_source, with, strlen(with));
}

char *repl_str_alloc(char *source, char *substring, char *with,
                     int freeSource) {
    char *ret = (char*)malloc(strlen(source) + strlen(with) + 1);
    strcpy(ret, source);

    if (freeSource) {
        free(source);
    }

    repl_str(ret, substring, with);

    return ret;
}

int count_substring(char *s, char c) {
    char buffer[1024];
    assert(strlen(s) < sizeof(buffer));

    snprintf(buffer, sizeof(buffer), "%s", s);

    return *buffer == '\0' ? 0
                           : count_substring(buffer + 1, c) + (*buffer == c);
}

int str_starts_with(char *string, char *prefix) {
    return strncmp(prefix, string, strlen(prefix)) == 0;
}

int str_ends_with(char *string, char *suffix) {
    char *dot = strrchr(string, '.');

    if (dot == NULL)
        return 0;

    return strcmp(dot, suffix) == 0;
}
