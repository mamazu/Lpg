#include "test_interpreter.h"
#include "test.h"
#include "lpg_interprete.h"
#include "lpg_stream_writer.h"
#include "lpg_assert.h"
#include "handle_parse_error.h"
#include "lpg_find_next_token.h"
#include "lpg_check.h"
#include "lpg_structure_member.h"
#include "lpg_standard_library.h"
#include <string.h>

static sequence parse(char const *input)
{
    test_parser_user user = {
        {input, strlen(input), source_location_create(0, 0)}, NULL, 0};
    expression_parser parser =
        expression_parser_create(find_next_token, handle_error, &user);
    sequence const result = parse_program(&parser);
    REQUIRE(user.base.remaining_size == 0);
    return result;
}

static void expect_no_errors(semantic_error const error, void *user)
{
    (void)error;
    (void)user;
    FAIL();
}

typedef struct test_environment
{
    stream_writer print_destination;
    unicode_view read_input;
} test_environment;

static value print(value const *const inferred, value const *const arguments,
                   garbage_collector *const gc, void *environment)
{
    (void)inferred;
    (void)gc;
    unicode_view const text = arguments[0].string_ref;
    test_environment *const actual_environment = environment;
    stream_writer *destination = &actual_environment->print_destination;
    REQUIRE(stream_writer_write_bytes(*destination, text.begin, text.length) ==
            success);
    return value_from_unit();
}

static value assert_impl(value const *const inferred, value const *arguments,
                         garbage_collector *const gc, void *environment)
{
    (void)environment;
    (void)inferred;
    (void)gc;
    enum_element_id const argument = arguments[0].enum_element;
    REQUIRE(argument == 1);
    return value_from_unit();
}

static value read_impl(value const *const inferred, value const *arguments,
                       garbage_collector *const gc, void *environment)
{
    (void)inferred;
    (void)gc;
    (void)arguments;
    test_environment *const actual_environment = environment;
    unicode_view result = actual_environment->read_input;
    actual_environment->read_input = unicode_view_create(NULL, 0);
    return value_from_string_ref(result);
}

static void expect_output(char const *source, char const *input,
                          char const *output, structure const global_object)
{
    memory_writer print_buffer = {NULL, 0, 0};
    test_environment environment = {
        memory_writer_erase(&print_buffer), unicode_view_from_c_str(input)};
    garbage_collector gc = {NULL};
    value const globals_values[11] = {
        /*type*/ value_from_unit(),
        /*g*/ value_from_unit(),
        /*print*/ value_from_function_pointer(
            function_pointer_value_from_external(print, &environment)),
        /*boolean*/ value_from_unit(),
        /*assert*/ value_from_function_pointer(
            function_pointer_value_from_external(assert_impl, NULL)),
        /*and*/ value_from_function_pointer(
            function_pointer_value_from_external(and_impl, NULL)),
        /*or*/ value_from_function_pointer(
            function_pointer_value_from_external(or_impl, NULL)),
        /*not*/ value_from_function_pointer(
            function_pointer_value_from_external(not_impl, NULL)),
        /*concat*/ value_from_function_pointer(
            function_pointer_value_from_external(concat_impl, NULL)),
        /*string-equals*/ value_from_function_pointer(
            function_pointer_value_from_external(string_equals_impl, NULL)),
        /*read*/ value_from_function_pointer(
            function_pointer_value_from_external(read_impl, &environment))};
    sequence root = parse(source);
    checked_program checked =
        check(root, global_object, expect_no_errors, NULL);
    sequence_free(&root);
    interprete(checked, globals_values, &gc);
    REQUIRE(memory_writer_equals(print_buffer, output));
    memory_writer_free(&print_buffer);
    checked_program_free(&checked);
    garbage_collector_free(gc);
}

void test_interpreter(void)
{
    standard_library_description const std_library =
        describe_standard_library();

    expect_output("", "", "", std_library.globals);
    expect_output("print(\"\")", "", "", std_library.globals);
    expect_output(
        "print(\"Hello, world!\")", "", "Hello, world!", std_library.globals);
    expect_output(
        "print(\"Hello, world!\")\n", "", "Hello, world!", std_library.globals);
    expect_output("let v = \"Hello, world!\"\nprint(v)\n", "", "Hello, world!",
                  std_library.globals);
    expect_output("print(\"Hello, world!\")\r\n", "", "Hello, world!",
                  std_library.globals);
    expect_output("print(\"Hello, \")\nprint(\"world!\")", "", "Hello, world!",
                  std_library.globals);
    expect_output("loop\n"
                  "    let v = \"Hello, world!\"\n"
                  "    print(v)\n"
                  "    break",
                  "", "Hello, world!", std_library.globals);
    expect_output("assert(boolean.true)", "", "", std_library.globals);
    expect_output("assert(not(boolean.false))", "", "", std_library.globals);
    expect_output(
        "assert(and(boolean.true, boolean.true))", "", "", std_library.globals);
    expect_output(
        "assert(or(boolean.true, boolean.true))", "", "", std_library.globals);
    expect_output(
        "assert(or(boolean.false, boolean.true))", "", "", std_library.globals);
    expect_output(
        "assert(or(boolean.true, boolean.false))", "", "", std_library.globals);
    expect_output(
        "let v = boolean.true\nassert(v)", "", "", std_library.globals);
    expect_output(
        "let v = boolean.false\nassert(not(v))", "", "", std_library.globals);
    expect_output(
        "let v = not(boolean.false)\nassert(v)", "", "", std_library.globals);
    expect_output("let v = 123\n", "", "", std_library.globals);
    expect_output("let s = concat(\"123\", \"456\")\nprint(s)\n", "", "123456",
                  std_library.globals);
    expect_output(
        "assert(string-equals(\"\", \"\"))\n", "", "", std_library.globals);
    expect_output("assert(string-equals(\"aaa\", \"aaa\"))\n", "", "",
                  std_library.globals);
    expect_output("assert(string-equals(concat(\"aa\", \"a\"), \"aaa\"))\n", "",
                  "", std_library.globals);
    expect_output("assert(string-equals(concat(\"aa\", read()), \"aaa\"))\n",
                  "a", "", std_library.globals);
    expect_output("assert(not(string-equals(\"a\", \"\")))\n", "", "",
                  std_library.globals);
    expect_output("assert(not(string-equals(\"a\", \"b\")))\n", "", "",
                  std_library.globals);
    expect_output(
        "assert(string-equals(read(), \"\"))\n", "", "", std_library.globals);
    expect_output("assert(string-equals(read(), \"aaa\"))\n", "aaa", "",
                  std_library.globals);
    expect_output("let f = () boolean.true\n"
                  "assert(f())\n",
                  "", "", std_library.globals);
    expect_output("let f = () print(\"hello\")\n"
                  "f()\n",
                  "", "hello", std_library.globals);
    expect_output("let xor = (a: boolean, b: boolean)\n"
                  "    or(and(a, not(b)), and(not(a), b))\n"
                  "assert(xor(boolean.true, boolean.false))\n"
                  "assert(xor(boolean.false, boolean.true))\n"
                  "assert(not(xor(boolean.true, boolean.true)))\n"
                  "assert(not(xor(boolean.false, boolean.false)))\n",
                  "", "", std_library.globals);

    standard_library_description_free(&std_library);
}