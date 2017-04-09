#include "lpg_integer.h"
#include "lpg_assert.h"

integer integer_create(uint64_t high, uint64_t low)
{
    integer result = {high, low};
    return result;
}

integer integer_shift_left_truncate(integer value, uint32_t bits)
{
    if (bits == 0)
    {
        return value;
    }
    if (bits == 64)
    {
        integer result = {value.low, 0};
        return result;
    }
    if (bits > 64)
    {
        value.high = value.low;
        value.low = 0;
        bits -= 64;
    }
    integer result = {((value.high << bits) | ((value.low >> (64u - bits)) &
                                               ((uint64_t)-1) >> (64u - bits))),
                      (value.low << bits)};
    return result;
}

int integer_shift_left(integer *value, uint32_t bits)
{
    if (bits >= 64)
    {
        if (value->high != 0)
        {
            return 0;
        }
        if ((bits > 64) && value->low & (UINT64_MAX << (128u - bits)))
        {
            return 0;
        }
    }
    if ((bits > 0) && (value->high & (UINT64_MAX << (64u - bits))))
    {
        return 0;
    }
    *value = integer_shift_left_truncate(*value, bits);
    return 1;
}

unsigned integer_bit(integer value, uint32_t bit)
{
    if (bit < 64u)
    {
        return (value.low >> bit) & 1u;
    }
    return (value.high >> (bit - 64u)) & 1u;
}

void integer_set_bit(integer *target, uint32_t bit, unsigned value)
{
    if (bit < 64u)
    {
        target->low |= ((uint64_t)value << bit);
    }
    else
    {
        target->high |= ((uint64_t)value << (bit - 64u));
    }
}

int integer_equal(integer left, integer right)
{
    return (left.high == right.high) && (left.low == right.low);
}

int integer_less(integer left, integer right)
{
    if (left.high < right.high)
    {
        return 1;
    }
    if (left.high > right.high)
    {
        return 0;
    }
    return (left.low < right.low);
}

integer integer_subtract(integer minuend, integer subtrahend)
{
    integer result = {0, 0};
    result.low = (minuend.low - subtrahend.low);
    if (result.low > minuend.low)
    {
        --minuend.high;
    }
    result.high = (minuend.high - subtrahend.high);
    return result;
}

int integer_multiply(integer *left, integer right)
{
    integer product = integer_create(0, 0);
    for (uint32_t i = 0; i < 128; ++i)
    {
        if (integer_bit(right, i))
        {
            integer summand = *left;
            if (!integer_shift_left(&summand, i))
            {
                return 0;
            }
            if (!integer_add(&product, summand))
            {
                return 0;
            }
        }
    }
    *left = product;
    return 1;
}

int integer_add(integer *left, integer right)
{
    uint64_t low = (left->low + right.low);
    uint64_t high = left->high;
    if (low < right.low)
    {
        if (high == UINT64_MAX)
        {
            return 0;
        }
        ++high;
    }
    high += right.high;
    if (high >= right.high)
    {
        left->high = high;
        left->low = low;
        return 1;
    }
    return 0;
}

int integer_parse(integer *into, unicode_view from)
{
    ASSUME(into);
    ASSUME(from.length >= 1);
    integer result = integer_create(0, 0);
    for (size_t i = 0; i < from.length; ++i)
    {
        if (!integer_multiply(&result, integer_create(0, 10)))
        {
            return 0;
        }
        if (!integer_add(&result, integer_create(0, (from.begin[i] - '0'))))
        {
            return 0;
        }
    }
    *into = result;
    return 1;
}

integer_division integer_divide(integer numerator, integer denominator)
{
    ASSERT(denominator.low || denominator.high);
    integer_division result = {{0, 0}, {0, 0}};
    for (unsigned i = 127; i < 128; --i)
    {
        result.remainder = integer_shift_left_truncate(result.remainder, 1);
        result.remainder.low |= integer_bit(numerator, i);
        if (!integer_less(result.remainder, denominator))
        {
            result.remainder = integer_subtract(result.remainder, denominator);
            integer_set_bit(&result.quotient, i, 1);
        }
    }
    return result;
}

char const lower_case_digits[] = "0123456789abcdef";

char *integer_format(integer const value, char const *digits, unsigned base,
                     char *buffer, size_t buffer_size)
{
    ASSUME(base >= 2);
    ASSUME(base <= 16);
    ASSUME(buffer_size > 0);
    size_t next_digit = buffer_size - 1;
    integer rest = value;
    for (;;)
    {
        integer_division const divided =
            integer_divide(rest, integer_create(0, base));
        buffer[next_digit] = digits[divided.remainder.low];
        rest = divided.quotient;
        if (rest.high == 0 && rest.low == 0)
        {
            break;
        }
        if (next_digit == 0)
        {
            return NULL;
        }
        --next_digit;
    }
    return buffer + next_digit;
}