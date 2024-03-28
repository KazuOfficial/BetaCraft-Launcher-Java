#include "Network.h"
#include "Betacraft.h"
#include "Logger.h"

#include <curl/curl.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

bc_download_progress bc_network_progress;
int bc_network_cancel = 0;

static size_t cb(void *data, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    bc_memory *mem = (bc_memory *)userp;

    char *ptr = (char*)realloc(mem->response, mem->size + realsize + 1);
    if (ptr == NULL)
        return 0;

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

int progress_callback(void *ptr, double totalToDownload, double nowDownloaded) {
    bc_network_progress.totalToDownload = totalToDownload;
    bc_network_progress.totalToDownloadMb =
        totalToDownload > 0 ? ((totalToDownload / 1024.0) / 1024.0) : 0;
    bc_network_progress.nowDownloaded = nowDownloaded;
    bc_network_progress.nowDownloadedMb =
        nowDownloaded > 0 ? ((nowDownloaded / 1024.0) / 1024.0) : 0;

    if (bc_network_cancel) {
        bc_network_cancel = 0;
        return -1;
    }

    return 0;
}

bc_memory bc_network_get_chunk(const char *url) {
    CURL *curl;
    CURLcode res;

    bc_memory chunk = {0};

    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, BETACRAFT_MAX_SIZE);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            bc_log("GET failed: %s\n", curl_easy_strerror(res));
        }

        curl_easy_cleanup(curl);
    }

    return chunk;
}

char *bc_network_get(const char *url, const char *header) {
    CURL *curl;
    CURLcode res;

    bc_memory chunk = {0};

    curl = curl_easy_init();

    if (curl) {
        struct curl_slist *curl_headers = NULL;
        if (header != NULL) {
            curl_headers = curl_slist_append(curl_headers, header);
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
        }

        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, BETACRAFT_MAX_SIZE);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            bc_log("GET failed: %s\n", curl_easy_strerror(res));
        }

        if (header != NULL) {
            curl_slist_free_all(curl_headers);
        }
        curl_easy_cleanup(curl);
    }

    return chunk.response;
}

char *bc_network_post(const char *url, const char *data, const char *header) {
    CURL *curl;
    CURLcode res;

    bc_memory chunk = {0};

    curl = curl_easy_init();

    if (curl) {
        struct curl_slist *curl_headers = NULL;
        if (header != NULL) {
            curl_headers = curl_slist_append(curl_headers, header);
        }

        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, BETACRAFT_MAX_SIZE);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_headers);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            bc_log("POST failed: %s\n", curl_easy_strerror(res));
        }

        if (header != NULL) {
            curl_slist_free_all(curl_headers);
        }
        curl_easy_cleanup(curl);
    }

    return chunk.response;
}

int bc_network_download(const char *url, const char *dest, int isFile) {
    CURL *curl;
    CURLcode res;

    FILE *fp;

    curl = curl_easy_init();

    if (curl) {
        char *split = (char*)strrchr(url, '/');

        char filename[PATH_MAX];
        snprintf(filename, sizeof(filename), "%s", split);

        char *dropboxParams = filename + (strlen(filename) - 5);

        if (strcmp(dropboxParams, "?dl=1") == 0)
            filename[strlen(filename) - 5] = '\0';

        char path[PATH_MAX];

        if (!isFile) {
            snprintf(path, sizeof(path), "%s%s", dest, filename);
        } else {
            snprintf(path, sizeof(path), "%s", dest);
        }

        snprintf(bc_network_progress.filename,
                 sizeof(bc_network_progress.filename), "%s", url);

        fp = fopen(path, "wb");

        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, BETACRAFT_MAX_SIZE);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, fwrite);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0);
        curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            bc_log("Download failed: %s\n", curl_easy_strerror(res));
            return 0;
        }

        bc_network_progress.filename[0] = '\0';

        fclose(fp);
        curl_easy_cleanup(curl);
    }

    return 1;
}

int bc_network_status() {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "http://checkip.amazonaws.com/");

        char *data = 0;
        curl_easy_setopt(curl, CURLOPT_BUFFERSIZE, BETACRAFT_MAX_SIZE);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, cb);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);

        res = curl_easy_perform(curl);
        CURLcode expected_res = CURLE_OK;

#ifndef NDEBUG
        bc_log("%s\n", "DEBUG");
        expected_res = CURLE_WRITE_ERROR;
#endif

        if (res != expected_res) {
            bc_log("Failed: %s\n", curl_easy_strerror(res));
            return 0;
        }

        curl_easy_cleanup(curl);
    }

    return 1;
}
