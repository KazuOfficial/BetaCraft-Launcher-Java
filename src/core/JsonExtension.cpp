#include "JsonExtension.h"

#include <stdio.h>
#include <string.h>

void jext_file_write(const char *path, json_object *obj) {
    FILE *fp = fopen(path, "w");
    fprintf(fp, "%s", json_object_to_json_string(obj));
    fclose(fp);
}

char *jext_alloc_string(json_object *obj) {
    int len = json_object_get_string_len(obj);

    if (len > 0) {
        char *result = (char*)malloc(len + 1);
        strcpy(result, json_object_get_string(obj));

        return result;
    }

    return NULL;
}

char **jext_alloc_string_array(json_object *obj) {
    int len = json_object_array_length(obj);
    char **strings = (char**)malloc(len * sizeof(char *));

    json_object *tmp;
    for (int i = 0; i < len; i++) {
        tmp = json_object_array_get_idx(obj, i);

        int tmpLen = json_object_get_string_len(tmp);
        strings[i] = (char*)malloc(tmpLen + 1);
        strcpy(strings[i], json_object_get_string(tmp));
    }

    return strings;
}

char *jext_get_string(json_object *json, const char *section) {
    json_object *tmp;
    json_object_object_get_ex(json, section, &tmp);
    return jext_alloc_string(tmp);
}

const char *jext_get_string_dummy(json_object *json, const char *section) {
    json_object *tmp;
    json_object_object_get_ex(json, section, &tmp);

    const char *ret = json_object_get_string(tmp);
    return ret == NULL ? "" : ret;
}

int jext_get_int(json_object *json, const char *section) {
    json_object *tmp;
    json_object_object_get_ex(json, section, &tmp);
    return json_object_get_int(tmp);
}

int jext_get_double(json_object *json, const char *section) {
    json_object *tmp;
    json_object_object_get_ex(json, section, &tmp);
    return json_object_get_double(tmp);
}

int jext_get_boolean(json_object *json, const char *section) {
    json_object *tmp;
    if (!json_object_object_get_ex(json, section, &tmp)) {
        // undefined if non existent
        return -1;
    }
    return json_object_get_boolean(tmp);
}

char **jext_get_string_array(json_object *json, const char *section) {
    json_object *tmp;

    if (!json_object_object_get_ex(json, section, &tmp)) {
        return NULL;
    }

    return jext_alloc_string_array(tmp);
}

int jext_get_string_array_index(json_object *arr, const char *section,
                                const char *value) {
    json_object *tmp;

    for (int i = 0; i < json_object_array_length(arr); i++) {
        tmp = json_object_array_get_idx(arr, i);
        const char *section_value = jext_get_string_dummy(tmp, section);

        if (strcmp(section_value, value) == 0) {
            return i;
        }
    }

    return -1;
}

void jext_replace_or_create_option_boolean(json_object *json, const char *key,
                                           int val) {
    json_object *tmp;

    if (json_object_object_get_ex(json, key, &tmp)) {
        json_object_set_boolean(tmp, val);
    } else {
        json_object_object_add(json, key, json_object_new_boolean(val));
    }
}

void jext_replace_or_create_option_int(json_object *json, const char *key,
                                       int val) {
    json_object *tmp;

    if (json_object_object_get_ex(json, key, &tmp)) {
        json_object_set_int(tmp, val);
    } else {
        json_object_object_add(json, key, json_object_new_int(val));
    }
}

void jext_replace_or_create_option_str(json_object *json, const char *key,
                                       const char *val) {
    json_object *tmp;

    if (json_object_object_get_ex(json, key, &tmp)) {
        json_object_set_string(tmp, val);
    } else {
        json_object_object_add(json, key, json_object_new_string(val));
    }
}
