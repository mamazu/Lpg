#include "lpg_allocate.h"
#include "lpg_assert.h"
#include "lpg_arithmetic.h"
#include <stdlib.h>
#include <assert.h>

static size_t active_allocations = 0;

void *allocate(size_t size)
{
    void *memory = malloc(size);
    ASSERT(memory);
    ++active_allocations;
    return memory;
}

void *allocate_array(size_t size, size_t element)
{
    void *memory = calloc(size, element);
    ASSERT(memory);
    ++active_allocations;
    return memory;
}

void *reallocate(void *memory, size_t new_size)
{
    int const was_null = (memory == NULL);
    void *const new_memory = realloc(memory, new_size);
    ASSERT(new_memory);
    if (was_null)
    {
        ++active_allocations;
    }
    return new_memory;
}

void *reallocate_array(void *memory, size_t new_size, size_t element)
{
    int const was_null = (memory == NULL);
    optional_size const total_size = size_multiply(new_size, element);
    ASSERT(total_size.state == optional_set);
    void *const new_memory = realloc(memory, total_size.value_if_set);
    ASSERT(new_memory);
    if (was_null)
    {
        ++active_allocations;
    }
    return new_memory;
}

void deallocate(void *memory)
{
    ASSERT(active_allocations > 0);
    --active_allocations;
    free(memory);
}

void check_allocations()
{
    ASSERT(active_allocations == 0);
}