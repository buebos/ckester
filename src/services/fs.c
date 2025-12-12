#ifndef __CKESTER_SERVICES_FS_C__
#define __CKESTER_SERVICES_FS_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

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
    void* _internal;  // For DIR*
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
// POSIX Implementation
typedef struct _Ckester_DirWalkStackItem {
    DIR* d;
    char* dir_path;
    struct _Ckester_DirWalkStackItem* next;
} _Ckester_DirWalkStackItem;

typedef struct _Ckester_DirWalkInternal {
    _Ckester_DirWalkStackItem* stack;
} _Ckester_DirWalkInternal;

static void _ckester_fs_walk_push(_Ckester_DirWalkInternal* internal, const char* path) {
    DIR* d = opendir(path);
    if (!d) return;

    _Ckester_DirWalkStackItem* item = malloc(sizeof(_Ckester_DirWalkStackItem));
    item->d = d;
    item->dir_path = strdup(path);
    item->next = internal->stack;
    internal->stack = item;
}

static void _ckester_fs_walk_pop(_Ckester_DirWalkInternal* internal) {
    if (!internal->stack) return;

    _Ckester_DirWalkStackItem* item = internal->stack;
    internal->stack = item->next;

    closedir(item->d);
    free(item->dir_path);
    free(item);
}

Ckester_DirWalk ckester_fs_walk_dir(Ckester_FsNode* node) {
    Ckester_DirWalk walk;
    walk.current_filepath = NULL;
    walk._internal = NULL;

    if (node->type != CKESTER_DIRECTORY) {
        return walk;
    }

    _Ckester_DirWalkInternal* internal = malloc(sizeof(_Ckester_DirWalkInternal));
    internal->stack = NULL;

    _ckester_fs_walk_push(internal, node->path);

    if (!internal->stack) {
        free(internal);
        return walk;
    }

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

    while (internal->stack) {
        _Ckester_DirWalkStackItem* item = internal->stack;
        struct dirent* dir;

        // Read next entry from current directory
        while ((dir = readdir(item->d)) != NULL) {
            if (strcmp(dir->d_name, ".") == 0 || strcmp(dir->d_name, "..") == 0) {
                continue;
            }

            // Construct full path
            size_t len = strlen(item->dir_path) + 1 + strlen(dir->d_name) + 1;
            char* fullpath = malloc(len);
            sprintf(fullpath, "%s/%s", item->dir_path, dir->d_name);

            struct stat sb;
            if (stat(fullpath, &sb) == 0 && S_ISDIR(sb.st_mode)) {
                // If directory, push to stack and process immediately (depth-first)
                // We don't return directory path itself as the current_filepath,
                // but we could if we wanted to process directories themselves.
                // The current usage implies file collection mostly, but let's stick to
                // just recursing for now.
                _ckester_fs_walk_push(internal, fullpath);
                free(fullpath);
                // Break inner loop to process new top of stack
                break;
            } else {
                // It's a file (or other), return it
                walk->current_filepath = fullpath;
                return;
            }
        }

        // If we finished the directory (dir == NULL), or pushed a new one (stack changed)
        // If stack changed, the loop `while(internal->stack)` will continue with new top.
        // If dir == NULL, we are done with this dir, so pop it.
        if (dir == NULL) {
            _ckester_fs_walk_pop(internal);
        }
    }

    // Stack empty = done
    ckester_fs_walk_dir_close(walk);
}

void ckester_fs_walk_dir_close(Ckester_DirWalk* walk) {
    if (walk->_internal) {
        _Ckester_DirWalkInternal* internal = (_Ckester_DirWalkInternal*)walk->_internal;
        while (internal->stack) {
            _ckester_fs_walk_pop(internal);
        }
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