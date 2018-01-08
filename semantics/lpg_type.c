#include "lpg_type.h"
#include "lpg_for.h"
#include "lpg_allocate.h"
#include "lpg_structure_member.h"
#include "lpg_assert.h"
#include "lpg_expression.h"

structure structure_create(structure_member *members, struct_member_id count)
{
    structure const result = {members, count};
    return result;
}

void structure_free(structure const *value)
{
    LPG_FOR(struct_member_id, i, value->count)
    {
        struct_member_free(value->members + i);
    }
    if (value->members)
    {
        deallocate(value->members);
    }
}

enumeration enumeration_create(enumeration_element *elements, enum_element_id size)
{
    enumeration const result = {elements, size};
    return result;
}

void enumeration_free(enumeration const *value)
{
    LPG_FOR(size_t, i, value->size)
    {
        enumeration_element_free(value->elements + i);
    }
    if (value->elements)
    {
        deallocate(value->elements);
    }
}

bool has_stateful_element(enumeration const enum_)
{
    for (size_t i = 0; i < enum_.size; ++i)
    {
        if (enum_.elements[i].state.kind != type_kind_unit)
        {
            return true;
        }
    }
    return false;
}

tuple_type tuple_type_create(type *elements, size_t length)
{
    tuple_type const result = {elements, length};
    return result;
}

bool tuple_type_equals(tuple_type const left, tuple_type const right)
{
    if (left.length != right.length)
    {
        return false;
    }
    for (size_t i = 0; i < left.length; ++i)
    {
        if (!type_equals(left.elements[i], right.elements[i]))
        {
            return false;
        }
    }
    return true;
}

interface interface_create(method_description *methods, function_id method_count, implementation_entry *implementations,
                           size_t implementation_count)
{
    interface const result = {methods, method_count, implementations, implementation_count};
    return result;
}

void interface_free(interface const value)
{
    for (function_id i = 0; i < value.method_count; ++i)
    {
        method_description_free(value.methods[i]);
    }
    if (value.methods)
    {
        deallocate(value.methods);
    }
    for (size_t i = 0; i < value.implementation_count; ++i)
    {
        implementation_entry_free(value.implementations[i]);
    }
    if (value.implementations)
    {
        deallocate(value.implementations);
    }
}

lambda_type lambda_type_create(function_id const lambda)
{
    lambda_type const result = {lambda};
    return result;
}

method_pointer_type method_pointer_type_create(interface_id interface_, size_t method_index)
{
    method_pointer_type const result = {interface_, method_index};
    return result;
}

bool method_pointer_type_equals(method_pointer_type const left, method_pointer_type const right)
{
    return (left.interface_ == right.interface_) && (left.method_index == right.method_index);
}

optional_type optional_type_create_set(type const value)
{
    optional_type const result = {true, value};
    return result;
}

optional_type optional_type_create_empty(void)
{
    optional_type const result = {false, type_from_unit()};
    return result;
}

bool optional_type_equals(optional_type const left, optional_type const right)
{
    if (left.is_set && right.is_set)
    {
        return type_equals(left.value, right.value);
    }
    return (left.is_set == right.is_set);
}

method_description method_description_create(unicode_string name, tuple_type parameters, type result)
{
    method_description const returning = {name, parameters, result};
    return returning;
}

void method_description_free(method_description const value)
{
    unicode_string_free(&value.name);
    if (value.parameters.elements)
    {
        deallocate(value.parameters.elements);
    }
}

enum_constructor_type enum_constructor_type_create(enum_id enumeration, enum_element_id which)
{
    enum_constructor_type const result = {enumeration, which};
    return result;
}

bool enum_constructor_type_equals(enum_constructor_type const left, enum_constructor_type const right)
{
    return (left.enumeration == right.enumeration) && (left.which == right.which);
}

implementation implementation_create(struct function_pointer_value *methods, size_t method_count)
{
    implementation const result = {methods, method_count};
    return result;
}

void implementation_free(implementation const value)
{
    if (value.methods)
    {
        deallocate(value.methods);
    }
}

implementation_entry implementation_entry_create(type self, implementation target)
{
    implementation_entry const result = {self, target};
    return result;
}

void implementation_entry_free(implementation_entry const value)
{
    implementation_free(value.target);
}

enumeration_element enumeration_element_create(unicode_string name, type state)
{
    enumeration_element const result = {name, state};
    return result;
}

void enumeration_element_free(enumeration_element const *value)
{
    unicode_string_free(&value->name);
}

function_pointer function_pointer_create(type result, tuple_type parameters, tuple_type captures, optional_type self)
{
    function_pointer const returning = {result, parameters, captures, self};
    return returning;
}

bool function_pointer_equals(function_pointer const left, function_pointer const right)
{
    if (!type_equals(left.result, right.result))
    {
        return false;
    }
    return tuple_type_equals(left.parameters, right.parameters) && tuple_type_equals(left.captures, right.captures) &&
           optional_type_equals(left.self, right.self);
}

type type_from_function_pointer(function_pointer const *value)
{
    type result;
    result.kind = type_kind_function_pointer;
    result.function_pointer_ = value;
    return result;
}

type type_from_unit(void)
{
    type result;
    result.kind = type_kind_unit;
    return result;
}

type type_from_string_ref(void)
{
    type result;
    result.kind = type_kind_string_ref;
    return result;
}

type type_from_enumeration(enum_id const value)
{
    type result;
    result.kind = type_kind_enumeration;
    result.enum_ = value;
    return result;
}

type type_from_tuple_type(tuple_type const value)
{
    type result;
    result.kind = type_kind_tuple;
    result.tuple_ = value;
    return result;
}

type type_from_type(void)
{
    type result;
    result.kind = type_kind_type;
    return result;
}

type type_from_integer_range(integer_range value)
{
    type result;
    result.kind = type_kind_integer_range;
    result.integer_range_ = value;
    return result;
}

type type_from_enum_constructor(LPG_NON_NULL(enum_constructor_type *enum_constructor))
{
    type result;
    result.kind = type_kind_enum_constructor;
    result.enum_constructor = enum_constructor;
    return result;
}

type type_from_lambda(lambda_type const lambda)
{
    type result;
    result.kind = type_kind_lambda;
    result.lambda = lambda;
    return result;
}

type type_from_interface(interface_id const value)
{
    type result;
    result.kind = type_kind_interface;
    result.interface_ = value;
    return result;
}

type type_from_method_pointer(method_pointer_type const value)
{
    type result;
    result.kind = type_kind_method_pointer;
    result.method_pointer = value;
    return result;
}

type type_from_struct(struct_id const value)
{
    type result;
    result.kind = type_kind_structure;
    result.structure_ = value;
    return result;
}

type type_from_generic_enum()
{
    type result;
    result.kind = type_kind_generic_enum;
    return result;
}

type *type_allocate(type const value)
{
    type *const result = allocate(sizeof(*result));
    *result = value;
    return result;
}

bool type_equals(type const left, type const right)
{
    if (left.kind != right.kind)
    {
        return false;
    }
    switch (left.kind)
    {
    case type_kind_method_pointer:
        return method_pointer_type_equals(left.method_pointer, right.method_pointer);

    case type_kind_interface:
        return (left.interface_ == right.interface_);

    case type_kind_lambda:
        return (left.lambda.lambda == right.lambda.lambda);

    case type_kind_structure:
        return (left.structure_ == right.structure_);

    case type_kind_enum_constructor:
    case type_kind_generic_enum:
        LPG_TO_DO();

    case type_kind_function_pointer:
        return function_pointer_equals(*left.function_pointer_, *right.function_pointer_);

    case type_kind_unit:
    case type_kind_string_ref:
    case type_kind_type:
        return true;

    case type_kind_enumeration:
        return (left.enum_ == right.enum_);

    case type_kind_tuple:
        return tuple_type_equals(left.tuple_, right.tuple_);

    case type_kind_integer_range:
        return integer_range_equals(left.integer_range_, right.integer_range_);
    }
    LPG_UNREACHABLE();
}

type type_clone(type const original, garbage_collector *const clone_gc)
{
    switch (original.kind)
    {
    case type_kind_lambda:
        return type_from_lambda(original.lambda);

    case type_kind_function_pointer:
    {
        function_pointer *const copy = garbage_collector_allocate(clone_gc, sizeof(*copy));
        type *const arguments = garbage_collector_allocate_array(
            clone_gc, original.function_pointer_->parameters.length, sizeof(*arguments));
        for (size_t i = 0; i < original.function_pointer_->parameters.length; ++i)
        {
            arguments[i] = type_clone(original.function_pointer_->parameters.elements[i], clone_gc);
        }
        *copy = function_pointer_create(type_clone(original.function_pointer_->result, clone_gc),
                                        tuple_type_create(arguments, original.function_pointer_->parameters.length),
                                        tuple_type_create(NULL, 0),
                                        optional_type_clone(original.function_pointer_->self, clone_gc));
        return type_from_function_pointer(copy);
    }

    case type_kind_unit:
    case type_kind_string_ref:
    case type_kind_enumeration:
    case type_kind_interface:
    case type_kind_integer_range:
        return original;

    case type_kind_tuple:
    {
        type *const elements = garbage_collector_allocate_array(clone_gc, original.tuple_.length, sizeof(*elements));
        for (size_t i = 0; i < original.tuple_.length; ++i)
        {
            elements[i] = type_clone(original.tuple_.elements[i], clone_gc);
        }
        return type_from_tuple_type(tuple_type_create(elements, original.tuple_.length));
    }

    case type_kind_type:
        return type_from_type();

    case type_kind_enum_constructor:
    {
        enum_constructor_type *const enum_constructor = garbage_collector_allocate(clone_gc, sizeof(*enum_constructor));
        *enum_constructor =
            enum_constructor_type_create(original.enum_constructor->enumeration, original.enum_constructor->which);
        return type_from_enum_constructor(enum_constructor);
    }

    case type_kind_structure:
        return type_from_struct(original.structure_);

    case type_kind_method_pointer:
        LPG_TO_DO();

    case type_kind_generic_enum:
        return type_from_generic_enum();
    }
    LPG_UNREACHABLE();
}

optional_type optional_type_clone(optional_type const original, garbage_collector *const clone_gc)
{
    if (!original.is_set)
    {
        return original;
    }
    return optional_type_create_set(type_clone(original.value, clone_gc));
}

void function_pointer_free(function_pointer const *value)
{
    if (value->parameters.elements)
    {
        deallocate(value->parameters.elements);
    }
    if (value->captures.elements)
    {
        deallocate(value->captures.elements);
    }
}

bool is_implicitly_convertible(type const flat_from, type const flat_into)
{
    if (flat_from.kind != flat_into.kind)
    {
        return false;
    }
    switch (flat_from.kind)
    {
    case type_kind_type:
    case type_kind_unit:
    case type_kind_string_ref:
    case type_kind_generic_enum:
        return true;

    case type_kind_function_pointer:
        return type_equals(flat_from, flat_into);

    case type_kind_enumeration:
        return flat_from.enum_ == flat_into.enum_;

    case type_kind_tuple:
    {
        if (flat_from.tuple_.length != flat_into.tuple_.length)
        {
            return false;
        }
        for (size_t i = 0; i < flat_from.tuple_.length; ++i)
        {
            if (!is_implicitly_convertible(flat_from.tuple_.elements[i], flat_into.tuple_.elements[i]))
            {
                return false;
            }
        }
        return true;
    }

    case type_kind_integer_range:
        return integer_range_contains(flat_into.integer_range_, flat_from.integer_range_);

    case type_kind_method_pointer:
    case type_kind_lambda:
    case type_kind_structure:
    case type_kind_enum_constructor:
    case type_kind_interface:
        LPG_TO_DO();
    }
    LPG_UNREACHABLE();
}
