#include "JavaInstallations.h"
#include "FileSystem.h"
#include "JsonExtension.h"
#include "Logger.h"
#include "Network.h"
#include "Version.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

char *bc_java_version(const char *path) {
    FILE *fp;
    char buff[512];
    char *version;

    char command[512];
    snprintf(command, sizeof(command), "\"%s\" -version 2>&1",
             path); // 2>&1 = stderr to stdout

    fp = popen(command, "r");

    fgets(buff, sizeof(buff), fp);

    bc_log("%s\n", buff);

    version = strtok(buff, "\"");
    version = strtok(NULL, "\"");

    if (version == NULL)
        return NULL;

    pclose(fp);

    char *out = (char*)malloc(strlen(version) + 1);
    strcpy(out, version);

    return out;
}

void bc_java_download(const char *url) {
    char *filename = (char*)strrchr(url, '/');
    char *dir_working = bc_file_directory_get_working();

    char path_download[PATH_MAX];
    char path_dir[PATH_MAX];

    snprintf(path_download, sizeof(path_download), "java%s", filename);
    snprintf(path_dir, sizeof(path_dir), "%s", path_download);

    char *fileExtension = strrchr(path_download, '.');

    if (fileExtension == NULL)
        return;

    if (strcmp(fileExtension, ".zip") == 0) {
        path_dir[strlen(path_download) - 3] = '\0';
    } else if (strcmp(fileExtension, ".gz") == 0) {
        path_dir[strlen(path_download) - 6] = '\0';
    } else {
        return;
    }

    path_dir[strlen(path_dir) - 1] = '/';

    make_path(path_dir, 0);
    int downloadRes = bc_network_download(url, "java/", 0);

    if (!downloadRes)
        return;

    bc_log("%s %s\n", "Downloaded Java to", path_dir);
    bc_file_extract(path_download, path_dir);
    remove(path_download);

    bc_file_list_array *files = bc_file_list(path_dir);

    if (files->len == 1 && files->arr[0].is_directory == 1) {
        char inside_dir[PATH_MAX];
        snprintf(inside_dir, sizeof(inside_dir), "%s%s/", path_dir,
                 files->arr[0].name);

        bc_file_directory_copy(inside_dir, path_dir);
        bc_file_directory_remove(inside_dir);
    }

    free(files);

    char path_bin[PATH_MAX];
    snprintf(path_bin, sizeof(path_bin), "%s%sbin/java", dir_working, path_dir);

#ifdef _WIN32
    snprintf(path_bin, sizeof(path_bin), "%sw.exe", path_bin);

    for (int i = 0; i <= strlen(path_bin); i++) {
        if (path_bin[i] == '/')
            path_bin[i] = '\\';
    }
#elif defined(__linux__) || defined(__APPLE__)
    chmod(path_bin, 0777);
#endif

    bc_jinst_add(path_bin);

    free(dir_working);
}

int bc_jrepo_check_os(const char *name, const char *arch) {
#ifdef _WIN64
    if (strcmp(name, "windows") == 0 && strcmp(arch, "x64") == 0) {
        return 0;
    }
#elif __linux__
    if (strcmp(name, "linux") == 0 && strcmp(arch, "x64") == 0) {
        return 0;
    }
#elif __APPLE__
#ifdef __aarch64__
    if (strcmp(name, "macos") == 0 && strcmp(arch, "aarch64") == 0) {
        return 0;
    }
#elif TARGET_OS_MAC
    if (strcmp(name, "macos") == 0 && strcmp(arch, "x64") == 0) {
        return 0;
    }
#endif
#endif

    return 1;
}

bc_jrepo_array *bc_jrepo_get_all() {
    bc_jrepo_array *jrepo_array = (bc_jrepo_array*)malloc(sizeof(bc_jrepo_array));
    json_object *json = json_object_from_file("java_repo.json");

    if (json == NULL) {
        jrepo_array->len = 0;
        return jrepo_array;
    }

    json_object *tmp, *tmp_platform;

    jrepo_array->len = json_object_array_length(json);

    for (int i = 0; i < jrepo_array->len; i++) {
        tmp = json_object_array_get_idx(json, i);
        bc_jrepo *jrepo = &jrepo_array->arr[i];

        jrepo->version = jext_get_int(tmp, "version");
        snprintf(jrepo->full_version, sizeof(jrepo->full_version), "%s",
                 jext_get_string_dummy(tmp, "full_version"));

        json_object_object_get_ex(tmp, "platforms", &tmp_platform);

        jrepo_array->arr[i].platforms_len =
            json_object_array_length(tmp_platform);

        for (int p = 0; p < jrepo_array->arr[i].platforms_len; p++) {
            tmp = json_object_array_get_idx(tmp_platform, p);

            snprintf(jrepo->platforms[p].name, sizeof(jrepo->platforms[p].name),
                     "%s", jext_get_string_dummy(tmp, "name"));
            snprintf(jrepo->platforms[p].arch, sizeof(jrepo->platforms[p].arch),
                     "%s", jext_get_string_dummy(tmp, "arch"));
            snprintf(jrepo->platforms[p].hash, sizeof(jrepo->platforms[p].hash),
                     "%s", jext_get_string_dummy(tmp, "hash"));
            snprintf(jrepo->platforms[p].url, sizeof(jrepo->platforms[p].url),
                     "%s", jext_get_string_dummy(tmp, "url"));
        };
    }

    json_object_put(json);
    return jrepo_array;
}

bc_jrepo_download_array *bc_jrepo_get_all_system() {
    bc_jrepo_download_array *jrepo_download_array =
        (bc_jrepo_download_array*)malloc(sizeof(bc_jrepo_download_array));
    bc_jrepo_array *jrepo_array = bc_jrepo_get_all();

    jrepo_download_array->len = 0;

    for (int i = 0; i < jrepo_array->len; i++) {
        for (int p = 0; p < jrepo_array->arr[i].platforms_len; p++) {
            if (!bc_jrepo_check_os(jrepo_array->arr[i].platforms[p].name,
                                   jrepo_array->arr[i].platforms[p].arch)) {
                bc_jrepo_download *download =
                    &jrepo_download_array->arr[jrepo_download_array->len];

                download->version = jrepo_array->arr[i].version;
                snprintf(download->full_version, sizeof(download->full_version),
                         "%s", jrepo_array->arr[i].full_version);
                snprintf(download->url, sizeof(download->url), "%s",
                         jrepo_array->arr[i].platforms[p].url);

                jrepo_download_array->len++;

                break;
            }
        }
    }

    free(jrepo_array);
    return jrepo_download_array;
}

void bc_jinst_system_check() {
    char *jinstSelected = bc_jinst_select_get();

    if (jinstSelected == NULL) {
#if defined(__linux__) || defined(__APPLE__)
        if (bc_file_exists("/usr/bin/java")) {
            bc_jinst_add("/usr/bin/java");
            bc_jinst_select("/usr/bin/java");
            return;
        }
#elif _WIN32
        char *path = getenv("PATH");
        char *token = strtok(path, ";");

        while (token != NULL) {
            if (strstr(token, "java") != NULL) {
                char java_path[PATH_MAX];
                snprintf(java_path, sizeof(java_path), "%s\\javaw.exe", token);

                bc_jinst_add(java_path);
                bc_jinst_select(java_path);

                return;
            }

            token = strtok(NULL, ";");
        }
#endif
    } else {
        free(jinstSelected);
    }
}

void bc_jinst_add(const char *path) {
    char *version = bc_java_version(path);

    if (version == NULL) {
        bc_log("%s\n", "Error: bc_jinst_add failed - can't read Java version");
        return;
    }

    json_object *settings = json_object_from_file("settings.json");
    json_object *java_installation = json_object_new_object();
    json_object *tmp, *java_object;

    json_object_object_get_ex(settings, "java", &java_object);
    json_object_object_get_ex(java_object, "installations", &tmp);

    int jinst_index = jext_get_string_array_index(tmp, "version", version);

    if (jinst_index != -1) { // if installation already exists
        free(version);
        json_object_put(settings);
        json_object_put(java_installation);
        return;
    }

    json_object_object_add(java_installation, "version",
                           json_object_new_string(version));
    json_object_object_add(java_installation, "path",
                           json_object_new_string(path));

    json_object_array_add(tmp, java_installation);

    jext_file_write("settings.json", settings);

    free(version);
    json_object_put(settings);
}

void bc_jinst_select(const char *path) {
    json_object *json = json_object_from_file("settings.json");
    json_object *tmp, *java_object;

    json_object_object_get_ex(json, "java", &java_object);
    json_object_object_get_ex(java_object, "selected", &tmp);
    json_object_set_string(tmp, path);

    jext_file_write("settings.json", json);

    json_object_put(json);
}

char *bc_jinst_select_get() {
    json_object *json = json_object_from_file("settings.json");
    json_object *tmp;

    json_object_object_get_ex(json, "java", &tmp);
    char *selected = jext_get_string(tmp, "selected");
    json_object_put(json);

    return selected;
}

void bc_jinst_remove(const char *path) {
    json_object *json = json_object_from_file("settings.json");
    json_object *arr, *java_object, *tmp;

    json_object_object_get_ex(json, "java", &java_object);
    json_object_object_get_ex(java_object, "installations", &arr);

    json_object_array_del_idx(
        arr, jext_get_string_array_index(arr, "path", path), 1);

    char *selected = bc_jinst_select_get();

    if (selected != NULL && strcmp(selected, path) == 0) {
        json_object_object_get_ex(java_object, "selected", &tmp);
        json_object_set_string(tmp, "");
    }

    jext_file_write("settings.json", json);

    free(selected);
    json_object_put(json);
}

bc_jinst_array *bc_jinst_get_all() {
    bc_jinst_array *jinst_array = NULL;

    json_object *json = json_object_from_file("settings.json");

    if (json == NULL) {
        jinst_array->len = 0;
        json_object_put(json);
        return jinst_array;
    }

    json_object *arr, *java_object, *tmp;

    json_object_object_get_ex(json, "java", &java_object);
    json_object_object_get_ex(java_object, "installations", &arr);

    jinst_array = (bc_jinst_array*)malloc(sizeof(bc_jinst_array));
    jinst_array->len = json_object_array_length(arr);

    for (int i = 0; i < jinst_array->len; i++) {
        tmp = json_object_array_get_idx(arr, i);

        snprintf(jinst_array->arr[i].version,
                 sizeof(jinst_array->arr[i].version), "%s",
                 jext_get_string_dummy(tmp, "version"));
        snprintf(jinst_array->arr[i].path, sizeof(jinst_array->arr[i].path),
                 "%s", jext_get_string_dummy(tmp, "path"));
    }

    json_object_put(json);

    return jinst_array;
}

bc_jinst *bc_jinst_get(const char *path) {
    bc_jinst *jinst = NULL;

    json_object *json = json_object_from_file("settings.json");
    json_object *arr, *java_object, *tmp;

    json_object_object_get_ex(json, "java", &java_object);
    json_object_object_get_ex(java_object, "installations", &arr);

    int len = json_object_array_length(arr);

    for (int i = 0; i < len; i++) {
        tmp = json_object_array_get_idx(arr, i);
        const char *tmpPath = jext_get_string_dummy(tmp, "path");

        if (strcmp(tmpPath, path) == 0) {
            jinst = (bc_jinst*)malloc(sizeof(bc_jinst));

            snprintf(jinst->path, sizeof(jinst->path), "%s",
                     jext_get_string_dummy(tmp, "path"));
            snprintf(jinst->version, sizeof(jinst->version), "%s",
                     jext_get_string_dummy(tmp, "version"));

            break;
        }
    }

    json_object_put(json);

    return jinst;
}

char *bc_jrepo_parse_version(const char *version) {
    char ver[8];
    int len = 0;

    if (strlen(version) < 2) {
        bc_log("Can't read Java version: %s\n", version);
        exit(1);
    }

    if (version[0] == '1' && version[1] == '.') {
        ver[0] = version[2];
        len++;
    } else {
        for (int i = 0; i < strlen(version); i++) {
            if (version[i] != '.') {
                ver[i] = version[i];
                len++;
            } else
                break;
        }
    }

    assert(ver[0] != '\0');

    ver[len] = '\0';

    char *out = (char*)malloc(strlen(ver) + 1);
    strcpy(out, ver);

    return out;
}

char *bc_jrepo_get_recommended(const char *gameVersion) {
    char *res = NULL;
    char jsonLoc[PATH_MAX];
    snprintf(jsonLoc, sizeof(jsonLoc), "versions/%s.json", gameVersion);

    json_object *json = json_object_from_file(jsonLoc);
    assert(json != NULL);

    bc_version *version = bc_version_read_json(json);

    bc_jrepo_download_array *jrepo = bc_jrepo_get_all_system();
    assert(jrepo->len > 0);

    for (int i = jrepo->len - 1; i >= 0; i--) {
        int maxVersion = jrepo->arr[i].version;

        if ((version->javaVersion.advisedMaxVersion > 0 &&
             version->javaVersion.advisedMaxVersion == maxVersion) ||
            (version->javaVersion.majorVersion > 0 &&
             version->javaVersion.majorVersion == maxVersion)) {
            res = (char*)malloc(strlen(jrepo->arr[i].full_version) + 1);
            strcpy(res, jrepo->arr[i].full_version);
            break;
        }
    }

    json_object_put(json);
    free(version);
    free(jrepo);

    assert(res != NULL);

    return res;
}
