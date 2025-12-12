#ifndef __CKESTER_SERVICES_FS_C__
#define __CKESTER_SERVICES_FS_C__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <direct.h>
#else
#include <dirent.h>
#include <unistd.h>
#endif

typedef enum Ckester_FsNodeType {
    CKESTER_FILE,
    CKESTER_DIRECTORY,
    CKESTER_UNKNOWN
} Ckester_FsNodeType;

typedef struct Ckester_FsNode {
    char* path;
    Ckester_FsNodeType type;
} Ckester_FsNode;

typedef struct Ckester_DirWalk {
    char* current_filepath;
    void* _internal; // For DIR*
} Ckester_DirWalk;

/* Function Prototypes */
Ckester_FsNode ckester_fs_get_node_info(const char* path);
size_t ckester_fs_get_file_size(const char* path);
Ckester_DirWalk ckester_fs_walk_dir(Ckester_FsNode* node);
void ckester_fs_walk_dir_continue(Ckester_DirWalk* walk);
void ckester_fs_walk_dir_close(Ckester_DirWalk* walk);


Ckester_FsNode ckester_fs_get_node_info(const char* path) {
    Ckester_FsNode node;
    node.path = strdup(path ? path : "");
    node.type = CKESTER_UNKNOWN;

    struct stat sb;
    if (stat(path, &sb) == 0) {
        if (S_ISDIR(sb.st_mode)) {
            node.type = CKESTER_DIRECTORY;
        } else if (S_ISREG(sb.st_mode)) {
            node.type = CKESTER_FILE;
        }
    }
    return node;
}

size_t ckester_fs_get_file_size(const char* path) {
    struct stat sb;
    if (stat(path, &sb) == 0) {
        return (size_t)sb.st_size;
    }
    return 0;
}

#ifndef _WIN32

// POSIX Implementation
typedef struct _Ckester_DirWalkInternal {
    DIR* d;
    char* dir_path;
} _Ckester_DirWalkInternal;

Ckester_DirWalk ckester_fs_walk_dir(Ckester_FsNode* node) {
    Ckester_DirWalk walk;
    walk.current_filepath = NULL;
    walk._internal = NULL;

    if (node->type != CKESTER_DIRECTORY) {
        return walk;
    }

    DIR* d = opendir(node->path);
    if (!d) return walk;

    _Ckester_DirWalkInternal* internal = malloc(sizeof(_Ckester_DirWalkInternal));
    internal->d = d;
    internal->dir_path = strdup(node->path);
    walk._internal = internal;

    ckester_fs_walk_dir_continue(&walk);
    return walk;
}

void ckester_fs_walk_dir_continue(Ckester_DirWalk* walk) {
    if (!walk->_internal) return;
    _Ckester_DirWalkInternal* internal = (_Ckester_DirWalkInternal*)walk->_internal;

    if (walk->current_filepath) {
        free(walk->current_filepath);
        walk->current_filepath = NULL;
    }

    struct dirent* dir;
    while ((dir = readdir(internal->d)) != NULL) {
        if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
            continue;
        }
        
        // Construct full path
        size_t len = strlen(internal->dir_path) + 1 + strlen(dir->d_name) + 1;
        char* fullpath = malloc(len);
        sprintf(fullpath, "%s/%s", internal->dir_path, dir->d_name);
        
        walk->current_filepath = fullpath;
        return; // Found next file
    }

    // End of stream
    ckester_fs_walk_dir_close(walk);
}

void ckester_fs_walk_dir_close(Ckester_DirWalk* walk) {
    if (walk->_internal) {
        _Ckester_DirWalkInternal* internal = (_Ckester_DirWalkInternal*)walk->_internal;
        if (internal->d) closedir(internal->d);
        if (internal->dir_path) free(internal->dir_path);
        free(internal);
        walk->_internal = NULL;
    }
    if (walk->current_filepath) {
        free(walk->current_filepath);
        walk->current_filepath = NULL;
    }
}

#else
// Windows implementation stub if needed, but assuming POSIX/Mac for now based on user info
Ckester_DirWalk ckester_fs_walk_dir(Ckester_FsNode* node) {
    Ckester_DirWalk walk = {0};
    return walk;
}
void ckester_fs_walk_dir_continue(Ckester_DirWalk* walk) {}
void ckester_fs_walk_dir_close(Ckester_DirWalk* walk) {}
#endif

#endif