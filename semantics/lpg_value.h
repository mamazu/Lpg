#pragma once
#include "lpg_checked_function.h"
#include "lpg_function_id.h"
#include "lpg_garbage_collector.h"
#include "lpg_generic_enum_id.h"
#include "lpg_generic_interface_id.h"
#include "lpg_generic_lambda_id.h"
#include "lpg_generic_struct_id.h"
#include "lpg_integer.h"
#include "lpg_type.h"

typedef struct implementation_ref
{
    interface_id target;
    size_t implementation_index;
} implementation_ref;

implementation_ref implementation_ref_create(interface_id const target, size_t implementation_index);
bool implementation_ref_equals(implementation_ref const left, implementation_ref const right);
implementation_entry *implementation_ref_resolve(lpg_interface const *const interfaces, implementation_ref const ref);

typedef struct enumeration enumeration;

struct value;
struct interpreter;
struct optional_value;

typedef struct external_function_result external_function(struct value const *const captures, void *environment,
                                                          struct optional_value const self,
                                                          struct value *const arguments,
                                                          struct interpreter *const context);

typedef struct function_pointer_value
{
    function_id code;
    external_function *external;
    void *external_environment;
    struct value *captures;
    size_t capture_count;
    function_pointer external_signature;
} function_pointer_value;

function_pointer_value function_pointer_value_from_external(LPG_NON_NULL(external_function *external),
                                                            void *environment, struct value *const captures,
                                                            function_pointer const signature);
function_pointer_value function_pointer_value_from_internal(function_id const code, struct value *const captures,
                                                            size_t const capture_count);
bool function_pointer_value_equals(function_pointer_value const left, function_pointer_value const right);

typedef enum value_kind {
    value_kind_integer = 1,
    value_kind_string,
    value_kind_function_pointer,
    value_kind_structure,
    value_kind_type,
    value_kind_enum_element,
    value_kind_unit,
    value_kind_tuple,
    value_kind_enum_constructor,
    value_kind_type_erased,
    value_kind_pattern,
    value_kind_generic_enum,
    value_kind_generic_interface,
    value_kind_array,
    value_kind_generic_lambda,
    value_kind_generic_struct
} value_kind;

typedef struct value_tuple
{
    struct value *elements;
    size_t element_count;
} value_tuple;

value_tuple value_tuple_create(struct value *elements, size_t element_count);

typedef struct enum_element_value
{
    enum_element_id which;

    /*NULL means unit*/
    struct value *state;

    type state_type;
} enum_element_value;

enum_element_value enum_element_value_create(enum_element_id which, struct value *state, type state_type);

typedef struct type_erased_value
{
    implementation_ref impl;
    struct value *self;
} type_erased_value;

type_erased_value type_erased_value_create(implementation_ref impl, LPG_NON_NULL(struct value *self));

typedef struct structure_value
{
    struct value const *members;
    size_t count;
} structure_value;

structure_value structure_value_create(struct value const *members, size_t count);

typedef struct array_value
{
    struct value *elements;
    size_t count;
    size_t allocated;
    type element_type;
} array_value;

array_value array_value_create(struct value *elements, size_t count, size_t allocated, type element_type);
bool array_value_equals(array_value const left, array_value const right);

typedef struct value
{
    value_kind kind;
    union
    {
        structure_value structure;
        integer integer_;
        unicode_view string;
        function_pointer_value function_pointer;
        type type_;
        enum_element_value enum_element;
        value_tuple tuple_;
        type_erased_value type_erased;
        generic_enum_id generic_enum;
        generic_interface_id generic_interface;
        array_value *array;
        generic_lambda_id generic_lambda;
        generic_struct_id generic_struct;
    };
} value;

static inline value value_from_structure(structure_value const content)
{
    value result;
    result.kind = value_kind_structure;
    result.structure = content;
    return result;
}

value value_from_function_pointer(function_pointer_value pointer);
value value_from_string(unicode_view const string);

static inline value value_from_unit(void)
{
    value result;
    result.kind = value_kind_unit;
    /*dummy value to avoid compiler warning*/
    result.integer_ = integer_create(0, 0);
    return result;
}

value value_from_type(type const type_);
value value_from_enum_element(enum_element_id const element, type const state_type, value *const state);
value value_from_integer(integer const content);
value value_from_tuple(value_tuple content);
value value_from_enum_constructor(void);
value value_from_type_erased(type_erased_value content);
value value_from_generic_enum(generic_enum_id content);
value value_from_generic_interface(generic_interface_id content);
value value_from_array(array_value *content);
value value_from_generic_lambda(generic_lambda_id content);
value value_or_unit(value const *const maybe);
value value_from_generic_struct(generic_struct_id content);
value value_create_invalid(void);
bool value_equals(value const left, value const right);
bool value_less_than(value const left, value const right);
bool value_greater_than(value const left, value const right);
bool enum_less_than(enum_element_value const left, enum_element_value const right);

static inline bool value_is_valid(value const checked)
{
    return (checked.kind >= value_kind_integer) && (checked.kind <= value_kind_generic_struct);
}

bool value_is_mutable(value const original);

typedef struct optional_value
{
    bool is_set;
    value value_;
} optional_value;

static inline optional_value optional_value_create(value v)
{
    optional_value const result = {true, v};
    return result;
}

static optional_value const optional_value_empty = {false, {(value_kind)0, {{NULL, 0}}}};

bool value_conforms_to_type(value const instance, type const expected);

typedef enum external_function_result_code {
    external_function_result_success = 1,
    external_function_result_out_of_memory,
    external_function_result_unavailable,
    external_function_result_stack_overflow,
    external_function_result_instruction_limit_reached
} external_function_result_code;

typedef struct external_function_result
{
    external_function_result_code code;
    value if_success;
} external_function_result;

external_function_result external_function_result_from_success(value success);
external_function_result external_function_result_create_out_of_memory(void);
external_function_result external_function_result_create_unavailable(void);
external_function_result external_function_result_create_stack_overflow(void);
external_function_result external_function_result_create_instruction_limit_reached(void);
