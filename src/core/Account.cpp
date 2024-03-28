#include "Account.h"
#include "AuthMicrosoft.h"
#include "JsonExtension.h"

#include <stdio.h>
#include <string.h>

char forbidden_accesstokens[128][2048];
int forbidden_accesstokens_size = 0;
char forbidden_profileids[64][64];
int forbidden_profileids_size = 0;

json_object *bc_account_create_json(const bc_account *account) {
    json_object *json = json_object_new_object();

    json_object_object_add(json, "uuid", json_object_new_string(account->uuid));
    json_object_object_add(json, "username",
                           json_object_new_string(account->username));
    json_object_object_add(json, "account_type",
                           json_object_new_int(account->account_type));
    json_object_object_add(
        json, "minecraft_access_token",
        json_object_new_string(account->minecraft_access_token));
    json_object_object_add(json, "access_token",
                           json_object_new_string(account->access_token));
    json_object_object_add(json, "refresh_token",
                           json_object_new_string(account->refresh_token));

    return json;
}

void bc_account_select(const char *uuid) {
    json_object *json = json_object_from_file("accounts.json");
    json_object *tmp;

    json_object_object_get_ex(json, "selected", &tmp);
    json_object_set_string(tmp, uuid);

    jext_file_write("accounts.json", json);
    json_object_put(json);
}

bc_account *bc_account_select_get() {
    json_object *json = json_object_from_file("accounts.json");

    const char *selected = jext_get_string_dummy(json, "selected");
    bc_account *account = bc_account_get(selected);

    json_object_put(json);

    return account;
}

void bc_account_update(const bc_account *account) {
    json_object *account_json = bc_account_create_json(account);
    json_object *json = json_object_from_file("accounts.json");
    json_object *tmp;

    json_object_object_get_ex(json, "accounts", &tmp);

    int account_index = jext_get_string_array_index(tmp, "uuid", account->uuid);
    json_object_array_put_idx(tmp, account_index, account_json);

    jext_file_write("accounts.json", json);

    json_object_put(json);

    bc_account_register_forbidden(account, 0);
}

void bc_account_create(const bc_account *account) {
    json_object *account_json = bc_account_create_json(account);
    json_object *json = json_object_from_file("accounts.json");
    json_object *tmp;

    json_object_object_get_ex(json, "accounts", &tmp);
    json_object_array_add(tmp, account_json);

    jext_file_write("accounts.json", json);

    json_object_put(json);

    bc_account_register_forbidden(account, 1);
}

void bc_account_remove(const char *uuid) {
    json_object *json = json_object_from_file("accounts.json");
    json_object *arr, *tmp;

    json_object_object_get_ex(json, "accounts", &arr);

    const char *account_selected = jext_get_string_dummy(json, "selected");

    if (account_selected != NULL && strcmp(account_selected, uuid) == 0) {
        json_object_object_get_ex(json, "selected", &tmp);
        json_object_set_string(tmp, "");
    }

    json_object_array_del_idx(
        arr, jext_get_string_array_index(arr, "uuid", uuid), 1);

    jext_file_write("accounts.json", json);

    json_object_put(json);
}

bc_account *bc_account_get(const char *uuid) {
    json_object *json = json_object_from_file("accounts.json");
    json_object *tmp;

    if (!json_object_object_get_ex(json, "accounts", &tmp)) {
        json_object_put(json);
        return NULL;
    }

    int account_index = jext_get_string_array_index(tmp, "uuid", uuid);
    if (account_index == -1) {
        json_object_put(json);
        return NULL;
    }

    bc_account *account = (bc_account*)malloc(sizeof(bc_account));
    json_object *tmp_accounts = json_object_array_get_idx(tmp, account_index);

    snprintf(account->uuid, sizeof(account->uuid), "%s",
             jext_get_string_dummy(tmp_accounts, "uuid"));
    snprintf(account->username, sizeof(account->username), "%s",
             jext_get_string_dummy(tmp_accounts, "username"));
    snprintf(account->minecraft_access_token,
             sizeof(account->minecraft_access_token), "%s",
             jext_get_string_dummy(tmp_accounts, "minecraft_access_token"));
    snprintf(account->access_token, sizeof(account->access_token), "%s",
             jext_get_string_dummy(tmp_accounts, "access_token"));
    snprintf(account->refresh_token, sizeof(account->refresh_token), "%s",
             jext_get_string_dummy(tmp_accounts, "refresh_token"));
    account->account_type = (bc_account_type)jext_get_int(tmp_accounts, "account_type");

    json_object_put(json);

    return account;
}

bc_account_array *bc_account_list() {
    bc_account_array *accounts = (bc_account_array*)malloc(sizeof(bc_account_array));

    json_object *json = json_object_from_file("accounts.json");

    if (json == NULL) {
        json_object_put(json);
        accounts->len = 0;
        return accounts;
    }

    json_object *tmp, *tmp_arr;

    json_object_object_get_ex(json, "accounts", &tmp);

    accounts->len = json_object_array_length(tmp);

    for (int i = 0; i < accounts->len; i++) {
        tmp_arr = json_object_array_get_idx(tmp, i);

        snprintf(accounts->arr[i].uuid, sizeof(accounts->arr[i].uuid), "%s",
                 jext_get_string_dummy(tmp_arr, "uuid"));
        snprintf(accounts->arr[i].username, sizeof(accounts->arr[i].username),
                 "%s", jext_get_string_dummy(tmp_arr, "username"));
        snprintf(accounts->arr[i].minecraft_access_token,
                 sizeof(accounts->arr[i].minecraft_access_token), "%s",
                 jext_get_string_dummy(tmp_arr, "minecraft_access_token"));
        snprintf(accounts->arr[i].access_token,
                 sizeof(accounts->arr[i].access_token), "%s",
                 jext_get_string_dummy(tmp_arr, "access_token"));
        snprintf(accounts->arr[i].refresh_token,
                 sizeof(accounts->arr[i].refresh_token), "%s",
                 jext_get_string_dummy(tmp_arr, "refresh_token"));
        accounts->arr[i].account_type = (bc_account_type)jext_get_int(tmp_arr, "account_type");
    }

    json_object_put(json);

    return accounts;
}

void bc_account_refresh() {
    bc_account *account_selected = bc_account_select_get();

    if (account_selected != NULL) {
        bc_auth_microsoft(account_selected->refresh_token);

        free(account_selected);
    }
}

void bc_account_register_forbidden_all() {
    bc_account_array *arr = bc_account_list();

    for (int i = 0; i < arr->len; i++) {
        bc_account_register_forbidden(&arr->arr[i], 1);
    }

    free(arr);
}

void bc_account_register_forbidden(const bc_account *account, int uuid) {
    snprintf(forbidden_accesstokens[forbidden_accesstokens_size], 2048, "%s",
             account->minecraft_access_token);
    forbidden_accesstokens_size++;

    if (uuid) {
        snprintf(forbidden_profileids[forbidden_profileids_size], 64, "%s",
                 account->uuid);
        forbidden_profileids_size++;
    }
}
