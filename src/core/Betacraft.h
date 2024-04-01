#ifndef BC_BETACRAFT_H
#define BC_BETACRAFT_H

#include <cstddef>
#include <limits.h>
#include <string>

#define BETACRAFT_VERSION "2.0.0-alpha.20230623"
#define BETACRAFT_MAX_SIZE 65536
#define BETACRAFT_MAX_UPDATE_TAG_SIZE 64
#define DEMO_ACCOUNT_UUID "bd346dd5-ac1c-427d-87e8-73bdd4bf3e13"

#define BC_SERVER_NAME_SIZE 64
#define BC_SERVER_DESCRIPTION_SIZE 1024
#define BC_SERVER_SOFTWARE_NAME_SIZE 128
#define BC_SERVER_SOFTWARE_VERSION_SIZE 64
#define BC_SERVER_PLAYER_NAMES_SIZE 4096
#define BC_SERVER_VERSION_CATEGORY_SIZE 128
#define BC_SERVER_CONNECT_VERSION_SIZE 64
#define BC_SERVER_CONNECT_PROTOCOL_SIZE 64
#define BC_SERVER_CONNECT_SOCKET_SIZE 64

#define BC_SERVER_ARRAY_MAX_SIZE 256

extern int betacraft_online;
extern char application_support_path[PATH_MAX];

typedef enum bc_download_type {
    BC_DOWNLOAD_TYPE_ASSETS,
    BC_DOWNLOAD_TYPE_LIBRARIES,
    BC_DOWNLOAD_TYPE_VERSION
} bc_download_type;

struct bc_progress {
    int cur;
    int total;
    int progress;
    bc_download_type download_type;
} typedef bc_progress;

typedef struct bc_server {
    char name[BC_SERVER_NAME_SIZE];
    char description[BC_SERVER_DESCRIPTION_SIZE];
    char icon[BETACRAFT_MAX_SIZE];
    int is_public;
    char software_name[BC_SERVER_SOFTWARE_NAME_SIZE];
    char software_version[BC_SERVER_SOFTWARE_VERSION_SIZE];
    int max_players;
    int online_players;
    char player_names[BC_SERVER_PLAYER_NAMES_SIZE];
    int last_ping_time;
    char version_category[BC_SERVER_VERSION_CATEGORY_SIZE];
    char connect_version[BC_SERVER_CONNECT_VERSION_SIZE];
    char connect_protocol[BC_SERVER_CONNECT_PROTOCOL_SIZE];
    char connect_socket[BC_SERVER_CONNECT_SOCKET_SIZE];
    int connect_online_mode;
} bc_server;

typedef struct bc_server_array {
    bc_server arr[BC_SERVER_ARRAY_MAX_SIZE];
    size_t len;
} bc_server_array;

int bc_server_list(bc_server_array *server_list);
std::string bc_avatar_get(const char *uuid);
int bc_update_check(char *updateVersion);

#endif
