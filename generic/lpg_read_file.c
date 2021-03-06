#include "lpg_read_file.h"
#include "lpg_allocate.h"
#include <stdint.h>
#include <stdio.h>

blob_or_error make_blob_success(blob success)
{
    blob_or_error const result = {NULL, success};
    return result;
}

blob_or_error make_blob_error(char const *const error)
{
    blob_or_error const result = {error, {NULL, 0}};
    return result;
}

blob_or_error read_file(char const *const name)
{
#ifdef _WIN32
    FILE *source_file = NULL;
    fopen_s(&source_file, name, "rb");
#else
    FILE *const source_file = fopen(name, "rb");
#endif
    if (!source_file)
    {
        return make_blob_error("Could not open source file\n");
    }

    fseek(source_file, 0, SEEK_END);

#ifdef _WIN32
    long long const source_size = _ftelli64
#else
    off_t const source_size = ftello
#endif
        (source_file);
    if (source_size < 0)
    {
        fclose(source_file);
        return make_blob_error("Could not determine size of source file\n");
    }

#if (SIZE_MAX < UINT64_MAX)
    if ((uint64_t)source_size > SIZE_MAX)
    {
        fclose(source_file);
        return make_blob_error("Source file does not fit into memory\n");
    }
#endif

    fseek(source_file, 0, SEEK_SET);
    size_t const checked_source_size = (size_t)source_size;
    blob const source = {allocate(checked_source_size), checked_source_size};
    size_t const read_result = fread(source.data, 1, source.length, source_file);
    if (read_result != source.length)
    {
        fclose(source_file);
        blob_free(&source);
        return make_blob_error("Could not read from source file\n");
    }

    fclose(source_file);
    return make_blob_success(source);
}

blob_or_error read_file_unicode_view_name(unicode_view const name)
{
    unicode_string const zero_name = unicode_view_zero_terminate(name);
    blob_or_error const result = read_file(zero_name.data);
    unicode_string_free(&zero_name);
    return result;
}

size_t remove_carriage_returns(char *const from, size_t const length)
{
    size_t new_length = 0;
    for (size_t i = 0; i < length; ++i)
    {
        if (from[i] == '\r')
        {
            continue;
        }
        from[new_length] = from[i];
        ++new_length;
    }
    return new_length;
}
