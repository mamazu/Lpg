#include "lpg_allocate.h"
#include "lpg_arithmetic.h"
#include "lpg_assert.h"
#include "lpg_atomic.h"
#include <string.h>

static size_t total_allocations = 0;
static size_t active_allocations = 0;

void *allocate(size_t size)
{
    void *memory = malloc(size);
    ASSERT(memory);
    atomic_size_increment(&active_allocations);
    atomic_size_increment(&total_allocations);
    return memory;
}

void *allocate_array(size_t size, size_t element)
{
    void *memory = calloc(size, element);
    ASSERT(memory);
    atomic_size_increment(&active_allocations);
    atomic_size_increment(&total_allocations);
    return memory;
}

void *reallocate(void *memory, size_t new_size)
{
    int const was_null = (memory == NULL);
    void *const new_memory = realloc(memory, new_size);
    ASSERT(new_memory);
    if (was_null)
    {
        atomic_size_increment(&active_allocations);
    }
    atomic_size_increment(&total_allocations);
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
        atomic_size_increment(&active_allocations);
    }
    atomic_size_increment(&total_allocations);
    return new_memory;
}

void deallocate(void *memory)
{
    ASSERT(memory != NULL);
    ASSERT(atomic_size_load(&active_allocations) > 0);
    atomic_size_decrement(&active_allocations);
    free(memory);
}

size_t count_total_allocations(void)
{
    return atomic_size_load(&total_allocations);
}

size_t count_active_allocations(void)
{
    return atomic_size_load(&active_allocations);
}

void *copy_array_impl(void const *from, size_t size_in_bytes)
{
    void *result = allocate(size_in_bytes);
    memcpy(result, from, size_in_bytes);
    return result;
}
