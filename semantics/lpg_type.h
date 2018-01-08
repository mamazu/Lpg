#pragma once

#include "lpg_unicode_string.h"
#include "lpg_struct_member_id.h"
#include "lpg_enum_element_id.h"
#include "lpg_integer.h"
#include "lpg_garbage_collector.h"
#include "lpg_function_id.h"
#include "lpg_interface_id.h"
#include "lpg_struct_id.h"
#include "lpg_integer_range.h"
#include "lpg_enum_id.h"

typedef struct structure_member structure_member;

typedef struct structure
{
    structure_member *members;
    struct_member_id count;
} structure;

structure structure_create(structure_member *members, struct_member_id count);
void structure_free(structure const *value);

typedef struct type type;

typedef struct enumeration_element enumeration_element;

typedef struct enumeration
{
    enumeration_element *elements;
    enum_element_id size;
} enumeration;

enumeration enumeration_create(enumeration_element *elements, enum_element_id size);
void enumeration_free(LPG_NON_NULL(enumeration const *value));
bool has_stateful_element(enumeration const enum_);

typedef struct tuple_type
{
    type *elements;
    size_t length;
} tuple_type;

tuple_type tuple_type_create(type *elements, size_t length);
bool tuple_type_equals(tuple_type const left, tuple_type const right);

typedef struct method_description method_description;
typedef struct implementation_entry implementation_entry;

typedef struct interface
{
    method_description *methods;
    function_id method_count;
    implementation_entry *implementations;
    size_t implementation_count;
} interface;

interface interface_create(method_description *methods, function_id method_count, implementation_entry *implementations,
                           size_t implementation_count);
void interface_free(interface const value);

typedef enum type_kind
{
    type_kind_structure,
    type_kind_function_pointer,
    type_kind_unit,
    type_kind_string_ref,
    type_kind_enumeration,
    type_kind_tuple,
    type_kind_type,
    type_kind_integer_range,
    type_kind_enum_constructor,
    type_kind_lambda,
    type_kind_interface,
    type_kind_method_pointer,
    type_kind_generic_enum
} type_kind;

typedef struct function_pointer function_pointer;

typedef struct enum_constructor_type enum_constructor_type;

typedef struct lambda_type
{
    function_id lambda;
} lambda_type;

lambda_type lambda_type_create(function_id const lambda);

typedef struct method_pointer_type
{
    interface_id interface_;
    size_t method_index;
} method_pointer_type;

method_pointer_type method_pointer_type_create(interface_id interface_, size_t method_index);
bool method_pointer_type_equals(method_pointer_type const left, method_pointer_type const right);

struct type
{
    type_kind kind;
    union
    {
        struct_id structure_;
        function_pointer const *function_pointer_;
        enum_id enum_;
        tuple_type tuple_;
        integer_range integer_range_;
        size_t inferred;
        enum_constructor_type *enum_constructor;
        lambda_type lambda;
        interface_id interface_;
        method_pointer_type method_pointer;
    };
};

typedef struct optional_type
{
    bool is_set;
    type value;
} optional_type;

optional_type optional_type_create_set(type const value);
optional_type optional_type_create_empty(void);
bool optional_type_equals(optional_type const left, optional_type const right);

struct method_description
{
    unicode_string name;
    tuple_type parameters;
    type result;
};

method_description method_description_create(unicode_string name, tuple_type parameters, type result);
void method_description_free(method_description const value);

struct enum_constructor_type
{
    enum_id enumeration;
    enum_element_id which;
};

enum_constructor_type enum_constructor_type_create(enum_id enumeration, enum_element_id which);
bool enum_constructor_type_equals(enum_constructor_type const left, enum_constructor_type const right);

typedef struct implementation
{
    struct function_pointer_value *methods;
    size_t method_count;
} implementation;

implementation implementation_create(struct function_pointer_value *methods, size_t method_count);
void implementation_free(implementation const value);

struct implementation_entry
{
    type self;
    implementation target;
};

implementation_entry implementation_entry_create(type self, implementation target);
void implementation_entry_free(implementation_entry const value);

struct enumeration_element
{
    unicode_string name;
    type state;
};

enumeration_element enumeration_element_create(unicode_string name, type state);
void enumeration_element_free(LPG_NON_NULL(enumeration_element const *value));

struct function_pointer
{
    type result;
    tuple_type parameters;
    tuple_type captures;
    optional_type self;
};

function_pointer function_pointer_create(type result, tuple_type parameters, tuple_type captures, optional_type self);
bool function_pointer_equals(function_pointer const left, function_pointer const right);

type type_from_function_pointer(function_pointer const *value);
type type_from_unit(void);
type type_from_string_ref(void);
type type_from_enumeration(enum_id const value);
type type_from_tuple_type(tuple_type const value);
type type_from_type(void);
type type_from_integer_range(integer_range value);
type type_from_enum_constructor(LPG_NON_NULL(enum_constructor_type *enum_constructor));
type type_from_lambda(lambda_type const lambda);
type type_from_interface(interface_id const value);
type type_from_method_pointer(method_pointer_type const value);
type type_from_struct(struct_id const value);
type type_from_generic_enum(void);
type *type_allocate(type const value);
bool type_equals(type const left, type const right);
type type_clone(type const original, garbage_collector *const clone_gc);
optional_type optional_type_clone(optional_type const original, garbage_collector *const clone_gc);

void function_pointer_free(LPG_NON_NULL(function_pointer const *value));

bool is_implicitly_convertible(type const flat_from, type const flat_into);
