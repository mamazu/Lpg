#include "lpg_console.h"
#include "lpg_expression.h"
#include "lpg_allocate.h"
#include <stdio.h>

void render_expression(console_printer *printer, expression const *source)
{
    switch (source->type)
    {
    case expression_type_lambda:
        console_print_char(printer, '/', console_color_cyan);
        render_expression(printer, source->lambda.parameter_type);
        console_print_char(printer, ' ', console_color_grey);
        render_expression(printer, source->lambda.parameter_name);
        console_new_line(printer);
        console_print_char(printer, ' ', console_color_grey);
        render_expression(printer, source->lambda.result);
        break;

    case expression_type_builtin:
        switch (source->builtin)
        {
        case builtin_unit:
            console_print_char(printer, '(', console_color_grey);
            console_print_char(printer, ')', console_color_grey);
            break;

        case builtin_empty_structure:
            console_print_char(printer, '{', console_color_grey);
            console_print_char(printer, '}', console_color_grey);
            break;

        case builtin_empty_variant:
            console_print_char(printer, '[', console_color_grey);
            console_print_char(printer, ']', console_color_grey);
            break;
        }
        break;
    case expression_type_call:
        abort();
        break;
    case expression_type_local:
        abort();
        break;

    case expression_type_integer_literal:
    {
        char buffer[2 + (4 * 8) + 1];
        if (snprintf(buffer, sizeof(buffer), "0x%08X%08X%08X%08X",
                     (unsigned)(source->integer_literal.high >> 32u),
                     (unsigned)source->integer_literal.high,
                     (unsigned)(source->integer_literal.low >> 32u),
                     (unsigned)source->integer_literal.low) !=
            (sizeof(buffer) - 1))
        {
            abort();
        }
        LPG_FOR(size_t, i, sizeof(buffer) - 1)
        {
            console_print_char(printer, buffer[i], console_color_white);
        }
        break;
    }

    case expression_type_integer_range:
        abort();
        break;
    case expression_type_function:
        abort();
        break;
    case expression_type_add_member:
        abort();
        break;
    case expression_type_fill_structure:
        abort();
        break;
    case expression_type_access_structure:
        abort();
        break;
    case expression_type_add_to_variant:
        abort();
        break;
    case expression_type_match:
        abort();
        break;
    case expression_type_sequence:
        abort();
        break;
    case expression_type_assignment:
        abort();
        break;
    case expression_type_utf8_literal:
        LPG_FOR(size_t, i, source->utf8_literal.length)
        {
            console_print_char(
                printer, source->utf8_literal.data[i], console_color_white);
        }
        break;
    }
}

enum
{
    console_width = 80,
    console_height = 24
};

void render(expression const *source)
{
    console_cell cells[console_width * console_height];
    LPG_FOR(size_t, y, console_height)
    {
        LPG_FOR(size_t, x, console_width)
        {
            console_cell *cell = &cells[(y * console_width) + x];
            cell->text = ' ';
            cell->text_color = console_color_white;
        }
    }
    console_printer printer = {&cells[0], console_width, console_height, 0, 0};
    render_expression(&printer, source);
    print_to_console(cells, console_width, console_height);
}

void run_editor(expression *source)
{
    render(source);
    HANDLE consoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    HANDLE consoleInput = GetStdHandle(STD_INPUT_HANDLE);
    if (!SetConsoleMode(consoleInput, ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT |
                                          ENABLE_EXTENDED_FLAGS |
                                          ENABLE_INSERT_MODE))
    {
        abort();
    }

    for (;;)
    {
        INPUT_RECORD input;
        DWORD inputEvents;
        if (!ReadConsoleInput(consoleInput, &input, 1, &inputEvents))
        {
            abort();
        }

        switch (input.EventType)
        {
        case KEY_EVENT:
        {
            if (!input.Event.KeyEvent.bKeyDown)
            {
                if ((input.Event.KeyEvent.wVirtualKeyCode == 0x53) &&
                    (input.Event.KeyEvent.dwControlKeyState &
                     (LEFT_CTRL_PRESSED | RIGHT_CTRL_PRESSED)))
                {
                    printf("Save\n");
                    break;
                }

                WCHAR c = input.Event.KeyEvent.uChar.UnicodeChar;
                switch (c)
                {
                case 13:
                {
                    CONSOLE_SCREEN_BUFFER_INFO consoleScreenBufferInfo;
                    if (!GetConsoleScreenBufferInfo(
                            consoleOutput, &consoleScreenBufferInfo))
                    {
                        abort();
                    }
                    const COORD zero = {0, 0};
                    DWORD written = 0;
                    if (!FillConsoleOutputCharacter(
                            consoleOutput, ' ',
                            consoleScreenBufferInfo.dwSize.X *
                                consoleScreenBufferInfo.dwSize.Y,
                            zero, &written))
                    {
                        abort();
                    }
                    if (!SetConsoleCursorPosition(consoleOutput, zero))
                    {
                        abort();
                    }
                    if (!SetConsoleTextAttribute(
                            consoleOutput, FOREGROUND_BLUE))
                    {
                        abort();
                    }
                    render(source);
                    if (!SetConsoleTextAttribute(
                            consoleOutput, FOREGROUND_BLUE | FOREGROUND_GREEN |
                                               FOREGROUND_RED))
                    {
                        abort();
                    }
                    break;
                }

                case 27:
                    return;
                }
            }
            break;
        }

        case MOUSE_EVENT:
            break;

        case WINDOW_BUFFER_SIZE_EVENT:
            break;

        case FOCUS_EVENT:
            break;

        default:
            printf("Unknown event\n");
            break;
        }
    }
}

int main(void)
{
    lambda main = {expression_allocate(expression_from_builtin(builtin_unit)),
                   expression_allocate(expression_from_utf8_string(
                       utf8_string_from_c_str("argument"))),
                   expression_allocate(expression_from_integer_literal(
                       integer_create(0, 123)))};
    expression root = expression_from_lambda(main);
    run_editor(&root);
    expression_free(&root);
    check_allocations();
    return 0;
}
