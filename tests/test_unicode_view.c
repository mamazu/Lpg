#include "test_unicode_view.h"
#include "lpg_unicode_view.h"
#include "test.h"
#include <string.h>

void test_unicode_view_finder()
{
    unicode_view const view = unicode_view_from_c_str("abc");
    {
        optional_size position = unicode_view_find(view, 'a');
        REQUIRE(position.state == optional_set);
        REQUIRE(position.value_if_set == 0);
    }
    {
        optional_size position = unicode_view_find(view, 'c');
        REQUIRE(position.state == optional_set);
        REQUIRE(position.value_if_set == 2);
    }
    {
        optional_size position = unicode_view_find(view, '0');
        REQUIRE(position.state == optional_empty);
    }
}

void test_unicode_view(void)
{
    {
        unicode_string const str = unicode_string_from_c_str("test");
        unicode_view const view = unicode_view_from_string(str);
        REQUIRE(unicode_view_equals_c_str(view, "test"));
        REQUIRE(!unicode_view_equals_c_str(view, "test2"));
        REQUIRE(!unicode_view_equals_c_str(view, "tes"));
        REQUIRE(!unicode_view_equals_c_str(view, ""));
        REQUIRE(!unicode_view_equals_c_str(view, "??????????"));
        unicode_string const copy = unicode_view_copy(view);
        REQUIRE(4 == copy.length);
        REQUIRE(!memcmp(copy.data, str.data, str.length * sizeof(*str.data)));
        unicode_string_free(&str);
        unicode_string_free(&copy);
    }
    {
        unicode_view const first = unicode_view_create(NULL, 0);
        char const a = 'a';
        unicode_view const second = unicode_view_create(&a, 1);
        REQUIRE(!unicode_view_equals(first, second));
        REQUIRE(unicode_view_equals(first, first));
        REQUIRE(unicode_view_equals(second, second));
    }
    {
        unicode_view const view = unicode_view_from_c_str("abc");
        REQUIRE(view.length == 3);
        REQUIRE(unicode_view_equals_c_str(view, "abc"));
    }
    {
        unicode_view const a = unicode_view_from_c_str("");
        unicode_view const b = unicode_view_from_c_str("b");
        REQUIRE(unicode_view_less(a, b));
        REQUIRE(!unicode_view_less(a, a));
        REQUIRE(!unicode_view_less(b, a));
    }
    {
        unicode_view const a = unicode_view_from_c_str("a");
        unicode_view const b = unicode_view_from_c_str("b");
        REQUIRE(unicode_view_less(a, b));
        REQUIRE(!unicode_view_less(a, a));
        REQUIRE(!unicode_view_less(b, a));
    }
    test_unicode_view_finder();
}
