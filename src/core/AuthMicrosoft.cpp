#include "AuthMicrosoft.h"

#include "Account.h"
#include "JsonExtension.h"
#include "Logger.h"
#include "Network.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
#include <windows.h>

#define sleep Sleep
#define sleepMultiplier 1000
#elif defined(__linux__) || defined(__APPLE__)
#include <unistd.h>
#define sleepMultiplier 1
#endif

const char API_MICROSOFT_DEVICECODE[] =
    "https://login.microsoftonline.com/consumers/oauth2/v2.0/devicecode";
const char API_MICROSOFT_TOKEN[] =
    "https://login.microsoftonline.com/consumers/oauth2/v2.0/token";

const char API_XBOX_XBL[] = "https://user.auth.xboxlive.com/user/authenticate";
const char API_XBOX_XSTS[] = "https://xsts.auth.xboxlive.com/xsts/authorize";
const char API_MINECRAFT[] =
    "https://api.minecraftservices.com/authentication/login_with_xbox";
const char API_MINECRAFT_PROFILE[] =
    "https://api.minecraftservices.com/minecraft/profile";

bc_auth_microsoftResponse *
bc_auth_microsoft_refresh_token(const char *refresh_token);

void bc_auth_microsoft_handle_device_flow(
    const bc_auth_microsoftDeviceResponse *device_res) {
    bc_auth_microsoftResponse *microsoft =
        bc_auth_microsoft_device_token(device_res);
    bc_auth_microsoft(microsoft->refresh_token);
    free(microsoft);
}

void bc_auth_microsoft(const char *refresh_token) {
    if (API_MICROSOFT_CLIENT_ID[0] == '\0')
        return;

    bc_auth_microsoftResponse *microsoft =
        bc_auth_microsoft_refresh_token(refresh_token);

    if (microsoft->access_token[0] == '\0' ||
        microsoft->refresh_token[0] == '\0') {
        bc_log("%s\n", "Error: bc_auth_microsoft_token - access_token or "
                       "refresh_token is empty");
        return;
    }

    bc_auth_XBLResponse *xbl = bc_auth_microsoft_xbl(microsoft->access_token);

    if (xbl->token[0] == '\0' || xbl->uhs[0] == '\0') {
        bc_log("%s\n", "Error: bc_auth_microsoft_xbl - token or uhs is empty");
        return;
    }

    char *token_xsts = bc_auth_microsoft_xsts(xbl->token);

    if (token_xsts == NULL) {
        bc_log("%s\n", "Error: bc_auth_microsoft_xsts - token_xsts is empty");
        return;
    }

    char *token_minecraft = bc_auth_minecraft(xbl->uhs, token_xsts);

    if (token_minecraft == NULL) {
        bc_log("%s\n", "Error: bc_auth_minecraft - token_minecraft is empty");
        return;
    }

    bc_auth_minecraftAccount *profile =
        bc_auth_minecraft_profile(token_minecraft);

    if (profile->id[0] == '\0' || profile->username[0] == '\0') {
        bc_log("%s\n",
               "Error: bc_auth_minecraft_profile - id or username is empty");
        return;
    }

    bc_account *account = bc_account_get(profile->id);

    if (account == NULL) {
        bc_account *new_account = (bc_account*)malloc(sizeof(bc_account));

        snprintf(new_account->username, sizeof(new_account->username), "%s",
                 profile->username);
        snprintf(new_account->uuid, sizeof(new_account->uuid), "%s",
                 profile->id);
        snprintf(new_account->access_token, sizeof(new_account->access_token),
                 "%s", microsoft->access_token);
        snprintf(new_account->minecraft_access_token,
                 sizeof(new_account->minecraft_access_token), "%s",
                 token_minecraft);
        snprintf(new_account->refresh_token, sizeof(new_account->refresh_token),
                 "%s", microsoft->refresh_token);
        new_account->account_type = BC_ACCOUNT_MICROSOFT;

        bc_account_create(new_account);
        free(new_account);
    } else {
        snprintf(account->username, sizeof(account->username), "%s",
                 profile->username);
        snprintf(account->access_token, sizeof(account->access_token), "%s",
                 microsoft->access_token);
        snprintf(account->minecraft_access_token,
                 sizeof(account->minecraft_access_token), "%s",
                 token_minecraft);
        snprintf(account->refresh_token, sizeof(account->refresh_token), "%s",
                 microsoft->refresh_token);

        bc_account_update(account);
        free(account);
    }

    bc_account_select(profile->id);

    free(microsoft);
    free(xbl);
    free(token_xsts);
    free(token_minecraft);
    free(profile);
}

bc_auth_microsoftDeviceResponse *bc_auth_microsoft_device() {
    bc_auth_microsoftDeviceResponse *res =
        (bc_auth_microsoftDeviceResponse*)malloc(sizeof(bc_auth_microsoftDeviceResponse));

    char data[100];
    snprintf(data, sizeof(data),
             "client_id=%s&scope=XboxLive.signin offline_access",
             API_MICROSOFT_CLIENT_ID);

    char *response =
        bc_network_post(API_MICROSOFT_DEVICECODE, data,
                        "Content-Type: application/x-www-form-urlencoded");
    json_object *json = json_tokener_parse(response);
    free(response);

    snprintf(res->user_code, sizeof(res->user_code), "%s",
             jext_get_string_dummy(json, "user_code"));
    snprintf(res->device_code, sizeof(res->device_code), "%s",
             jext_get_string_dummy(json, "device_code"));
    snprintf(res->verification_uri, sizeof(res->verification_uri), "%s",
             jext_get_string_dummy(json, "verification_uri"));
    res->expires_in = jext_get_int(json, "expires_in");
    res->interval = jext_get_int(json, "interval");

    json_object_put(json);

    return res;
}

int bc_auth_microsoft_check_token(const char *data,
                                  bc_auth_microsoftResponse *res) {
    char *response =
        bc_network_post(API_MICROSOFT_TOKEN, data,
                        "Content-Type: application/x-www-form-urlencoded");
    json_object *json = json_tokener_parse(response);
    free(response);

    const char *error = jext_get_string_dummy(json, "error");

    if (error == NULL || strcmp(error, "") == 0) {
        snprintf(res->access_token, sizeof(res->access_token), "%s",
                 jext_get_string_dummy(json, "access_token"));
        snprintf(res->refresh_token, sizeof(res->refresh_token), "%s",
                 jext_get_string_dummy(json, "refresh_token"));
        return 0;
    } else if (strcmp(error, "authorization_pending") == 0) {
        return 1;
    }

    bc_log("Error: bc_auth_microsoft_check_token - \n\"%s\"\n", error);
    return 0;
}

bc_auth_microsoftResponse *
bc_auth_microsoft_refresh_token(const char *refresh_token) {
    bc_auth_microsoftResponse *res = (bc_auth_microsoftResponse*)malloc(sizeof(bc_auth_microsoftResponse));

    char *error;
    char data[2048];
    snprintf(data, sizeof(data),
             "grant_type=refresh_token&client_id=%s&refresh_token=%s",
             API_MICROSOFT_CLIENT_ID, refresh_token);

    if (!bc_auth_microsoft_check_token(data, res)) {
        // TODO: handle (malformed request/token? no connection? api down?)
        return res;
    }

    return res;
}

bc_auth_microsoftResponse *
bc_auth_microsoft_device_token(const bc_auth_microsoftDeviceResponse *dev) {
    bc_auth_microsoftResponse *res = (bc_auth_microsoftResponse*)malloc(sizeof(bc_auth_microsoftResponse));

    char *error;
    char data[2048];
    snprintf(data, sizeof(data),
             "grant_type=urn:ietf:params:oauth:grant-type:device_code&client_"
             "id=%s&device_code=%s",
             API_MICROSOFT_CLIENT_ID, dev->device_code);

    // while authorization pending
    while (bc_auth_microsoft_check_token(data, res)) {
        sleep(dev->interval * sleepMultiplier);
    }

    return res;
}

bc_auth_XBLResponse *bc_auth_microsoft_xbl(const char *access_token) {
    bc_auth_XBLResponse *res = (bc_auth_XBLResponse*)malloc(sizeof(bc_auth_XBLResponse));

    json_object *data = json_object_new_object();
    json_object *properties = json_object_new_object();

    char rps_ticket[4096];
    snprintf(rps_ticket, sizeof(rps_ticket), "d=%s", access_token);

    json_object_object_add(properties, "AuthMethod",
                           json_object_new_string("RPS"));
    json_object_object_add(properties, "SiteName",
                           json_object_new_string("user.auth.xboxlive.com"));
    json_object_object_add(properties, "RpsTicket",
                           json_object_new_string(rps_ticket));

    json_object_object_add(data, "Properties", properties);
    json_object_object_add(data, "RelyingParty",
                           json_object_new_string("http://auth.xboxlive.com"));
    json_object_object_add(data, "TokenType", json_object_new_string("JWT"));

    char *response =
        bc_network_post(API_XBOX_XBL, json_object_to_json_string(data),
                        "Content-Type: application/json");
    json_object *json = json_tokener_parse(response);
    json_object *tmp;
    free(response);

    snprintf(res->token, sizeof(res->token), "%s",
             jext_get_string_dummy(json, "Token"));

    json_object_object_get_ex(json, "DisplayClaims", &tmp);
    json_object_object_get_ex(tmp, "xui", &tmp);

    json_object *uhs = json_object_array_get_idx(tmp, 0);
    snprintf(res->uhs, sizeof(res->uhs), "%s",
             jext_get_string_dummy(uhs, "uhs"));

    json_object_put(json);
    json_object_put(data);

    return res;
}

char *bc_auth_microsoft_xsts(const char *xbl_token) {
    json_object *data = json_object_new_object();
    json_object *properties = json_object_new_object();
    json_object *user_tokens = json_object_new_array();

    json_object_array_add(user_tokens, json_object_new_string(xbl_token));
    json_object_object_add(properties, "SandboxId",
                           json_object_new_string("RETAIL"));
    json_object_object_add(properties, "UserTokens", user_tokens);

    json_object_object_add(data, "Properties", properties);
    json_object_object_add(
        data, "RelyingParty",
        json_object_new_string("rp://api.minecraftservices.com/"));
    json_object_object_add(data, "TokenType", json_object_new_string("JWT"));

    char *response =
        bc_network_post(API_XBOX_XSTS, json_object_to_json_string(data),
                        "Content-Type: application/json");
    json_object *json = json_tokener_parse(response);
    free(response);

    char *token_xsts = jext_get_string(json, "Token");

    json_object_put(json);
    json_object_put(data);

    return token_xsts;
}

char *bc_auth_minecraft(const char *uhs, const char *xsts_token) {
    json_object *data = json_object_new_object();

    char identity_token[4096];
    snprintf(identity_token, sizeof(identity_token), "XBL3.0 x=%s;%s", uhs,
             xsts_token);

    json_object_object_add(data, "identityToken",
                           json_object_new_string(identity_token));

    char *response =
        bc_network_post(API_MINECRAFT, json_object_to_json_string(data),
                        "Content-Type: application/json");
    json_object *json = json_tokener_parse(response);
    free(response);

    char *token = jext_get_string(json, "access_token");
    bc_log("%s\n", "Processed Minecraft auth successfully");

    json_object_put(data);
    json_object_put(json);

    return token;
}

bc_auth_minecraftAccount *bc_auth_minecraft_profile(const char *token) {
    bc_auth_minecraftAccount *res = (bc_auth_minecraftAccount*)malloc(sizeof(bc_auth_minecraftAccount));

    char auth[4096];
    snprintf(auth, sizeof(auth), "Authorization: Bearer %s", token);

    char *response = bc_network_get(API_MINECRAFT_PROFILE, auth);
    json_object *json = json_tokener_parse(response);
    free(response);

    snprintf(res->id, sizeof(res->id), "%s", jext_get_string_dummy(json, "id"));
    snprintf(res->username, sizeof(res->username), "%s",
             jext_get_string_dummy(json, "name"));

    json_object_put(json);

    return res;
}
