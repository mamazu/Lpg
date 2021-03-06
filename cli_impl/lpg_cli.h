#pragma once
#include "lpg_expression.h"
#include "lpg_stream_writer.h"
#include <stdbool.h>

typedef struct cli_parser_user
{
    stream_writer diagnostics;
    bool has_error;
} cli_parser_user;

typedef struct optional_sequence
{
    bool has_value;
    sequence value;
} optional_sequence;

optional_sequence parse(cli_parser_user user, unicode_view const file_name, unicode_view const source);

typedef enum compiler_command {
    compiler_command_run = 1,
    compiler_command_compile,
    compiler_command_format,
    compiler_command_web
#ifdef LPG_NODEJS
    ,
    compiler_command_node
#endif
} compiler_command;

typedef struct compiler_arguments
{
    bool valid;
    compiler_command command;
    char *file_name;
    char const *output_file_name;
} compiler_arguments;

bool run_cli(int const argc, LPG_NON_NULL(char **const argv), stream_writer const diagnostics,
             unicode_view const current_directory, unicode_view const module_directory);
