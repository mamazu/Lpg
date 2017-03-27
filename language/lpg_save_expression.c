#include "lpg_save_expression.h"
#include "lpg_assert.h"
#include "lpg_for.h"

success_indicator save_expression(stream_writer const to,
                                  expression const *value, size_t indentation)
{
    switch (value->type)
    {
    case expression_type_lambda:
        LPG_TRY(stream_writer_write_string(to, "("));
        LPG_TRY(save_expression(to, value->lambda.parameter_name, indentation));
        LPG_TRY(stream_writer_write_string(to, ": "));
        LPG_TRY(save_expression(to, value->lambda.parameter_type, indentation));
        LPG_TRY(stream_writer_write_string(to, ") => "));
        LPG_TRY(save_expression(to, value->lambda.result, indentation));
        return success;

    case expression_type_builtin:
        switch (value->builtin)
        {
        case builtin_unit:
            return stream_writer_write_string(to, "()");

        case builtin_empty_structure:
            return stream_writer_write_string(to, "{}");

        case builtin_empty_variant:
            return stream_writer_write_string(to, "{}");
        }
        UNREACHABLE();

    case expression_type_call:
        LPG_TRY(save_expression(to, value->call.callee, indentation));
        LPG_TRY(stream_writer_write_string(to, "("));
        LPG_FOR(size_t, i, value->call.number_of_arguments)
        {
            if (i > 0)
            {
                LPG_TRY(stream_writer_write_string(to, ", "));
            }
            LPG_TRY(
                save_expression(to, &value->call.arguments[i], indentation));
        }
        LPG_TRY(stream_writer_write_string(to, ")"));
        return success;

    case expression_type_local:
        UNREACHABLE();

    case expression_type_integer_literal:
    {
        char buffer[39];
        char *const formatted =
            integer_format(value->integer_literal, lower_case_digits, 10,
                           buffer, sizeof(buffer));
        return stream_writer_write_bytes(
            to, formatted, (size_t)(buffer + sizeof(buffer) - formatted));
    }

    case expression_type_integer_range:
    case expression_type_function:
    case expression_type_add_member:
    case expression_type_fill_structure:
    case expression_type_access_structure:
    case expression_type_add_to_variant:
    case expression_type_match:
    case expression_type_assignment:
        UNREACHABLE();

    case expression_type_string:
        LPG_TRY(stream_writer_write_string(to, "\""));
        LPG_FOR(size_t, i, value->string.length)
        {
            switch (value->string.data[i])
            {
            case '\"':
            case '\'':
            case '\\':
                LPG_TRY(stream_writer_write_string(to, "\\"));
                break;
            }
            LPG_TRY(stream_writer_write_utf8(to, value->string.data[i]));
        }
        LPG_TRY(stream_writer_write_string(to, "\""));
        return success;

    case expression_type_identifier:
        LPG_FOR(size_t, i, value->identifier.length)
        {
            LPG_TRY(stream_writer_write_utf8(to, value->string.data[i]));
        }
        return success;

    case expression_assign:
        LPG_TRY(save_expression(to, value->assign.left, indentation));
        LPG_TRY(stream_writer_write_string(to, " = "));
        LPG_TRY(save_expression(to, value->assign.right, indentation));
        return stream_writer_write_string(to, "\n");

    case expression_return:
        LPG_TRY(stream_writer_write_string(to, "return "));
        LPG_TRY(save_expression(to, value->return_, indentation));
        return stream_writer_write_string(to, "\n");

    case expression_loop:
        LPG_TRY(stream_writer_write_string(to, "loop\n"));
        LPG_FOR(size_t, i, (indentation + 1))
        {
            LPG_TRY(stream_writer_write_string(to, "    "));
        }
        return save_expression(to, value->loop_body, indentation + 1);

    case expression_break:
        return stream_writer_write_string(to, "break\n");

    case expression_sequence:
        LPG_FOR(size_t, i, value->sequence.length)
        {
            if (i > 0)
            {
                LPG_FOR(size_t, j, indentation)
                {
                    LPG_TRY(stream_writer_write_string(to, "    "));
                }
            }
            LPG_TRY(
                save_expression(to, value->sequence.elements + i, indentation));
        }
        return success;
    }
    UNREACHABLE();
}