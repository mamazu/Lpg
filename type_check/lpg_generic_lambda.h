#pragma once

#include "lpg_expression.h"
#include "lpg_generic_interface.h"
#include "lpg_source_file.h"
#include "lpg_value.h"

typedef struct generic_lambda
{
    lambda tree;
    generic_closures closures;
    source_file_owning const *file;
    unicode_string current_import_directory;
} generic_lambda;

generic_lambda generic_lambda_create(lambda tree, generic_closures closures, source_file_owning const *file,
                                     unicode_string current_import_directory);
void generic_lambda_free(generic_lambda const freed);
