#include "Mod.h"
#include "FileSystem.h"
#include "JsonExtension.h"
#include <cpr/cpr.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char API_MODS[] = "https://api.betacraft.uk/v2/mod_list";

bc_mod_array *bc_mod_list(const char *version) {
    bc_mod_array *mod_list = (bc_mod_array*)malloc(sizeof(bc_mod_array));

    char endpoint[100];
    snprintf(endpoint, sizeof(endpoint), "%s?game_version=%s", API_MODS,
             version);

    cpr::Response response = cpr::Get(cpr::Url{endpoint});
    json_object *json = json_tokener_parse(response.text.c_str());
    json_object *tmp, *tmp_versions, *tmp_v, *tmp_requirements, *tmp_r;

    mod_list->len = 0;

    if (json == NULL)
        return mod_list;

    mod_list->len = json_object_array_length(json);

    for (int i = 0; i < mod_list->len; i++) {
        tmp = json_object_array_get_idx(json, i);

        mod_list->arr[i].id = jext_get_int(tmp, "id");
        snprintf(mod_list->arr[i].name, sizeof(mod_list->arr[i].name), "%s",
                 jext_get_string_dummy(tmp, "name"));
        snprintf(mod_list->arr[i].description,
                 sizeof(mod_list->arr[i].description), "%s",
                 jext_get_string_dummy(tmp, "description"));

        json_object_object_get_ex(tmp, "versions", &tmp_versions);

        if (tmp_versions == NULL) {
            mod_list->arr[i].versions.len = 0;
            continue;
        }

        mod_list->arr[i].versions.len = json_object_array_length(tmp_versions);

        for (int j = 0; j < mod_list->arr[i].versions.len; j++) {
            tmp_v = json_object_array_get_idx(tmp_versions, j);

            bc_mod_version *modV = &mod_list->arr[i].versions.arr[j];
            modV->id = jext_get_int(tmp_v, "id");
            modV->modid = jext_get_int(tmp_v, "modid");

            snprintf(modV->name, sizeof(modV->name), "%s",
                     mod_list->arr[i].name);
            snprintf(modV->version, sizeof(modV->version), "%s",
                     jext_get_string_dummy(tmp_v, "version"));
            snprintf(modV->game_version, sizeof(modV->game_version), "%s",
                     jext_get_string_dummy(tmp_v, "gameversion"));
            snprintf(modV->download_url, sizeof(modV->download_url), "%s",
                     jext_get_string_dummy(tmp_v, "downloadurl"));
            snprintf(modV->client_classes, sizeof(modV->client_classes), "%s",
                     jext_get_string_dummy(tmp_v, "client_classes"));
            snprintf(modV->minecraft_dir, sizeof(modV->minecraft_dir), "%s",
                     jext_get_string_dummy(tmp_v, "minecraft_dir"));
            snprintf(modV->resources_dir, sizeof(modV->resources_dir), "%s",
                     jext_get_string_dummy(tmp_v, "resources_dir"));
            snprintf(modV->server_classes, sizeof(modV->server_classes), "%s",
                     jext_get_string_dummy(tmp_v, "server_classes"));

            json_object_object_get_ex(tmp_v, "requirements", &tmp_requirements);

            if (tmp_requirements == NULL) {
                modV->requirements_len = 0;
                modV->requirements = NULL;
                continue;
            }

            modV->requirements_len = json_object_array_length(tmp_requirements);
            modV->requirements =
                (bc_mod_version*)malloc(modV->requirements_len * sizeof(bc_mod_version));

            for (int k = 0; k < modV->requirements_len; k++) {
                tmp_r = json_object_array_get_idx(tmp_requirements, k);

                bc_mod_version *req = &modV->requirements[k];
                req->id = jext_get_int(tmp_r, "id");

                snprintf(req->name, sizeof(req->name), "%s",
                         jext_get_string_dummy(tmp_r, "name"));
                snprintf(req->version, sizeof(req->version), "%s",
                         jext_get_string_dummy(tmp_r, "version"));
                snprintf(req->download_url, sizeof(req->download_url), "%s",
                         jext_get_string_dummy(tmp_r, "downloadurl"));
                snprintf(req->client_classes, sizeof(req->client_classes), "%s",
                         jext_get_string_dummy(tmp_r, "client_classes"));
                snprintf(req->minecraft_dir, sizeof(req->minecraft_dir), "%s",
                         jext_get_string_dummy(tmp_r, "minecraft_dir"));
                snprintf(req->resources_dir, sizeof(req->resources_dir), "%s",
                         jext_get_string_dummy(tmp_r, "resources_dir"));
                snprintf(req->server_classes, sizeof(req->server_classes), "%s",
                         jext_get_string_dummy(tmp_r, "server_classes"));
            }
        }
    }

    json_object_put(json);

    return mod_list;
}

bc_mod_version_array *bc_mod_list_installed(const char *instance_path) {
    bc_mod_version_array *mods = (bc_mod_version_array*)malloc(sizeof(bc_mod_version_array));

    json_object *json = json_object_from_file(instance_path);
    json_object *tmp, *m_tmp;

    if (!json_object_object_get_ex(json, "mods", &tmp)) {
        mods->len = 0;
        json_object_put(json);
        return mods;
    }

    mods->len = json_object_array_length(tmp);

    for (int i = 0; i < mods->len; i++) {
        m_tmp = json_object_array_get_idx(tmp, i);

        bc_mod_version *mod = &mods->arr[i];

        snprintf(mod->name, sizeof(mod->name), "%s",
                 jext_get_string_dummy(m_tmp, "name"));
        snprintf(mod->version, sizeof(mod->version), "%s",
                 jext_get_string_dummy(m_tmp, "version"));
        snprintf(mod->path, sizeof(mod->path), "%s",
                 jext_get_string_dummy(m_tmp, "path"));
        snprintf(mod->client_classes, sizeof(mod->client_classes), "%s",
                 jext_get_string_dummy(m_tmp, "client_classes"));
        snprintf(mod->resources_dir, sizeof(mod->resources_dir), "%s",
                 jext_get_string_dummy(m_tmp, "resources_dir"));
    }

    json_object_put(json);

    return mods;
}

void bc_mod_install_mod(bc_mod_version *version, const char *instance_dir,
                        const char *jar_dir) {
    char path[PATH_MAX], r_path[PATH_MAX];
    if (version->client_classes[0] != '\0') {
        if (strcmp(version->client_classes, ".") == 0) {
            bc_file_directory_copy(version->path, jar_dir);
        } else {
            snprintf(path, sizeof(path), "%s%s", version->path,
                     version->client_classes);
            bc_file_directory_copy(path, jar_dir);
        }
    }

    if (version->resources_dir[0] != '\0') {
        snprintf(path, sizeof(path), "%s%s", version->path,
                 version->resources_dir);
        snprintf(r_path, sizeof(r_path), "%s.minecraft/resources/",
                 instance_dir);

        make_path(r_path, 0);
        bc_file_directory_copy(path, r_path);
    }
}

void bc_mod_install(bc_mod_version_array *installed, const char *instance_dir,
                    const char *game_version) {
    char jar_path[PATH_MAX], meta_inf_path[PATH_MAX], jar_dir_path[PATH_MAX];
    snprintf(jar_dir_path, sizeof(jar_dir_path), "%stemp/", instance_dir);
    snprintf(jar_path, sizeof(jar_path), "versions/%s.jar", game_version);
    snprintf(meta_inf_path, sizeof(meta_inf_path), "%sMETA-INF/", jar_dir_path);

    make_path(jar_dir_path, 0);

    bc_file_extract(jar_path, jar_dir_path);
    bc_file_directory_remove(meta_inf_path);

    for (int i = 0; i < installed->len; i++) {
        bc_mod_install_mod(&installed->arr[i], instance_dir, jar_dir_path);
    }

    snprintf(jar_path, sizeof(jar_path), "%sbc_instance.jar", instance_dir);

    if (bc_file_exists(jar_path))
        remove(jar_path);

    bc_file_archive(jar_dir_path, jar_path);
    bc_file_directory_remove(jar_dir_path);
}

void bc_mod_list_add(bc_mod_version *mod, const char *mod_path,
                     const char *instance_path) {
    json_object *json = json_object_from_file(instance_path);
    json_object *mod_object = json_object_new_object();
    json_object *tmp;

    json_object_object_add(mod_object, "name",
                           json_object_new_string(mod->name));

    json_object_object_add(mod_object, "path",
                           json_object_new_string(mod_path));
    json_object_object_add(mod_object, "version",
                           json_object_new_string(mod->version));
    json_object_object_add(mod_object, "client_classes",
                           json_object_new_string(mod->client_classes));
    json_object_object_add(mod_object, "resources_dir",
                           json_object_new_string(mod->resources_dir));

    json_object_object_get_ex(json, "mods", &tmp);
    json_object_array_add(tmp, mod_object);

    jext_file_write(instance_path, json);

    json_object_put(json);
}

void bc_mod_list_remove(const char *instance_path, const char *mod_path) {
    json_object *json = json_object_from_file(instance_path);
    json_object *tmp, *tmp_m, *tmp_mp;

    json_object_object_get_ex(json, "mods", &tmp);

    for (int i = 0; i < json_object_array_length(tmp); i++) {
        tmp_m = json_object_array_get_idx(tmp, i);

        json_object_object_get_ex(tmp_m, "path", &tmp_mp);

        if (strcmp(json_object_get_string(tmp_mp), mod_path) == 0) {
            json_object_array_del_idx(tmp, i, 1);
            break;
        }
    }

    jext_file_write(instance_path, json);

    json_object_put(json);
}

void bc_mod_add(const char *mod_path, const char *instance_path,
                const char *game_version) {
    char *split = (char*)strrchr(mod_path, '/');
    split++;
    char *filename = strdup(split);
    filename[strlen(filename) - 4] = '\0'; // remove .zip

    char directory[PATH_MAX];
    snprintf(directory, sizeof(directory), "libraries/%s/%s/", game_version,
             filename);

    make_path(directory, 0);
    bc_file_extract(mod_path, directory);

    bc_mod_version mod;
    snprintf(mod.name, sizeof(mod.name), "%s", filename);
    strcpy(mod.version, "");
    strcpy(mod.client_classes, ".");
    strcpy(mod.resources_dir, "");

    bc_mod_list_add(&mod, directory, instance_path);

    free(filename);
}

void bc_mod_download_version(bc_mod_version *mod, const char *instance_path,
                             const char *game_version) {
    char *split = strrchr(mod->download_url, '/');
    split++;
    char *filename = strdup(split);

    char *dropboxParams = filename + (strlen(filename) - 5);
    if (strcmp(dropboxParams, "?dl=1") == 0)
        filename[strlen(filename) - 5] = '\0'; // remove '?dl=1'

    char directory[PATH_MAX], path[PATH_MAX];
    snprintf(directory, sizeof(directory), "libraries/%s/%s/%s/", game_version,
             mod->name, mod->version);

    if (!bc_file_directory_exists(directory)) {
        make_path(directory, 0);
        std::ofstream of(directory, std::ios::binary);
        cpr::Response response = cpr::Download(of, cpr::Url{mod->download_url});

        snprintf(path, sizeof(path), "%s%s", directory, filename);

        bc_file_extract(path, directory);
        remove(path);
    }

    free(filename);

    bc_mod_list_add(mod, directory, instance_path);
}

int bc_mod_list_installed_exists(const char *instance_path, const char *name,
                                 const char *version) {
    bc_mod_version_array *installed = bc_mod_list_installed(instance_path);
    int found = 0;

    for (int i = 0; i < installed->len; i++) {
        if (strcmp(installed->arr[i].name, name) == 0 &&
            strcmp(installed->arr[i].version, version) == 0) {
            found = 1;
            break;
        }
    }

    free(installed);
    return found;
}

void bc_mod_download(bc_mod_version *version, const char *instance_path,
                     const char *game_version) {
    if (!bc_mod_list_installed_exists(instance_path, version->name,
                                      version->version))
        bc_mod_download_version(version, instance_path, game_version);

    if (version->requirements_len > 0) {
        for (int i = 0; i < version->requirements_len; i++) {
            if (!bc_mod_list_installed_exists(instance_path,
                                              version->requirements[i].name,
                                              version->requirements[i].version))
                bc_mod_download_version(&version->requirements[i],
                                        instance_path, game_version);
        }
    }
}

void bc_mod_list_installed_move(bc_mod_version_array *mods,
                                const char *instance_path) {
    json_object *json = json_object_from_file(instance_path);

    json_object_object_del(json, "mods");
    json_object_object_add(json, "mods", json_object_new_array());

    jext_file_write(instance_path, json);

    json_object_put(json);

    for (int i = 0; i < mods->len; i++) {
        bc_mod_list_add(&mods->arr[i], mods->arr[i].path, instance_path);
    }
}

void bc_mod_replace_jar(const char *jar_path, const char *instance_path,
                        const char *game_version) {
    char replace_dir[PATH_MAX];

    snprintf(replace_dir, sizeof(replace_dir), "%s", instance_path);
    replace_dir[strlen(replace_dir) - 16] = '\0'; // remove bc_instance.json
    snprintf(replace_dir, sizeof(replace_dir), "%sbc_replace/", replace_dir);

    if (bc_file_directory_exists(replace_dir)) {
        bc_file_directory_remove(replace_dir);
    }

    bc_file_extract(jar_path, replace_dir);

    char *jar_name = (char*)strrchr(jar_path, '/');
    jar_name++;

    bc_mod_version mod;
    snprintf(mod.name, sizeof(mod.name), "%s", jar_name);
    strcpy(mod.version, "");
    strcpy(mod.client_classes, ".");
    strcpy(mod.resources_dir, "");

    bc_mod_list_add(&mod, replace_dir, instance_path);
}
