#include "test_fuzz.h"
#include "fuzz_target.h"
#include "lpg_array_size.h"
#include "lpg_list_directory.h"
#include "lpg_path.h"
#include "lpg_read_file.h"
#include "test.h"

static unicode_string find_fuzz_corpus_directory(void)
{
    unicode_view const pieces[] = {path_remove_leaf(path_remove_leaf(unicode_view_from_c_str(__FILE__))),
                                   unicode_view_from_c_str("fuzz"), unicode_view_from_c_str("corpus")};
    return path_combine(pieces, LPG_ARRAY_SIZE(pieces));
}

void test_fuzz(void)
{
    unicode_string const corpus = find_fuzz_corpus_directory();
    directory_listing const corpus_entries = list_directory(unicode_view_from_string(corpus));
    for (size_t i = 0; i < corpus_entries.count; ++i)
    {
        directory_entry const entry = corpus_entries.entries[i];
        unicode_view const full_path_pieces[] = {
            unicode_view_from_string(corpus), unicode_view_from_string(entry.name)};
        unicode_string const full_path = path_combine(full_path_pieces, LPG_ARRAY_SIZE(full_path_pieces));
        blob_or_error const read_result = read_file_unicode_view_name(unicode_view_from_string(full_path));
        REQUIRE(!read_result.error);
        ParserTypeCheckerFuzzTarget((uint8_t const *)read_result.success.data, read_result.success.length);
        blob_free(&read_result.success);
        unicode_string_free(&full_path);
    }
    directory_listing_free(corpus_entries);
    unicode_string_free(&corpus);
}
