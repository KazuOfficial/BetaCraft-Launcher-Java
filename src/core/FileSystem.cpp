#include "FileSystem.h"

#include "Betacraft.h"
#include "Logger.h"

#include <archive.h>
#include <archive_entry.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <json.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#elif __APPLE__
#include <TargetConditionals.h>
#include <mach-o/dyld.h>
#endif

#if defined(__linux__) || defined(__APPLE__)
#include <pwd.h>
#include <unistd.h>
#endif

char *bc_file_make_absolute_path(const char *relative_path) {
    char *workdir = bc_file_directory_get_working();
    char *res = (char*)malloc(strlen(workdir) + strlen(relative_path) + 1);
    sprintf(res, "%s%s", workdir, relative_path);
    free(workdir);
#ifdef _WIN32
    for (int i = 0; i < strlen(res); i++) {
        if (res[i] == '\\') {
            res[i] = '/';
        }
    }
#endif
    return res;
}

char *bc_file_absolute_path(const char *relative_path) {
#ifdef _WIN32
    char path[_MAX_PATH + 2];
    _fullpath(path, relative_path, _MAX_PATH);
    int len = strlen(path);

    for (int i = 0; i < len; i++) {
        if (path[i] == '\\') {
            path[i] = '/';
        }
    }

    char *pathee = malloc(len + 1);
    strcpy(pathee, path);

    return pathee;
#elif defined(__linux__) || defined(__APPLE__)
    char real[PATH_MAX + 2];
    realpath(relative_path, real);
    int len = strlen(real);

    if (bc_file_directory_exists(real) && real[len - 1] != '/') {
        real[len] = '/';
        real[len + 1] = '\0';
        len++;
    }

    char *realee = (char*)malloc(len + 1);
    strcpy(realee, real);

    return realee;
#endif
}

off_t bc_file_size(const char *filepath) {
    struct stat st;
    stat(filepath, &st);

    return st.st_size;
}

int bc_file_exists(const char *filepath) {
    if (access(filepath, 0) == 0) {
        return 1;
    }

    return 0;
}

void bc_file_copy(const char *filepath, const char *destination) {
    FILE *source, *target;
    source = fopen(filepath, "rb");

    fseek(source, 0, SEEK_END);
    int length = ftell(source);

    fseek(source, 0, SEEK_SET);
    target = fopen(destination, "wb");

    if (target == NULL)
        fclose(source);

    for (int i = 0; i < length; i++) {
        fputc(fgetc(source), target);
    }

    fclose(source);
    fclose(target);
}

void bc_file_create(const char *filepath, const char *data) {
    if (!bc_file_exists(filepath)) {
        FILE *fp = fopen(filepath, "w+");
        if (fp == NULL) {
            bc_log("Error: bc_file_create failed %s\n", strerror(errno));
            return;
        }

        fprintf(fp, "%s", data);
        fclose(fp);
    }
}

bc_file_list_array *bc_file_list(const char *path) {
    bc_file_list_array *arr = (bc_file_list_array*)malloc(sizeof(bc_file_list_array));
    arr->len = 0;

    DIR *dir;
    struct dirent *ent;

    if ((dir = opendir(path)) == NULL) {
        bc_log("%s\n", "Error: bc_file_list failed");
        return arr;
    }

    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
            continue;

        struct stat statbuf;
        char file_path[PATH_MAX];
        snprintf(file_path, sizeof(file_path), "%s/%s", path, ent->d_name);

#ifdef _WIN32
        if (stat(file_path, &statbuf) != 0) {
            bc_log("%s\n", "ERROR: bc_file_list failed - stat");
            exit(EXIT_FAILURE);
        }
#elif defined(__APPLE__) || defined(__linux__)
        if (lstat(file_path, &statbuf) != 0) {
            bc_log("%s\n", "ERROR: bc_file_list failed - lstat");
            exit(EXIT_FAILURE);
        }
#endif

        snprintf(arr->arr[arr->len].name, sizeof(arr->arr[arr->len].name), "%s",
                 ent->d_name);

        arr->arr[arr->len].size = statbuf.st_size;
        arr->arr[arr->len].is_directory = S_ISDIR(statbuf.st_mode);
#if defined(__APPLE__) || defined(__linux__)
        arr->arr[arr->len].is_symlink = S_ISLNK(statbuf.st_mode);
#else
        arr->arr[arr->len].is_symlink = 0;
#endif
        arr->len++;
    }

    closedir(dir);

    return arr;
}

char *bc_file_minecraft_directory() {
    char *mcdir;
#ifdef __APPLE__
    int size = strlen(application_support_path) + strlen("/minecraft/") + 1;
    mcdir = (char*)malloc(size);
    snprintf(mcdir, size, "%s/minecraft/", application_support_path);
#elif __linux__
    struct passwd *pw = getpwuid(getuid());

    int size = strlen(pw->pw_dir) + strlen("/.minecraft/") + 1;
    mcdir = (char*)malloc(size);
    snprintf(mcdir, size, "%s/.minecraft/", pw->pw_dir);
#elif _WIN32
    char *env = getenv("APPDATA");

    int size = strlen(env) + strlen("/.minecraft/") + 1;
    mcdir = malloc(size);
    snprintf(mcdir, size, "%s/.minecraft/", env);
#endif
    return mcdir;
}

char *bc_file_directory_get_working() {
    char *real;
#ifdef _WIN32
    real = _getcwd(NULL, PATH_MAX);
#elif defined(__linux__) || defined(__APPLE__)
    real = (char*)malloc(PATH_MAX);
    getcwd(real, PATH_MAX);
#endif
    int len = strlen(real);

    if (bc_file_directory_exists(real) && real[len - 1] != '/') {
        real = (char*)realloc(real, len + 2);
        real[len] = '/';
        real[len + 1] = '\0';
    }

    return real;
}

void bc_file_directory_create(const char *name) {
#if defined(__linux__) || defined(__APPLE__)
    mode_t mode = 0755;
    mkdir(name, mode);
#elif _WIN32
    _mkdir(name);
#endif
}

void bc_file_directory_remove(const char *path) {
    int err = rmdir(path);

    if (err == -1 && errno == ENOTEMPTY) {
        bc_file_list_array *arr = bc_file_list(path);

        for (int i = 0; i < arr->len; i++) {
            char filepath[PATH_MAX];
            snprintf(filepath, sizeof(filepath), "%s/%s", path,
                     arr->arr[i].name);

            if (arr->arr[i].is_directory != 0) {
                bc_file_directory_remove(filepath);
            } else {
                remove(filepath);
            }
        }

        rmdir(path);
        free(arr);
    }
}

void bc_file_directory_copy(const char *source, const char *destination) {
    bc_file_list_array *files = bc_file_list(source);

    for (int i = 0; i < files->len; i++) {
        char sPath[PATH_MAX];
        snprintf(sPath, sizeof(sPath), "%s/%s", source, files->arr[i].name);

        char dPath[PATH_MAX];
        snprintf(dPath, sizeof(dPath), "%s/%s", destination,
                 files->arr[i].name);

        if (files->arr[i].is_directory == 1) {
            make_path(dPath, 0);
            bc_file_directory_copy(sPath, dPath);
            continue;
        } else if (files->arr[i].is_symlink == 1) {
#if defined(__linux__) || defined(__APPLE__)
            char linkTarget[PATH_MAX];
            int len = readlink(sPath, linkTarget, sizeof(linkTarget));
            linkTarget[len] = '\0';
            symlink(linkTarget, dPath);
#endif
        } else {
            bc_file_copy(sPath, dPath);
        }
    }

    free(files);
}

int bc_file_directory_exists(const char *folder) {
    int result = access(folder, 0);

    if (result == 0) {
        struct stat status;
        stat(folder, &status);

        return (status.st_mode & S_IFDIR) != 0;
    }

    return 0;
}

static int do_mkdir(const char *path) {
    struct stat st;
    int status = 0;

    if (stat(path, &st) != 0) {
        int res;
#ifdef _WIN32
        res = mkdir(path);
#else
        mode_t mode = 0755;
        res = mkdir(path, mode);
#endif
        /* Directory does not exist. EEXIST for race condition */
        if (res != 0 && errno != EEXIST)
            status = -1;
    } else if (st.st_mode & (S_IFDIR == 0)) {
        errno = ENOTDIR;
        status = -1;
    }

    return (status);
}

/**
** mkpath - ensure all directories in path exist
** Algorithm takes the pessimistic view and works top-down to ensure
** each directory in path exists, rather than optimistically creating
** the last element and working backwards.
*/
int make_path(const char *path, int onlyParents) {
    char *pp;
    char *sp;
    int status;
    char *copypath = strdup(path);

    status = 0;
    pp = copypath;
    while (status == 0 && (sp = strchr(pp, '/')) != 0) {
        if (sp != pp) {
            /* Neither root nor double slash in path */
            *sp = '\0';
            status = do_mkdir(copypath);
            *sp = '/';
        }
        pp = sp + 1;
    }
    if (status == 0 && onlyParents == 0)
        status = do_mkdir(path);

    free(copypath);
    return (status);
}

void bc_file_clean() {
    bc_file_directory_remove("instances/");
    bc_file_directory_remove("versions/");
    bc_file_directory_remove("libraries/");

    remove("settings.json");
    remove("accounts.json");
}

void bc_file_init() {
    make_path("instances/", 0);
    make_path("versions/", 0);
    make_path("libraries/", 0);

    json_object *settings = json_object_new_object();
    json_object *settings_java = json_object_new_object();
    json_object *accounts = json_object_new_object();
    json_object *instance = json_object_new_object();

    json_object_object_add(instance, "selected", json_object_new_string(""));
    json_object_object_add(instance, "standalone", json_object_new_array());
    json_object_object_add(instance, "grouped", json_object_new_array());

    json_object_object_add(settings_java, "selected",
                           json_object_new_string(""));
    json_object_object_add(settings_java, "installations",
                           json_object_new_array());

    json_object_object_add(settings, "language",
                           json_object_new_string("English"));
    json_object_object_add(settings, "discord", json_object_new_boolean(1));
    json_object_object_add(settings, "instance", instance);
    json_object_object_add(settings, "java", settings_java);

    json_object_object_add(accounts, "selected", json_object_new_string(""));
    json_object_object_add(accounts, "accounts", json_object_new_array());

    bc_file_create("settings.json", json_object_to_json_string(settings));
    bc_file_create("accounts.json", json_object_to_json_string(accounts));

    json_object_put(settings);
    json_object_put(accounts);

    remove("betacraft.log"); // start a fresh log every time

    bc_log("%s\n", "Files initialized");
}

int copy_data(struct archive *ar, struct archive *aw) {
    int r;
    const void *buff;
    size_t size;
    la_int64_t offset;

    for (;;) {
        r = archive_read_data_block(ar, &buff, &size, &offset);

        if (r == ARCHIVE_EOF)
            return (ARCHIVE_OK);
        if (r < ARCHIVE_OK)
            return (r);

        r = archive_write_data_block(aw, buff, size, offset);

        if (r < ARCHIVE_OK) {
            bc_log("%s\n", archive_error_string(aw));
            return (r);
        }
    }
}

void bc_file_extract(const char *filepath, const char *dest) {
    struct archive *a;
    struct archive *ext;
    struct archive_entry *entry;
    int flags;
    int r;

    flags = ARCHIVE_EXTRACT_TIME;
    flags |= ARCHIVE_EXTRACT_PERM;
    flags |= ARCHIVE_EXTRACT_ACL;
    flags |= ARCHIVE_EXTRACT_FFLAGS;

    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);

    ext = archive_write_disk_new();
    archive_write_disk_set_options(ext, flags);
    archive_write_disk_set_standard_lookup(ext);

    if ((r = archive_read_open_filename(a, filepath, 10240))) {
        bc_log("%s %s\n", "bc_file_extract - couldn't extract", filepath);
        return;
    }

    for (;;) {
        r = archive_read_next_header(a, &entry);

        const char *path = archive_entry_pathname(entry);
        char newPath[PATH_MAX];
        snprintf(newPath, sizeof(newPath), "%s/%s", dest, path);
        archive_entry_set_pathname(entry, newPath);

        if (r == ARCHIVE_EOF)
            break;
        if (r < ARCHIVE_OK)
            bc_log("%s\n", archive_error_string(a));
        if (r < ARCHIVE_WARN) {
            bc_log("%s\n", archive_error_string(a));
            return;
        }

        r = archive_write_header(ext, entry);

        if (r < ARCHIVE_OK)
            bc_log("%s\n", archive_error_string(a));
        else if (archive_entry_size(entry) > 0) {
            r = copy_data(a, ext);
            if (r < ARCHIVE_OK)
                bc_log("%s\n", archive_error_string(a));
            if (r < ARCHIVE_WARN) {
                bc_log("%s\n", archive_error_string(a));
                return;
            }
        }
        r = archive_write_finish_entry(ext);
        if (r < ARCHIVE_OK)
            bc_log("%s\n", archive_error_string(a));
        if (r < ARCHIVE_WARN) {
            bc_log("%s\n", archive_error_string(a));
            return;
        }
    }

    archive_read_close(a);
    archive_read_free(a);
    archive_write_close(ext);
    archive_write_free(ext);
}

void bc_file_archive_directory(const char *p, int n, struct archive *a,
                               struct archive_entry *entry) {
    char buff[8192];
    int len;
    int fd;

    bc_file_list_array *files = bc_file_list(p);

    for (int i = 0; i < files->len; i++) {
        char dirPath[PATH_MAX];
        snprintf(dirPath, sizeof(dirPath), "%s", p);

        if (p[strlen(p) - 1] != '/')
            strcat(dirPath, "/");

        strcat(dirPath, files->arr[i].name);
        char *path = dirPath + n;

        entry = archive_entry_new();

        archive_entry_set_pathname(entry, path);
        archive_entry_set_size(entry, files->arr[i].size);
        archive_entry_set_filetype(
            entry, files->arr[i].is_directory ? AE_IFDIR : AE_IFREG);
        archive_write_header(a, entry);

#ifdef _WIN32
        int mode = O_BINARY;
#else
        int mode = O_RDONLY;
#endif

        fd = open(dirPath, mode);
        len = read(fd, buff, sizeof(buff));

        while (len > 0) {
            archive_write_data(a, buff, len);
            len = read(fd, buff, sizeof(buff));
        }

        close(fd);
        archive_entry_free(entry);

        if (files->arr[i].is_directory) {
            bc_file_archive_directory(dirPath, n, a, entry);
        }
    }

    free(files);
}

void bc_file_archive(const char *directory, const char *filename) {
    struct archive *a;
    struct archive_entry *entry;

    a = archive_write_new();

    archive_write_add_filter_none(a);
    archive_write_set_format_zip(a);
    archive_write_open_filename(a, filename);

    bc_file_archive_directory(directory, strlen(directory), a, entry);

    archive_write_close(a);
    archive_write_free(a);
}

char *bc_file_uuid() {
    char v[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    char *buf = (char*)malloc(37);

    srand(time(NULL));
    for (int i = 0; i < 36; ++i) {
        buf[i] = v[rand() % 16];
    }

    buf[8] = '-';
    buf[13] = '-';
    buf[18] = '-';
    buf[23] = '-';
    buf[36] = '\0';

    return buf;
}

char *bc_file_os() {
    char *os = (char*)malloc(32);
#ifdef _WIN64
    strcpy(os, "win64");
#elif _WIN32
    strcpy(os, "win32");
#elif __linux__
    strcpy(os, "linux");
#elif __APPLE__

#ifdef __aarch64__
    strcpy(os, "macos-arm64");
#elif __i386__
    strcpy(os, "macos-i386");
#elif __ppc__
    strcpy(os, "macos-ppc");
#elif __ppc64__
    strcpy(os, "macos-ppc64");
#else
    strcpy(os, "macos-x86_64");
#endif

#endif

    return os;
}
