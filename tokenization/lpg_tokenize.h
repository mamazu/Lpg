#pragma once
#include "lpg_unicode_string.h"

static size_t const spaces_for_indentation = 4;

typedef enum token_type
{
    token_newline,
    token_comment,
    token_space,
    token_indentation,
    token_identifier,
    token_left_parenthesis,
    token_right_parenthesis,
    token_left_curly_brace,
    token_right_curly_brace,
    token_comma,
    token_dot,
    token_colon,
    token_assign,
    token_fat_arrow,
    token_integer,
    token_match,
    token_return,
    token_case,
    token_loop,
    token_break,
    token_let,
    token_string
} token_type;

typedef enum tokenize_status
{
    tokenize_success,
    tokenize_invalid
} tokenize_status;

typedef struct tokenize_result
{
    tokenize_status status;
    token_type token;
    size_t length;
} tokenize_result;

tokenize_result tokenize(char const *input, size_t length);
