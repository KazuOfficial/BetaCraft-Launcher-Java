#ifdef _WIN32
#include "WindowsProcessHandler.h"

#include "Game.h"
#include "Logger.h"
#include "StringUtils.h"

#include <stdio.h>
#include <windows.h>

#define BUFSIZE 4096

HANDLE g_hChildStd_OUT_Rd = NULL;
HANDLE g_hChildStd_OUT_Wr = NULL;
HANDLE g_hChildStd_ERR_Rd = NULL;
HANDLE g_hChildStd_ERR_Wr = NULL;

PROCESS_INFORMATION CreateChildProcess(bc_process_args *args);
void ReadFromPipe(PROCESS_INFORMATION, bc_account *account);

char *bc_winprocess_get_args(bc_process_args *args) {
    char *cmd = malloc(args->size + (args->len * 3));
    strcpy(cmd, "\"");
    strcat(cmd, args->arr[0]);
    strcat(cmd, "\"");

    for (int i = 1; i < args->len; i++) {
        strcat(cmd, " \"");
        strcat(cmd, args->arr[i]);
        strcat(cmd, "\"");
    }

    return cmd;
}

void bc_winprocess_create(bc_process_args *args) {
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    char *cmd = bc_winprocess_get_args(args);

    if (!CreateProcess(NULL, cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
        free(cmd);
        bc_log("CreateProcess failed (%d).\n", GetLastError());
        return;
    }

    free(cmd);
    bc_game_run_progress.progress = 100;
    WaitForSingleObject(pi.hProcess, INFINITE);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void bc_winprocess_create_log(bc_process_args *args, bc_account *account) {
    SECURITY_ATTRIBUTES sa;

    sa.nLength = sizeof(SECURITY_ATTRIBUTES);
    sa.bInheritHandle = TRUE;
    sa.lpSecurityDescriptor = NULL;

    if (!CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &sa, 0)) {
        exit(1);
    }

    if (!SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0)) {
        exit(1);
    }

    if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &sa, 0)) {
        exit(1);
    }

    if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0)) {
        exit(1);
    }

    PROCESS_INFORMATION piProcInfo = CreateChildProcess(args);
    bc_game_run_progress.progress = 100;
    ReadFromPipe(piProcInfo, account);
}

PROCESS_INFORMATION CreateChildProcess(bc_process_args *args) {
    PROCESS_INFORMATION piProcInfo;
    STARTUPINFO siStartInfo;
    int bSuccess = FALSE;

    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
    siStartInfo.cb = sizeof(STARTUPINFO);
    siStartInfo.hStdError = g_hChildStd_ERR_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    char *cmd = bc_winprocess_get_args(args);

    bSuccess = CreateProcess(NULL, cmd, NULL, NULL, TRUE, 0, NULL, NULL,
                             &siStartInfo, &piProcInfo);
    CloseHandle(g_hChildStd_ERR_Wr);
    CloseHandle(g_hChildStd_OUT_Wr);

    free(cmd);

    if (!bSuccess) {
        bc_log("%s\n", "Couldn't create a child process");
        exit(1);
    }

    return piProcInfo;
}

void ReadFromPipe(PROCESS_INFORMATION piProcInfo, bc_account *account) {
    DWORD dwRead;
    CHAR chBuf[BUFSIZE];
    int bSuccess = FALSE;

    for (;;) {
        bSuccess = ReadFile(g_hChildStd_OUT_Rd, chBuf, BUFSIZE, &dwRead, NULL);
        if (!bSuccess || dwRead == 0)
            break;
        chBuf[dwRead] = '\0';

        if (account->minecraft_access_token[0] != '-' ||
            strcmp(account->uuid, DEMO_ACCOUNT_UUID) != 0) {
            repl_str(chBuf, account->minecraft_access_token, "<ACCESS TOKEN>");
            repl_str(chBuf, account->uuid, "<PROFILE ID>");
        }

        if (bc_process_log != NULL) {
            char *newLog = malloc(strlen(bc_process_log) + dwRead + 1);
            strcpy(newLog, bc_process_log);
            strcat(newLog, chBuf);

            free(bc_process_log);
            bc_process_log = newLog;
        } else {
            bc_process_log = malloc(dwRead + 1);
            strcpy(bc_process_log, chBuf);
        }
    }
}
#endif
