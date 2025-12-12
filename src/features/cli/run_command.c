#ifndef __CKESTER_FEATURES_CLI_RUN_COMMAND_C__
#define __CKESTER_FEATURES_CLI_RUN_COMMAND_C__

#include <stdio.h>
#include <string.h>

#include "../../components/string.c"
#include "../../core/contracts.c"
#include "../../services/fs.c"
#include "../../services/sys.c"

static void _ckester_run_command_ensure_batch_compilation(
    Ckester_String* test_batch_compilation_command,
    char* current_filepath,
    size_t* accumulated_file_size,
    Ckester_CliContext* ctx

) {
    *accumulated_file_size += ckester_fs_get_file_size(current_filepath);

    if (*accumulated_file_size < CKESTER_DEFAULT_SIZE_PATH) {
        return;
    }

    ckester_string_push(test_batch_compilation_command, "-o ");
    ckester_string_push(test_batch_compilation_command, ctx->build_dir);
    ckester_string_push(test_batch_compilation_command, "/a.out");  // should gen a name someway
    ckester_sys_execute(test_batch_compilation_command);

    /**
     * Print the binary if the requested verbosity from the user desired
     * it.
     */
    if (ctx->verbosity.bin) {
        printf("[BIN]: \n");  // should add the name used for the bin, avoiding dynamic memory or something
    }

    ckester_string_clear(test_batch_compilation_command);
    *accumulated_file_size = 0;

    ckester_string_push(test_batch_compilation_command, ctx->c_compiler);
    ckester_string_push(test_batch_compilation_command, " ");
};

static void _ckester_run_command_add_filepath_to_batch_compilation(
    Ckester_String* test_batch_compilation_command,
    char* filepath,
    size_t* accumulated_file_size,
    Ckester_CliContext* ctx

) {
    if (ctx->filename_pattern && !ckester_string_match(filepath, ctx->filename_pattern)) {
        return;
    }

    ckester_string_push(test_batch_compilation_command, filepath);
    ckester_string_push(test_batch_compilation_command, " ");

    if (ctx->verbosity.src) {
        printf("[SRC]: %s\n", filepath);
    }
}

int ckester_run_command(Ckester_CliContext* ctx) {
    if (NULL == ctx->paths) {
        ctx->paths = strdup(CKESTER_DEFAULT_TEST_PATH);
    }

    char* path = strtok(ctx->paths, ",");
    /**
     * This will be the resulting command for compiling a batch of
     * tests. It will be reset every couple of kbs of C files.
     */
    Ckester_String test_batch_compilation_command = ckester_string_init((Ckester_StringInitParams){
        .capacity = 128,
    });
    size_t accumulated_file_compilation_size = 0;

    ckester_string_push(&test_batch_compilation_command, ctx->c_compiler);
    ckester_string_push(&test_batch_compilation_command, " ");

    while (path != NULL) {
        Ckester_FsNode node = ckester_fs_get_node_info(path);

        switch (node.type) {
            case CKESTER_DIRECTORY: {
                Ckester_DirWalk walk = ckester_fs_walk_dir(&node);

                while (NULL != walk.current_filepath) {
                    _ckester_run_command_ensure_batch_compilation(
                        &test_batch_compilation_command,
                        walk.current_filepath,
                        &accumulated_file_compilation_size,
                        ctx

                    );
                    _ckester_run_command_add_filepath_to_batch_compilation(
                        &test_batch_compilation_command,
                        walk.current_filepath,
                        &accumulated_file_compilation_size,
                        ctx

                    );

                    ckester_fs_walk_dir_continue(&walk);
                }

                ckester_fs_walk_dir_close(&walk);

                break;
            }
            case CKESTER_FILE:
                _ckester_run_command_ensure_batch_compilation(
                    &test_batch_compilation_command,
                    node.path,
                    &accumulated_file_compilation_size,
                    ctx

                );
                _ckester_run_command_add_filepath_to_batch_compilation(
                    &test_batch_compilation_command,
                    node.path,
                    &accumulated_file_compilation_size,
                    ctx

                );

                break;
            default:
                break;
        }

        if (node.path) free(node.path);

        path = strtok(NULL, ",");
    }

    ckester_string_free(&test_batch_compilation_command);

    return 0;
}

#endif