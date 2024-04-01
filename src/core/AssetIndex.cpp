#include "AssetIndex.h"

#include "Betacraft.h"
#include "FileSystem.h"
#include "JsonExtension.h"
#include "cpr/api.h"
#include "cpr/cprtypes.h"
#include "cpr/response.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cpr/cpr.h>

#ifdef __linux__
#include <pwd.h>
#include <unistd.h>
#endif

bc_assetindex *bc_assetindex_read_objects(const char *responseData) {
    json_object *responseJson = json_tokener_parse(responseData);
    bc_assetindex *index = (bc_assetindex*)malloc(sizeof(bc_assetindex));

    json_object *assetArray = NULL;
    if (json_object_object_get_ex(responseJson, "objects", &assetArray)) {
        struct json_object_iterator itBegin =
            json_object_iter_begin(assetArray);
        struct json_object_iterator itEnd = json_object_iter_end(assetArray);

        index->len = json_object_object_length(assetArray);
        index->objects = (bc_assetindex_asset*)malloc(index->len * sizeof(bc_assetindex_asset));

        int pos = 0;
        while (!json_object_iter_equal(&itBegin, &itEnd)) {
            snprintf(index->objects[pos].objectid,
                     sizeof(index->objects[pos].objectid), "%s",
                     json_object_iter_peek_name(&itBegin));

            json_object *assetObject = json_object_iter_peek_value(&itBegin);

            snprintf(index->objects[pos].hash, sizeof(index->objects[pos].hash),
                     "%s", jext_get_string_dummy(assetObject, "hash"));
            index->objects[pos].size = jext_get_int(assetObject, "size");
            // betacraft exclusive
            snprintf(index->objects[pos].baseUrl,
                     sizeof(index->objects[pos].baseUrl), "%s",
                     jext_get_string_dummy(assetObject, "custom_url"));

            pos++;
            json_object_iter_next(&itBegin);
        }
    }

    // index->virtual = jext_get_boolean(responseJson, "virtual");
    index->mapToResources = jext_get_boolean(responseJson, "mapToResources");

    return index;
}

bc_assetindex *bc_assetindex_load(bc_version_assetIndexData *data) {
    char *assetsData = NULL;
    char *location = bc_file_minecraft_directory();
    char indexesLoc[PATH_MAX];

    json_object *json = NULL;

    snprintf(indexesLoc, sizeof(indexesLoc), "%sassets/indexes/", location);
    free(location);

    make_path(indexesLoc, 0);

    char jsonLoc[PATH_MAX];
    snprintf(jsonLoc, sizeof(jsonLoc), "%s%s.json", indexesLoc, data->id);

    if (betacraft_online &&
        (!bc_file_exists(jsonLoc) || bc_file_size(jsonLoc) != data->size)) {

        cpr::Response response = cpr::Get(cpr::Url{data->url});
        assetsData = (char*)malloc(sizeof(char) * strlen(response.text.c_str()));
        strcpy(assetsData, response.text.c_str());
        bc_file_create(jsonLoc, assetsData);
    } else {
        json = json_object_from_file(jsonLoc);
        const char *json_str = json_object_to_json_string(json);

        assetsData = (char*)malloc(sizeof(char) * strlen(json_str));
        strcpy(assetsData, json_str);
    }

    bc_assetindex *assetIndex = bc_assetindex_read_objects(assetsData);

    if (json != NULL) {
        json_object_put(json);
    }

    free(assetsData);

    return assetIndex;
}
