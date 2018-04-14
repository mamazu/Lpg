#include "lpg_program_check.h"
#include "lpg_allocate.h"

void program_check_free(program_check const freed)
{
    for (size_t i = 0; i < freed.generic_enum_count; ++i)
    {
        generic_enum_free(freed.generic_enums[i]);
    }
    if (freed.generic_enums)
    {
        deallocate(freed.generic_enums);
    }
    for (size_t i = 0; i < freed.enum_instantiation_count; ++i)
    {
        generic_enum_instantiation_free(freed.enum_instantiations[i]);
    }
    if (freed.enum_instantiations)
    {
        deallocate(freed.enum_instantiations);
    }
    for (size_t i = 0; i < freed.module_count; ++i)
    {
        module_free(freed.modules[i]);
    }
    if (freed.modules)
    {
        deallocate(freed.modules);
    }
}

void add_module(program_check *to, module added)
{
    to->modules = reallocate_array(to->modules, to->module_count + 1, sizeof(*to->modules));
    to->modules[to->module_count] = added;
    ++(to->module_count);
}
