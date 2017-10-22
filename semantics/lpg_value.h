#pragma once
#include "lpg_checked_function.h"
#include "lpg_integer.h"
#include "lpg_type.h"
#include "lpg_garbage_collector.h"
#include "lpg_function_id.h"

typedef struct implementation_ref
{
    interface_id target;
    size_t implementation_index;
} implementation_ref;

implementation_ref implementation_ref_create(interface_id const target, size_t implementation_index);
bool implementation_ref_equals(implementation_ref const left, implementation_ref const right);
implementation *implementation_ref_resolve(interface const *const interfaces, implementation_ref const ref);

typedef struct enumeration enumeration;

typedef struct function_call_arguments
{
    struct value const *const inferred;
    struct value *const arguments;
    struct value const *globals;
    garbage_collector *const gc;
    checked_function const *const all_functions;
    interface const *all_interfaces;
} function_call_arguments;

function_call_arguments function_call_arguments_create(struct value const *const inferred,
                                                       struct value *const arguments, struct value const *globals,
                                                       LPG_NON_NULL(garbage_collector *const gc),
                                                       LPG_NON_NULL(checked_function const *const all_functions),
                                                       LPG_NON_NULL(interface const *all_interfaces));

typedef struct value external_function(function_call_arguments const arguments, struct value const *const captures,
                                       void *environment);

typedef struct function_pointer_value
{
    function_id code;
    external_function *external;
    void *external_environment;
    struct value *captures;
    size_t capture_count;
} function_pointer_value;

function_pointer_value function_pointer_value_from_external(LPG_NON_NULL(external_function *external),
                                                            void *environment, struct value *const captures,
                                                            size_t const capture_count);
function_pointer_value function_pointer_value_from_internal(function_id const code, struct value *const captures,
                                                            size_t const capture_count);
bool function_pointer_value_equals(function_pointer_value const left, function_pointer_value const right);

typedef enum value_kind
{
    value_kind_integer,
    value_kind_string,
    value_kind_function_pointer,
    value_kind_flat_object,
    value_kind_type,
    value_kind_enum_element,
    value_kind_unit,
    value_kind_tuple,
    value_kind_enum_constructor,
    value_kind_type_erased
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
} enum_element_value;

typedef struct type_erased_value
{
    implementation_ref impl;
    struct value *self;
} type_erased_value;

type_erased_value type_erased_value_create(implementation_ref impl, LPG_NON_NULL(struct value *self));

typedef struct value
{
    value_kind kind;
    union
    {
        struct value const *flat_object;
        integer integer_;
        unicode_view string_ref;
        function_pointer_value function_pointer;
        type type_;
        enum_element_value enum_element;
        value_tuple tuple_;
        type_erased_value type_erased;
    };
} value;

value value_from_flat_object(LPG_NON_NULL(value const *flat_object));
value value_from_function_pointer(function_pointer_value function_pointer);
value value_from_string_ref(unicode_view const string_ref);
value value_from_unit(void);
value value_from_type(type const type_);
value value_from_enum_element(enum_element_id const element, value *const state);
value value_from_integer(integer const content);
value value_from_tuple(value_tuple content);
value value_from_enum_constructor(void);
value value_from_type_erased(type_erased_value content);
bool value_equals(value const left, value const right);
bool value_less_than(value const left, value const right);
bool value_greater_than(value const left, value const right);
bool enum_less_than(enum_element_value const left, enum_element_value const right);

typedef struct optional_value
{
    bool is_set;
    value value_;
} optional_value;

optional_value optional_value_create(value v);

static optional_value const optional_value_empty = {false, {value_kind_integer, {NULL}}};
