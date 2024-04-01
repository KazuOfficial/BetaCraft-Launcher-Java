#include "ProcessHandler.h"
#include "Game.h"
#include "StringUtils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include "WindowsProcessHandler.h"
#elif defined(__APPLE__) || defined(__linux__)
#include <fcntl.h>
#include <sys/wait.h>
#include <unistd.h>
#endif

char *bc_process_log = NULL;

#if defined(__APPLE__) || defined(__linux__)
void bc_unixprocess_create(bc_process_args *args, int output, bc_account *acc) {
    args->arr[args->len] = NULL;

    int fd[2];
    if (output) {
        pipe(fd);
    }

    int pid = fork();

    if (pid == 0) {
        if (output) {
            close(fd[0]);
            dup2(fd[1], 1); // stdout
            dup2(fd[1], 2); // stderr
            close(fd[1]);
        }

        execvp(args->arr[0], args->arr);
    } else if (output) {
        bc_game_run_progress.progress = 100;
        char buffer[1024];
        close(fd[1]);

        while (read(fd[0], buffer, sizeof(buffer)) != 0) {
            if (acc->minecraft_access_token[0] != '-' ||
                strcmp(acc->uuid, DEMO_ACCOUNT_UUID) != 0) {
                repl_str(buffer, acc->minecraft_access_token, "<ACCESS TOKEN>");
                repl_str(buffer, acc->uuid, "<PROFILE ID>");
            }

            write(1, buffer, strlen(buffer));

            if (bc_process_log != NULL) {
                char *newLog =
                    (char*)malloc(strlen(bc_process_log) + strlen(buffer) + 1);
                strcpy(newLog, bc_process_log);
                strcat(newLog, buffer);

                free(bc_process_log);
                bc_process_log = newLog;
            } else {
                bc_process_log = (char*)malloc(strlen(buffer) + 1);
                strcpy(bc_process_log, buffer);
            }

            memset(buffer, 0, sizeof(buffer));
        }
    }

    bc_game_run_progress.progress = 100;
    if (!output) {
        waitpid(pid, NULL, 0);
    }
}
#endif

void bc_process_create(bc_process_args *args) {
#ifdef _WIN32
    bc_winprocess_create(args);
#elif defined(__APPLE__) || defined(__linux__)
    bc_unixprocess_create(args, 0, NULL);
#endif
}

void bc_process_create_log(bc_process_args *args, bc_account *account) {
    if (bc_process_log != NULL) {
        free(bc_process_log);
        bc_process_log = NULL;
    }

#ifdef _WIN32
    bc_winprocess_create_log(args, account);
#elif defined(__APPLE__) || defined(__linux__)
    bc_unixprocess_create(args, 1, account);
#endif
}
