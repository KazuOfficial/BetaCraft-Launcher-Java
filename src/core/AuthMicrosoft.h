#ifndef BC_AUTHMICROSOFT_H
#define BC_AUTHMICROSOFT_H

#include "cpr/payload.h"
typedef struct bc_auth_microsoftDeviceResponse {
    char user_code[512];
    char device_code[1024];
    char verification_uri[128];
    int expires_in;
    int interval;
} bc_auth_microsoftDeviceResponse;

typedef struct bc_auth_microsoftResponse {
    char access_token[2048];
    char refresh_token[2048];
} bc_auth_microsoftResponse;

typedef struct bc_auth_XBLResponse {
    char token[2048];
    char uhs[2048];
} bc_auth_XBLResponse;

typedef struct bc_auth_minecraftAccount {
    char id[512];
    char username[32];
} bc_auth_minecraftAccount;

void bc_auth_microsoft_handle_device_flow(
    const bc_auth_microsoftDeviceResponse *device_res);
bc_auth_microsoftDeviceResponse *bc_auth_microsoft_device();

bc_auth_microsoftResponse *
bc_auth_microsoft_device_token(const bc_auth_microsoftDeviceResponse *res);
bc_auth_XBLResponse *bc_auth_microsoft_xbl(const char *access_token);
int bc_auth_microsoft_check_token(const cpr::Payload& payload,
                                  bc_auth_microsoftResponse *res);
char *bc_auth_microsoft_xsts(const char *xbl_token);
void bc_auth_microsoft(const char *refresh_token);
char *bc_auth_minecraft(const char *uhs, const char *xsts_token);
bc_auth_minecraftAccount *bc_auth_minecraft_profile(const char *token);

#endif
