#ifndef __CKESTER_FEATURES_CLI_RUN_COMMAND_C__
#define __CKESTER_FEATURES_CLI_RUN_COMMAND_C__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../components/string.c"
#include "../../core/contracts.c"
#include "../../services/fs.c"
#include "../../services/sys.c"

static void _ckester_run_command_ensure_batch_exec(
    Ckester_String* test_batch_compilation_command,
    char* current_filepath,
    size_t* accumulated_file_size,
    Ckester_CliContext* ctx

) {
    static int batch_counter = 0;

    if (NULL != current_filepath) {
        *accumulated_file_size += ckester_fs_get_file_size(current_filepath);

        if (*accumulated_file_size < CKESTER_DEFAULT_SIZE_PATH) {
            return;
        }
    }

    char binary_path[256];
    sprintf(binary_path, "%s/ckester_batch_%d.out", ctx->build_dir, batch_counter++);

    ckester_string_push(test_batch_compilation_command, "-o ");
    ckester_string_push(test_batch_compilation_command, binary_path);

    int compile_result = ckester_sys_execute(test_batch_compilation_command->data);

    if (ctx->verbosity.src) {
        printf("[SRC]: ");

        /**
         * Since the command by this point has a pattern like: 'cc file1.c ...'
         * we can print the sources used to compile this batch binary by
         * separating the string by spaces.
         */
        char* src_filepath = strtok(test_batch_compilation_command->data, " ");
        src_filepath = strtok(NULL, " "); /** Skips the c compiler prefix */

        while (NULL != src_filepath) {
            printf("%s", src_filepath);
            src_filepath = strtok(NULL, " ");
            if (strcmp(src_filepath, "-o") == 0) break;
            if (src_filepath) printf(", ");
        }

        printf("\n");
    }

    if (compile_result == 0) {
        if (ctx->verbosity.bin) {
            printf("[BIN]: Running %s\n", binary_path);
        }

        Ckester_String run_cmd = ckester_string_init((Ckester_StringInitParams){0});
        ckester_string_push(&run_cmd, binary_path);

        ckester_sys_execute(run_cmd.data);
        ckester_string_free(&run_cmd);
    } else {
        printf("[ERROR]: Compilation failed for batch %d\n", batch_counter - 1);
    }

    ckester_string_clear(test_batch_compilation_command);
    *accumulated_file_size = 0;

    ckester_string_push(test_batch_compilation_command, ctx->c_compiler);
    ckester_string_push(test_batch_compilation_command, " ");
};

static void _ckester_run_command_add_filepath_to_batch(
    Ckester_String* test_batch_compilation_command,
    char* filepath,
    size_t* accumulated_file_size,
    Ckester_CliContext* ctx

) {
    if (ctx->filename_pattern && !ckester_string_match(filepath, ctx->filename_pattern)) {
        return;
    }
    size_t filepath_len = strlen(filepath);

    if (
        !ckester_native_string_endswith(filepath, filepath_len, ".c", 2) &&
        !ckester_native_string_endswith(filepath, filepath_len, ".sh", 3) &&
        !ckester_native_string_endswith(filepath, filepath_len, ".bash", 5)

    ) {
        return;
    }

    ckester_string_push(test_batch_compilation_command, filepath);
    ckester_string_push(test_batch_compilation_command, " ");
}

int ckester_run_command(Ckester_CliContext* ctx) {
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

    char* make_build_dir = calloc(strlen(ctx->build_dir) + 16, sizeof(char));
    sprintf(make_build_dir, "mkdir -p %s", ctx->build_dir);
    ckester_sys_execute(make_build_dir);
    free(make_build_dir);

    while (path != NULL) {
        Ckester_FsNode node = ckester_fs_get_node_info(path);

        switch (node.type) {
            case CKESTER_DIRECTORY: {
                Ckester_DirWalk walk = ckester_fs_walk_dir(&node);

                while (NULL != walk.current_filepath) {
                    _ckester_run_command_ensure_batch_exec(
                        &test_batch_compilation_command,
                        walk.current_filepath,
                        &accumulated_file_compilation_size,
                        ctx

                    );
                    _ckester_run_command_add_filepath_to_batch(
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
                _ckester_run_command_ensure_batch_exec(
                    &test_batch_compilation_command,
                    node.path,
                    &accumulated_file_compilation_size,
                    ctx

                );
                _ckester_run_command_add_filepath_to_batch(
                    &test_batch_compilation_command,
                    node.path,
                    &accumulated_file_compilation_size,
                    ctx

                );

                break;
            default:
                printf("[ERROR]: Some node type wasn't expected\n");
                return 1;
        }

        if (node.path) free(node.path);

        path = strtok(NULL, ",");
    }

    /**
     * This will ensure batch compilation even if the file compilation
     * size wasn't exceeded. Pay attention to the NULL pointer.
     */
    _ckester_run_command_ensure_batch_exec(
        &test_batch_compilation_command,
        NULL,
        &accumulated_file_compilation_size,
        ctx

    );

    ckester_string_free(&test_batch_compilation_command);

    return 0;
}

#endif