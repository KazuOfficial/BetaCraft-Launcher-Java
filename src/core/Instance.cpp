#include "Instance.h"
#include "FileSystem.h"
#include "Game.h"
#include "JavaInstallations.h"
#include "JsonExtension.h"
#include "Mod.h"
#include "Version.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INSTANCE_HEIGHT_DEFAULT_SIZE 480
#define INSTANCE_WIDTH_DEFAULT_SIZE 854

char *bc_instance_get_path(const char *instance_name) {
    char path[PATH_MAX];
    snprintf(path, sizeof(path), "instances/%s/bc_instance.json",
             instance_name);

    return bc_file_make_absolute_path(path);
}

void bc_instance_fill_object_from_json(bc_instance *instance,
                                       const char *instance_path,
                                       json_object *json) {
    snprintf(instance->name, sizeof(instance->name), "%s",
             jext_get_string_dummy(json, "name"));
    snprintf(instance->version, sizeof(instance->version), "%s",
             jext_get_string_dummy(json, "version"));
    snprintf(instance->jvm_args, sizeof(instance->jvm_args), "%s",
             jext_get_string_dummy(json, "jvm_args"));
    snprintf(instance->program_args, sizeof(instance->program_args), "%s",
             jext_get_string_dummy(json, "program_args"));
    snprintf(instance->server_ip, sizeof(instance->server_ip), "%s",
             jext_get_string_dummy(json, "server_ip"));
    snprintf(instance->server_port, sizeof(instance->server_port), "%s",
             jext_get_string_dummy(json, "server_port"));

    instance->height = jext_get_int(json, "height");
    instance->width = jext_get_int(json, "width");
    instance->maximized = jext_get_int(json, "maximized");
    instance->fullscreen = jext_get_int(json, "fullscreen");
    instance->show_log = jext_get_int(json, "show_log");
    instance->keep_open = jext_get_int(json, "keep_open");
    instance->join_server = jext_get_int(json, "join_server");

    char *absPath = bc_file_absolute_path(instance_path);
    snprintf(instance->path, sizeof(instance->path), "%s", absPath);
    free(absPath);
}

json_object *bc_instance_create_default_config(const char *name,
                                               const char *version) {
    json_object *config = json_object_new_object();

    json_object_object_add(config, "name", json_object_new_string(name));
    json_object_object_add(config, "version", json_object_new_string(version));
    json_object_object_add(
        config, "jvm_args",
        json_object_new_string(
            "-Xmx1G\n-XX:HeapDumpPath=java.exe_minecraft.exe.heapdump"));
    json_object_object_add(config, "program_args", json_object_new_string(""));
    json_object_object_add(config, "height",
                           json_object_new_int(INSTANCE_HEIGHT_DEFAULT_SIZE));
    json_object_object_add(config, "width",
                           json_object_new_int(INSTANCE_WIDTH_DEFAULT_SIZE));
    json_object_object_add(config, "maximized", json_object_new_boolean(0));
    json_object_object_add(config, "fullscreen", json_object_new_boolean(0));
    json_object_object_add(config, "show_log", json_object_new_boolean(0));
    json_object_object_add(config, "keep_open", json_object_new_boolean(0));
    json_object_object_add(config, "join_server", json_object_new_boolean(0));
    json_object_object_add(config, "server_ip", json_object_new_string(""));
    json_object_object_add(config, "server_port", json_object_new_string(""));
    json_object_object_add(config, "mods", json_object_new_array());

    return config;
}

json_object *bc_instance_create_config(const bc_instance *instance) {
    json_object *json = json_object_from_file(instance->path);
    json_object *tmp = NULL;

    jext_replace_or_create_option_str(json, "name", instance->name);
    jext_replace_or_create_option_str(json, "version", instance->version);
    jext_replace_or_create_option_str(json, "jvm_args", instance->jvm_args);
    jext_replace_or_create_option_str(json, "program_args",
                                      instance->program_args);
    jext_replace_or_create_option_int(json, "height", instance->height);
    jext_replace_or_create_option_int(json, "width", instance->width);
    jext_replace_or_create_option_boolean(json, "maximized",
                                          instance->maximized);
    jext_replace_or_create_option_boolean(json, "fullscreen",
                                          instance->fullscreen);
    jext_replace_or_create_option_boolean(json, "show_log", instance->show_log);
    jext_replace_or_create_option_boolean(json, "keep_open",
                                          instance->keep_open);
    jext_replace_or_create_option_boolean(json, "join_server",
                                          instance->join_server);
    jext_replace_or_create_option_str(json, "server_ip", instance->server_ip);
    jext_replace_or_create_option_str(json, "server_port",
                                      instance->server_port);

    return json;
}

void bc_instance_group_create(const char *name) {
    json_object *settings = json_object_from_file("settings.json");
    json_object *tmp = NULL;
    json_object *instance_tmp = NULL;
    json_object *group_object = json_object_new_object();

    json_object_object_get_ex(settings, "instance", &instance_tmp);
    json_object_object_get_ex(instance_tmp, "grouped", &tmp);

    json_object_object_add(group_object, "group_name",
                           json_object_new_string(name));
    json_object_object_add(group_object, "instances", json_object_new_array());
    json_object_array_add(tmp, group_object);

    jext_file_write("settings.json", settings);
    json_object_put(settings);
}

static bool bc_instance_name_char_meets_constraints(const char name_char) {
    if (!isalnum(name_char) && !isspace(name_char) && name_char != '-' &&
        name_char != '_' && name_char != '.') {
        return false;
    }

    return true;
}

bool bc_instance_validate_name(const char *name) {
    if (name[0] == '.') {
        return false;
    }

    for (int i = 0; i < strlen(name); i++) {
        char name_char = name[i];

        if (!bc_instance_name_char_meets_constraints(name_char)) {
            return false;
        }
    }

    return true;
}

void bc_instance_move(bc_instance_array *standard,
                      bc_instance_group_array *grouped,
                      const char *instanceSelected) {
    json_object *json = json_object_from_file("settings.json");
    json_object *instance = json_object_new_object();
    json_object *tmp = NULL;
    json_object *tmpArr = NULL;
    json_object *tmpArrGrouped = NULL;

    json_object_object_del(json, "instance");

    json_object_object_add(instance, "selected",
                           json_object_new_string(instanceSelected));
    json_object_object_add(instance, "standalone", json_object_new_array());
    json_object_object_add(instance, "grouped", json_object_new_array());

    json_object_object_add(json, "instance", instance);

    json_object_object_get_ex(json, "instance", &tmp);
    json_object_object_get_ex(tmp, "standalone", &tmpArr);

    for (int i = 0; i < standard->len; i++) {
        json_object_array_add(tmpArr,
                              json_object_new_string(standard->arr[i].path));
    }

    json_object_object_get_ex(tmp, "grouped", &tmpArr);

    for (int i = 0; i < grouped->len; i++) {
        json_object *group = json_object_new_object();
        json_object_object_add(
            group, "group_name",
            json_object_new_string(grouped->arr[i].group_name));
        json_object_object_add(group, "instances", json_object_new_array());

        json_object_object_get_ex(group, "instances", &tmpArrGrouped);

        for (int j = 0; j < grouped->arr[i].len; j++) {
            json_object_array_add(
                tmpArrGrouped,
                json_object_new_string(grouped->arr[i].instances[j].path));
        }

        json_object_array_add(tmpArr, group);
    }

    jext_file_write("settings.json", json);
    json_object_put(json);
}

bc_instance_group_name_array *bc_instance_group_name_get_all() {
    bc_instance_group_name_array *group_array =
        (bc_instance_group_name_array*)malloc(sizeof(bc_instance_group_name_array));

    json_object *settings = json_object_from_file("settings.json");

    if (settings == NULL) {
        group_array->len = 0;
        json_object_put(settings);
        return group_array;
    }

    json_object *tmp = NULL;
    json_object *instance_tmp = NULL;
    json_object *group_tmp = NULL;
    json_object_object_get_ex(settings, "instance", &instance_tmp);
    json_object_object_get_ex(instance_tmp, "grouped", &tmp);

    group_array->len = json_object_array_length(tmp);

    for (int i = 0; i < group_array->len; i++) {
        group_tmp = json_object_array_get_idx(tmp, i);
        snprintf(group_array->arr[i], sizeof(group_array->arr[i]), "%s",
                 jext_get_string_dummy(group_tmp, "group_name"));
    }

    json_object_put(settings);

    return group_array;
}

void bc_instance_update_settings(const char *instance_path,
                                 const char *group_name) {
    json_object *settings = json_object_from_file("settings.json");
    json_object *instance_tmp = NULL;
    json_object *tmp = NULL;
    json_object *val_tmp = NULL;
    json_object *val_arr_tmp = NULL;

    json_object_object_get_ex(settings, "instance", &instance_tmp);

    if (group_name == NULL) {
        json_object_object_get_ex(instance_tmp, "standalone", &tmp);
        json_object_array_add(tmp, json_object_new_string(instance_path));
    } else {
        json_object_object_get_ex(instance_tmp, "grouped", &tmp);

        for (int i = 0; i < json_object_array_length(tmp); i++) {
            val_tmp = json_object_array_get_idx(tmp, i);

            if (strcmp(jext_get_string_dummy(val_tmp, "group_name"),
                       group_name) == 0) {
                json_object_object_get_ex(val_tmp, "instances", &val_arr_tmp);
                json_object_array_add(val_arr_tmp,
                                      json_object_new_string(instance_path));

                break;
            }
        }
    }

    jext_file_write("settings.json", settings);
    json_object_put(settings);
}

bc_instance *bc_instance_select_get() {
    bc_instance *instance_selected = NULL;

    json_object *json = json_object_from_file("settings.json");
    json_object *tmp = NULL;

    json_object_object_get_ex(json, "instance", &tmp);

    const char *selected = jext_get_string_dummy(tmp, "selected");

    if (selected[0] != '\0') {
        instance_selected = bc_instance_get(selected);
    }

    json_object_put(json);

    return instance_selected;
}

void bc_instance_select(const char *path) {
    json_object *json = json_object_from_file("settings.json");
    json_object *tmp = NULL;
    json_object *tmp_selected = NULL;

    json_object_object_get_ex(json, "instance", &tmp);
    json_object_object_get_ex(tmp, "selected", &tmp_selected);

    json_object_set_string(tmp_selected, path);

    jext_file_write("settings.json", json);
    json_object_put(json);
}

void bc_instance_remove_group(const char *name) {
    json_object *settings = json_object_from_file("settings.json");
    json_object *tmp = NULL;
    json_object *instance_tmp = NULL;
    json_object *arr_tmp = NULL;

    json_object_object_get_ex(settings, "instance", &instance_tmp);
    json_object_object_get_ex(instance_tmp, "grouped", &tmp);

    for (int i = 0; i < json_object_array_length(tmp); i++) {
        arr_tmp = json_object_array_get_idx(tmp, i);

        if (strcmp(jext_get_string_dummy(arr_tmp, "group_name"), name) == 0) {
            json_object_array_del_idx(tmp, i, 1);
            break;
        }
    }

    jext_file_write("settings.json", settings);
    json_object_put(settings);
}

void bc_instance_create(const char *name, const char *version,
                        const char *group_name) {
    char instance_name[BC_INSTANCE_NAME_MAX_SIZE];
    snprintf(instance_name, sizeof(instance_name), "%s", name);

    char *path = bc_instance_get_path(instance_name);
    int counter = 1;

    while (bc_file_exists(path)) {
        for (int i = 0; i < counter; i++) {
            snprintf(instance_name, sizeof(instance_name), "%s-",
                     instance_name);
        }

        free(path);
        path = bc_instance_get_path(instance_name);
        counter++;
    }

    json_object *config =
        bc_instance_create_default_config(instance_name, version);
    bc_instance_update_settings(path, group_name);

    make_path(path, 1);
    bc_file_create(path, json_object_to_json_string(config));
    free(path);

    json_object_put(config);
}

void bc_instance_update(const bc_instance *instance) {
    json_object *config = bc_instance_create_config(instance);

    jext_file_write(instance->path, config);
    json_object_put(config);
}

void bc_instance_remove(const char *instance_path) {
    json_object *settings = json_object_from_file("settings.json");
    json_object *instance_tmp = NULL;
    json_object *tmp = NULL;
    json_object *val_tmp = NULL;
    json_object *val_arr_tmp = NULL;
    json_object *val_grouped_arr_tmp = NULL;
    json_object *tmp_selected = NULL;

    json_object_object_get_ex(settings, "instance", &instance_tmp);
    json_object_object_get_ex(instance_tmp, "standalone", &tmp);

    for (int i = 0; i < json_object_array_length(tmp); i++) {
        val_tmp = json_object_array_get_idx(tmp, i);

        if (strcmp(json_object_get_string(val_tmp), instance_path) == 0) {
            json_object_array_del_idx(tmp, i, 1);
            break;
        }
    }

    json_object_object_get_ex(instance_tmp, "grouped", &tmp);

    for (int i = 0; i < json_object_array_length(tmp); i++) {
        val_tmp = json_object_array_get_idx(tmp, i);
        json_object_object_get_ex(val_tmp, "instances", &val_arr_tmp);

        for (int j = 0; j < json_object_array_length(val_arr_tmp); j++) {
            val_grouped_arr_tmp = json_object_array_get_idx(val_arr_tmp, i);

            if (strcmp(json_object_get_string(val_grouped_arr_tmp),
                       instance_path) == 0) {
                json_object_array_del_idx(val_arr_tmp, i, 1);
                break;
            }
        }
    }

    json_object_object_get_ex(instance_tmp, "selected", &tmp_selected);

    if (strcmp(json_object_get_string(tmp_selected), instance_path) == 0) {
        json_object_set_string(tmp_selected, "");
    }

    jext_file_write("settings.json", settings);
    json_object_put(settings);
}

bc_instance *bc_instance_get(const char *instance_path) {
    bc_instance *instance = NULL;
    json_object *json = json_object_from_file(instance_path);

    if (json != NULL) {
        instance = (bc_instance*)malloc(sizeof(bc_instance));

        bc_instance_fill_object_from_json(instance, instance_path, json);
        json_object_put(json);
    }

    return instance;
}

bc_instance_array *bc_instance_get_all() {
    bc_instance_array *instance_array = (bc_instance_array*)malloc(sizeof(bc_instance_array));

    json_object *settings = json_object_from_file("settings.json");

    if (settings == NULL) {
        instance_array->len = 0;
        return instance_array;
    }

    json_object *tmp = NULL;
    json_object *instance_tmp = NULL;
    json_object *arr_tmp = NULL;
    json_object *instance_file_tmp = NULL;

    json_object_object_get_ex(settings, "instance", &instance_tmp);
    json_object_object_get_ex(instance_tmp, "standalone", &tmp);

    instance_array->len = json_object_array_length(tmp);

    for (int i = 0; i < instance_array->len; i++) {
        arr_tmp = json_object_array_get_idx(tmp, i);

        const char *instance_path = json_object_get_string(arr_tmp);
        instance_file_tmp = json_object_from_file(instance_path);

        bc_instance_fill_object_from_json(&instance_array->arr[i],
                                          instance_path, instance_file_tmp);
    }

    json_object_put(settings);

    return instance_array;
}

bc_instance_group_array *bc_instance_group_get_all() {
    bc_instance_group_array *instance_array =
        (bc_instance_group_array*)malloc(sizeof(bc_instance_group_array));

    json_object *settings = json_object_from_file("settings.json");

    if (settings == NULL) {
        instance_array->len = 0;
        json_object_put(settings);
        return instance_array;
    }

    json_object *tmp = NULL;
    json_object *instance_tmp = NULL;
    json_object *arr_tmp = NULL;
    json_object *group_instances_tmp = NULL;
    json_object *instance_file_tmp = NULL;

    json_object_object_get_ex(settings, "instance", &instance_tmp);
    json_object_object_get_ex(instance_tmp, "grouped", &tmp);

    instance_array->len = json_object_array_length(tmp);

    for (int i = 0; i < instance_array->len; i++) {
        arr_tmp = json_object_array_get_idx(tmp, i);

        snprintf(instance_array->arr[i].group_name,
                 sizeof(instance_array->arr[i].group_name), "%s",
                 jext_get_string_dummy(arr_tmp, "group_name"));

        json_object_object_get_ex(arr_tmp, "instances", &group_instances_tmp);
        instance_array->arr[i].len =
            json_object_array_length(group_instances_tmp);

        for (int j = 0; j < instance_array->arr[i].len; j++) {
            arr_tmp = json_object_array_get_idx(group_instances_tmp, j);
            const char *instance_path = json_object_get_string(arr_tmp);
            instance_file_tmp = json_object_from_file(instance_path);

            bc_instance_fill_object_from_json(
                &instance_array->arr[i].instances[j], instance_path,
                instance_file_tmp);
        }
    }

    json_object_put(settings);

    return instance_array;
}

bc_progress bc_instance_run_progress() { return bc_game_run_progress; }

void bc_instance_run(const char *server_ip, const char *server_port) {
    bc_instance *selected_instance = bc_instance_select_get();
    bc_mod_version_array *mods = bc_mod_list_installed(selected_instance->path);

    // makes the path be not of the json, but of the instance directory
    unsigned long pathSize =
        strlen(selected_instance->path) - strlen("bc_instance.json");
    selected_instance->path[pathSize] = '\0';

    char *selectedJinst = bc_jinst_select_get();
    snprintf(selected_instance->java_path, sizeof(selected_instance->java_path),
             "%s", selectedJinst);
    free(selectedJinst);

    char jsonLoc[PATH_MAX];
    snprintf(jsonLoc, sizeof(jsonLoc), "versions/%s.json",
             selected_instance->version);

    json_object *jsonObj = json_object_from_file(jsonLoc);
    bc_version *ver = bc_version_read_json(jsonObj);
    json_object_put(jsonObj);

    bc_account *acc = bc_account_select_get();

    if (acc == NULL) {
        acc = (bc_account*)malloc(sizeof(bc_account));

        acc->account_type = BC_ACCOUNT_UNAUTHENTICATED;
        strcpy(acc->username, "Player");
        strcpy(acc->uuid, DEMO_ACCOUNT_UUID);
        strcpy(acc->access_token, "-");
        strcpy(acc->minecraft_access_token, "-");
    }

    bc_game_data *data = (bc_game_data*)malloc(sizeof(bc_game_data));
    data->instance = selected_instance;
    data->version = ver;
    data->account = acc;
    data->mods = mods;
    strcpy(data->server_ip, server_ip);
    strcpy(data->server_port, server_port);

    char *random_uuid = bc_file_uuid();
    snprintf(data->natives_folder, sizeof(data->natives_folder),
             "%snatives-%s/", selected_instance->path, random_uuid);
    free(random_uuid);

    bc_game_run(data);

    free(data->instance);
    free(data->version);
    free(data->account);
    free(data->mods);
    free(data);
}
