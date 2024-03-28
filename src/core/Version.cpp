#include "Version.h"
#include "JsonExtension.h"
#include "StringUtils.h"

#include <json.h>
#include <stdio.h>
#include <string.h>

void bc_version_read_rule(bc_version_actionRule *rule, json_object *obj) {
    json_object *tmp;

    if (json_object_object_get_ex(obj, "action", &tmp)) {
        snprintf(rule->action, sizeof(rule->action), "%s",
                 json_object_get_string(tmp));
    } else {
        strcpy(rule->action, "allow");
    }

    if (json_object_object_get_ex(obj, "features", &tmp)) {
        rule->features.is_empty = 0;
        rule->features.has_custom_resolution =
            jext_get_boolean(tmp, "has_custom_resolution");
        rule->features.is_demo_user = jext_get_boolean(tmp, "is_demo_user");
        rule->features.has_server = jext_get_boolean(tmp, "has_server");
        rule->features.is_game_fullscreen = 0;
        rule->features.is_game_fullscreen = 0;

        // 1.19.4+
        rule->features.has_quick_plays_support =
            jext_get_boolean(tmp, "has_quick_plays_support");
        rule->features.is_quick_play_singleplayer =
            jext_get_boolean(tmp, "is_quick_play_singleplayer");
        rule->features.is_quick_play_multiplayer =
            jext_get_boolean(tmp, "is_quick_play_multiplayer");
        rule->features.is_quick_play_realms =
            jext_get_boolean(tmp, "is_quick_play_realms");
    } else {
        rule->features.is_empty = 1;
    }

    if (json_object_object_get_ex(obj, "os", &tmp)) {
        rule->os.is_empty = 0;
        snprintf(rule->os.arch, sizeof(rule->os.arch), "%s",
                 jext_get_string_dummy(tmp, "arch"));
        snprintf(rule->os.name, sizeof(rule->os.name), "%s",
                 jext_get_string_dummy(tmp, "name"));
        snprintf(rule->os.version, sizeof(rule->os.version), "%s",
                 jext_get_string_dummy(tmp, "version"));
    } else {
        rule->os.is_empty = 1;
    }
}

void bc_version_read_rule_all(bc_version_actionRule *rules, json_object *obj) {
    array_list *rulesJson = json_object_get_array(obj);

    for (int j = 0; j < rulesJson->size; j++) {
        bc_version_read_rule(&rules[j], (json_object*)rulesJson->array[j]);
    }
}

void bc_version_read_arg_rule(bc_version_argRule *rule, json_object *arg_rule) {
    if (json_object_is_type(arg_rule, json_type_string)) {
        snprintf(rule->value[0], sizeof(rule->value[0]), "%s",
                 json_object_get_string(arg_rule));
        rule->value_len = 0;
        rule->rules_len = 0;
    } else {
        json_object *valueJson, *rulesJson;

        if (json_object_object_get_ex(arg_rule, "value", &valueJson)) {
            if (json_object_is_type(valueJson, json_type_string)) {
                snprintf(rule->value[0], sizeof(rule->value[0]), "%s",
                         json_object_get_string(valueJson));
                rule->value_len = 0;
            } else if (json_object_is_type(valueJson, json_type_array)) {
                rule->value_len = json_object_array_length(valueJson);

                int len = json_object_array_length(valueJson);

                json_object *tmp;
                for (int i = 0; i < len; i++) {
                    tmp = json_object_array_get_idx(valueJson, i);

                    snprintf(rule->value[i], sizeof(rule->value[i]), "%s",
                             json_object_get_string(tmp));
                }
            }
        }

        if (json_object_object_get_ex(arg_rule, "rules", &rulesJson)) {
            rule->rules_len = json_object_array_length(rulesJson);
            bc_version_read_rule_all(rule->rules, rulesJson);
        }
    }
}

void bc_version_read_json_asset_index(json_object *obj, json_object *tmp,
                                      bc_version *v) {
    if (json_object_object_get_ex(obj, "assetIndex", &tmp)) {
        snprintf(v->assetIndex.id, sizeof(v->assetIndex.id), "%s",
                 jext_get_string_dummy(tmp, "id"));
        snprintf(v->assetIndex.sha1, sizeof(v->assetIndex.sha1), "%s",
                 jext_get_string_dummy(tmp, "sha1"));
        snprintf(v->assetIndex.url, sizeof(v->assetIndex.url), "%s",
                 jext_get_string_dummy(tmp, "url"));
        v->assetIndex.size = jext_get_int(tmp, "size");
        v->assetIndex.totalSize = jext_get_int(tmp, "totalSize");
    }
}

void bc_version_read_json_downloads(json_object *obj, json_object *tmp,
                                    const char *value,
                                    bc_version_downloadable *dw) {
    if (json_object_object_get_ex(obj, value, &tmp)) {
        snprintf(dw->sha1, sizeof(dw->sha1), "%s",
                 jext_get_string_dummy(tmp, "sha1"));
        snprintf(dw->url, sizeof(dw->url), "%s",
                 jext_get_string_dummy(tmp, "url"));
        dw->size = jext_get_int(tmp, "size");
    }
}

void bc_version_read_json_java_version(json_object *obj, json_object *tmp,
                                       bc_version *v) {
    if (json_object_object_get_ex(obj, "javaVersion", &tmp)) {
        snprintf(v->javaVersion.component, sizeof(v->javaVersion.component),
                 "%s", jext_get_string_dummy(tmp, "component"));
        v->javaVersion.majorVersion = jext_get_int(tmp, "majorVersion");
        // ignore java 16, recommend 17 instead
        if (v->javaVersion.majorVersion == 16) {
            v->javaVersion.majorVersion = 17;
        }

        // Betacraft exclusive
        v->javaVersion.advisedMaxVersion =
            jext_get_int(tmp, "advisedMaxVersion");
        v->javaVersion.minVersion = jext_get_int(tmp, "minVersion");
    }
}

void bc_version_read_json_betacraft(json_object *obj, json_object *tmp,
                                    bc_version *v) {
    // Betacraft exclusive
    if (json_object_object_get_ex(obj, "modern_server_parameters", &tmp)) {
        v->modern_server_parameters =
            jext_get_boolean(obj, "modern_server_parameters");
    } else {
        v->modern_server_parameters = 1;
    }

    if (json_object_object_get_ex(obj, "uuid", &tmp)) {
        snprintf(v->uuid, sizeof(v->uuid), "%s", json_object_get_string(tmp));
    } else {
        snprintf(v->uuid, sizeof(v->uuid), "%s", v->id);
    }
}

void bc_game_version_json_read_logging(json_object *obj, json_object *tmp,
                                       bc_version *v) {
    if (json_object_object_get_ex(obj, "logging", &tmp)) {
        if (json_object_object_get_ex(tmp, "client", &tmp)) {
            snprintf(v->logging.client.type, sizeof(v->logging.client.type),
                     "%s", jext_get_string_dummy(tmp, "type"));
            snprintf(v->logging.client.argument,
                     sizeof(v->logging.client.argument), "%s",
                     jext_get_string_dummy(tmp, "argument"));

            if (json_object_object_get_ex(tmp, "file", &tmp)) {
                snprintf(v->logging.client.file.id,
                         sizeof(v->logging.client.file.id), "%s",
                         jext_get_string_dummy(tmp, "id"));
                snprintf(v->logging.client.file.sha1,
                         sizeof(v->logging.client.file.sha1), "%s",
                         jext_get_string_dummy(tmp, "sha1"));
                snprintf(v->logging.client.file.url,
                         sizeof(v->logging.client.file.url), "%s",
                         jext_get_string_dummy(tmp, "url"));
                v->logging.client.file.size = jext_get_int(tmp, "size");
            }
        }
    }
}

void bc_game_version_json_read_minecraft_arguments(json_object *tmp,
                                                   bc_version *v) {
    const char *mcargs = json_object_get_string(tmp);
    char *mcargs_heap = (char*)malloc(strlen(mcargs) + 1);
    int size = count_substring(mcargs_heap, ' ') + 1 + /* width, height */ 4;
    char split[128][1024];

    char *token = strtok(mcargs_heap, " ");
    int counter = 0;

    while (token != NULL) {
        snprintf(split[counter], 256, "%s", token);
        token = strtok(NULL, " ");
        counter++;
    }

    free(mcargs_heap);

    v->arguments.game_len = 1;
    v->arguments.game->rules_len = 0;
    v->arguments.game->value_len = size;

    // minecraftArguments *never* contains width and height.
    strcpy(v->arguments.game->value[0], "--width");
    strcpy(v->arguments.game->value[1], "${resolution_width}");
    strcpy(v->arguments.game->value[2], "--height");
    strcpy(v->arguments.game->value[3], "${resolution_height}");

    for (int i = 4; i < size; i++) {
        snprintf(v->arguments.game->value[i],
                 sizeof(v->arguments.game->value[i]), "%s", split[i - 4]);
    }

    v->arguments.jvm_len = 1;
    v->arguments.jvm->rules_len = 0;

    strcpy(v->arguments.jvm->value[0],
           "-Djava.library.path=${natives_directory}");
    strcpy(v->arguments.jvm->value[1],
           "-Dminecraft.launcher.brand=${launcher_name}");
    strcpy(v->arguments.jvm->value[2],
           "-Dminecraft.launcher.version=${launcher_version}");
    strcpy(v->arguments.jvm->value[3], "-cp");
    strcpy(v->arguments.jvm->value[4], "${classpath}");

    v->arguments.jvm->value_len = 5;
}

void bc_game_version_read_arguments(json_object *tmp, bc_version *v) {
    json_object *innerObj;

    if (json_object_object_get_ex(tmp, "game", &innerObj)) {
        array_list *gameArr = json_object_get_array(innerObj);

        v->arguments.game_len = gameArr->size;

        for (int i = 0; i < gameArr->size; i++) {
            json_object *gameobj = (json_object*)gameArr->array[i];

            bc_version_read_arg_rule(&v->arguments.game[i], gameobj);
        }
    }

    if (json_object_object_get_ex(tmp, "jvm", &innerObj)) {
        array_list *jvmArr = json_object_get_array(innerObj);

        v->arguments.jvm_len = jvmArr->size;

        for (int i = 0; i < jvmArr->size; i++) {
            json_object *jvmobj = (json_object*)jvmArr->array[i];

            bc_version_read_arg_rule(&v->arguments.jvm[i], jvmobj);
        }
    }
}

void bc_game_version_read_lib_rules(json_object *obj, json_object *tmp,
                                    bc_version *v, int i) {
    if (json_object_object_get_ex(obj, "rules", &tmp)) {
        bc_version_read_rule_all(v->libraries[i].rules, tmp);

        array_list *rulesJson = json_object_get_array(tmp);
        v->libraries[i].rules_len = rulesJson->size;
    } else {
        v->libraries[i].rules_len = 0;
    }
}

void bc_game_version_read_lib_natives(json_object *obj, json_object *tmp,
                                      bc_version *v, int i) {
    if (json_object_object_get_ex(obj, "natives", &tmp)) {
        struct json_object_iterator itBegin = json_object_iter_begin(tmp);
        struct json_object_iterator itEnd = json_object_iter_end(tmp);

        int size = json_object_object_length(tmp);
        v->libraries[i].natives_len = size;

        int pos = 0;
        while (!json_object_iter_equal(&itBegin, &itEnd)) {
            bc_version_nativeMap *nativeMap = &v->libraries[i].natives[pos];

            snprintf(nativeMap->os, sizeof(nativeMap->os), "%s",
                     json_object_iter_peek_name(&itBegin));

            json_object *val = json_object_iter_peek_value(&itBegin);

            snprintf(nativeMap->classifierId, sizeof(nativeMap->classifierId),
                     "%s", json_object_get_string(val));

            pos++;
            json_object_iter_next(&itBegin);
        }
    } else {
        v->libraries[i].natives_len = 0;
    }
}

void bc_game_version_read_lib_dw_artifact(json_object *obj, bc_version *v,
                                          int i) {
    json_object *artifactJson;

    if (json_object_object_get_ex(obj, "artifact", &artifactJson)) {
        bc_version_downloadable *downloadable =
            &v->libraries[i].downloads.artifact;

        snprintf(downloadable->sha1, sizeof(downloadable->sha1), "%s",
                 jext_get_string_dummy(artifactJson, "sha1"));
        snprintf(downloadable->url, sizeof(downloadable->url), "%s",
                 jext_get_string_dummy(artifactJson, "url"));
        downloadable->size = jext_get_int(artifactJson, "size");
    } else {
        v->libraries[i].downloads.artifact.size = 0;
    }
}

void bc_game_version_read_lib_dw_classifiers(json_object *obj, bc_version *v,
                                             int i) {
    json_object *classifiersJson;
    if (json_object_object_get_ex(obj, "classifiers", &classifiersJson)) {
        struct json_object_iterator itBegin =
            json_object_iter_begin(classifiersJson);
        struct json_object_iterator itEnd =
            json_object_iter_end(classifiersJson);

        int size = json_object_object_length(classifiersJson);
        v->libraries[i].downloads.classifiers_len = size;

        int pos = 0;
        while (!json_object_iter_equal(&itBegin, &itEnd)) {
            const char *key = json_object_iter_peek_name(&itBegin);
            bc_version_classifiersMap *classifiersMap =
                &v->libraries[i].downloads.classifiers[pos];

            snprintf(classifiersMap->id, sizeof(classifiersMap->id), "%s", key);

            json_object *dlobj = json_object_iter_peek_value(&itBegin);

            snprintf(classifiersMap->object.sha1,
                     sizeof(classifiersMap->object.sha1), "%s",
                     jext_get_string_dummy(dlobj, "sha1"));
            snprintf(classifiersMap->object.url,
                     sizeof(classifiersMap->object.url), "%s",
                     jext_get_string_dummy(dlobj, "url"));
            classifiersMap->object.size = jext_get_int(dlobj, "size");

            pos++;

            json_object_iter_next(&itBegin);
        }
    } else {
        v->libraries[i].downloads.classifiers_len = 0;
    }
}

void bc_game_version_read_lib_list(json_object *obj, json_object *tmp,
                                   bc_version *v) {
    if (json_object_object_get_ex(obj, "libraries", &tmp)) {
        array_list *libArr = json_object_get_array(tmp);

        v->lib_len = libArr->size;

        for (int i = 0; i < libArr->size; i++) {
            json_object *libraryobj = (json_object*)libArr->array[i];

            snprintf(v->libraries[i].name, sizeof(v->libraries[i].name), "%s",
                     jext_get_string_dummy(libraryobj, "name"));
            snprintf(v->libraries[i].url, sizeof(v->libraries[i].url), "%s",
                     jext_get_string_dummy(libraryobj, "url"));

            json_object *temp;
            bc_game_version_read_lib_rules(libraryobj, temp, v, i);
            bc_game_version_read_lib_natives(libraryobj, temp, v, i);

            if (json_object_object_get_ex(libraryobj, "downloads", &temp)) {
                bc_game_version_read_lib_dw_artifact(temp, v, i);
                bc_game_version_read_lib_dw_classifiers(temp, v, i);
            }
        }
    }
}

bc_version *bc_version_read_json(json_object *obj) {
    bc_version *v = (bc_version*)malloc(sizeof(bc_version));
    json_object *tmp, *downloads;
    json_object_object_get_ex(obj, "downloads", &downloads);

    v->minimumLauncherVersion = jext_get_int(obj, "minimumLauncherVersion");
    v->usesMinecraftArguments =
        json_object_object_get_ex(obj, "minecraftArguments", NULL);

    snprintf(v->id, sizeof(v->id), "%s", jext_get_string_dummy(obj, "id"));
    snprintf(v->assets, sizeof(v->assets), "%s",
             jext_get_string_dummy(obj, "assets"));
    snprintf(v->type, sizeof(v->type), "%s",
             jext_get_string_dummy(obj, "type"));
    snprintf(v->mainClass, sizeof(v->mainClass), "%s",
             jext_get_string_dummy(obj, "mainClass"));
    snprintf(v->inheritsFrom, sizeof(v->inheritsFrom), "%s",
             jext_get_string_dummy(obj, "inheritsFrom"));

    snprintf(v->releaseTime, sizeof(v->releaseTime), "%s",
             jext_get_string_dummy(obj, "releaseTime"));
    snprintf(v->time, sizeof(v->time), "%s",
             jext_get_string_dummy(obj, "time"));

    bc_version_read_json_betacraft(obj, tmp, v);
    bc_version_read_json_asset_index(obj, tmp, v);
    bc_version_read_json_downloads(downloads, tmp, "client",
                                   &v->downloads.client);
    bc_version_read_json_downloads(downloads, tmp, "server",
                                   &v->downloads.server);
    bc_version_read_json_downloads(downloads, tmp, "windows_server",
                                   &v->downloads.windows_server);
    bc_version_read_json_downloads(downloads, tmp, "client_mappings",
                                   &v->downloads.client_mappings);
    bc_version_read_json_downloads(downloads, tmp, "server_mappings",
                                   &v->downloads.server_mappings);

    bc_version_read_json_java_version(obj, tmp, v);
    bc_game_version_read_lib_list(obj, tmp, v);

    if (json_object_object_get_ex(obj, "arguments", &tmp)) {
        bc_game_version_read_arguments(tmp, v);
    } else if (json_object_object_get_ex(obj, "minecraftArguments", &tmp)) {
        bc_game_version_json_read_minecraft_arguments(tmp, v);
    }

    bc_game_version_json_read_logging(obj, tmp, v);

    return v;
}
