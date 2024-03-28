#include "VersionList.h"

#include "Betacraft.h"
#include "FileSystem.h"
#include "JsonExtension.h"
#include "Network.h"

#include <stdio.h>
#include <string.h>

bc_versionlist *bc_versionlist_instance = NULL;

void bc_versionlist_read_version_json(bc_versionlist_version *version,
                                      json_object *obj) {
    snprintf(version->id, sizeof(version->id), "%s",
             jext_get_string_dummy(obj, "id"));
    snprintf(version->type, sizeof(version->type), "%s",
             jext_get_string_dummy(obj, "type"));
    snprintf(version->url, sizeof(version->url), "%s",
             jext_get_string_dummy(obj, "url"));
    snprintf(version->releaseTime, sizeof(version->releaseTime), "%s",
             jext_get_string_dummy(obj, "releaseTime"));
}

bc_versionlist *bc_versionlist_read_json(json_object *obj,
                                         json_object *bcList) {
    bc_versionlist *vl = (bc_versionlist*)malloc(sizeof(bc_versionlist));
    json_object *tmp;

    vl->versions_len = 0;

    json_object_object_get_ex(obj, "latest", &tmp);

    snprintf(vl->latest.release, sizeof(vl->latest.release), "%s",
             jext_get_string_dummy(tmp, "release"));
    snprintf(vl->latest.snapshot, sizeof(vl->latest.snapshot), "%s",
             jext_get_string_dummy(tmp, "snapshot"));

    json_object_object_get_ex(obj, "versions", &tmp);
    array_list *verArr = json_object_get_array(tmp);

    if (verArr == NULL) {
        return vl;
    }

    json_object *verObj;

    const char *trimAt = jext_get_string_dummy(bcList, "trim_at");
    int max = jext_get_string_array_index(tmp, "id", trimAt);

    json_object *bctmp;
    json_object_object_get_ex(bcList, "versions", &bctmp);

    array_list *bcarr = json_object_get_array(bctmp);
    int bc_len = bcarr->size;

    vl->versions_len = max + bc_len;

    // Official version list
    for (int i = 0; i < max; i++) {
        verObj = (json_object*)verArr->array[i];

        bc_versionlist_read_version_json(&vl->versions[i], verObj);
    }

    // Betacraft version list
    for (int i = 0; i < bc_len; i++) {
        verObj = (json_object*)bcarr->array[i];

        bc_versionlist_read_version_json(&vl->versions[i + max], verObj);
    }

    json_object_put(obj);
    json_object_put(bcList);

    return vl;
}

bc_versionlist *bc_versionlist_fetch() {
    char *response = bc_network_get(
        "https://launchermeta.mojang.com/mc/game/version_manifest.json", NULL);
    json_object *obj = json_tokener_parse(response);
    free(response);

    if (obj == NULL)
        return NULL;

    response = bc_network_get(
        "http://files.betacraft.uk/launcher/v2/assets/version_list.json", NULL);
    json_object *bcList = json_tokener_parse(response);
    free(response);

    return bc_versionlist_read_json(obj, bcList);
}

bc_versionlist_version *bc_versionlist_find(const char *id) {
    bc_versionlist *vl = bc_versionlist_fetch();
    bc_versionlist_version *version = NULL;

    for (int i = 0; i < vl->versions_len; i++) {
        if (strcmp(vl->versions[i].id, id) == 0) {
            version = (bc_versionlist_version*)malloc(sizeof(bc_versionlist_version));

            snprintf(version->id, sizeof(version->id), "%s",
                     vl->versions[i].id);
            snprintf(version->url, sizeof(version->url), "%s",
                     vl->versions[i].url);
            snprintf(version->releaseTime, sizeof(version->releaseTime), "%s",
                     vl->versions[i].releaseTime);
            snprintf(version->type, sizeof(version->type), "%s",
                     vl->versions[i].type);

            break;
        }
    }

    free(vl);
    return version;
}

int bc_versionlist_download(const char *gameVersion) {
    if (!betacraft_online) {
        // offline
        return 1;
    }

    char jsonLoc[PATH_MAX];
    snprintf(jsonLoc, sizeof(jsonLoc), "versions/%s.json", gameVersion);

    bc_versionlist_version *version = bc_versionlist_find(gameVersion);

    if (version == NULL) {
        if (!bc_file_exists(jsonLoc)) {
            return 0;
        }

        // custom json
        return 1;
    }

    int result = bc_network_download(version->url, jsonLoc, 1);
    free(version);

    return result;
}
