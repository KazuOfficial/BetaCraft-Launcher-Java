#include "Settings.h"
#include "FileSystem.h"
#include "JsonExtension.h"

#include <stdio.h>
#include <string.h>

#include <json.h>

void bc_settings_update(bc_settings *settings) {
    json_object *json = json_object_from_file("settings.json");
    json_object *tmp;

    json_object_object_get_ex(json, "language", &tmp);
    json_object_set_string(tmp, settings->language);

    json_object_object_get_ex(json, "discord", &tmp);
    json_object_set_boolean(tmp, settings->discord);

    jext_file_write("settings.json", json);

    json_object_put(json);
}

bc_settings *bc_settings_get() {
    bc_settings *settings = (bc_settings*)malloc(sizeof(bc_settings));
    json_object *json = json_object_from_file("settings.json");

    settings->discord = jext_get_boolean(json, "discord");
    snprintf(settings->language, sizeof(settings->language), "%s",
             jext_get_string_dummy(json, "language"));

    json_object *tmp;
    if (json_object_object_get_ex(json, "instance", &tmp)) {
        snprintf(settings->instance, sizeof(settings->instance), "%s",
                 jext_get_string_dummy(tmp, "selected"));
    }

    if (json_object_object_get_ex(json, "java", &tmp)) {
        snprintf(settings->java_install, sizeof(settings->java_install), "%s",
                 jext_get_string_dummy(tmp, "selected"));
    }

    json_object_put(json);

    return settings;
}
