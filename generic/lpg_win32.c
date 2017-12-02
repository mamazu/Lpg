#include "lpg_win32.h"
#include "lpg_allocate.h"
#include "lpg_assert.h"

#ifdef _WIN32
#include <Windows.h>

void win32_string_free(win32_string const freed)
{
    if (freed.c_str)
    {
        deallocate(freed.c_str);
    }
}

win32_string to_win32_path(unicode_view const original)
{
    ASSERT(original.length <= (size_t)INT_MAX);
    int const output_size = MultiByteToWideChar(CP_UTF8, 0, original.begin, (int)original.length, NULL, 0);
    ASSERT((original.length == 0) || (output_size != 0));
    win32_string const result = {allocate_array(((size_t)output_size) + 1, sizeof(*result.c_str)), (size_t)output_size};
    ASSERT(MultiByteToWideChar(CP_UTF8, 0, original.begin, (int)original.length, result.c_str, (int)result.size) ==
           output_size);
    for (int i = 0; i < output_size; ++i)
    {
        if (result.c_str[i] == L'/')
        {
            result.c_str[i] = L'\\';
        }
    }
    result.c_str[result.size] = L'\0';
    return result;
}
#endif