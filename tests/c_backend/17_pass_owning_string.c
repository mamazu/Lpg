#include <lpg_std_unit.h>
#include <lpg_std_string.h>
#include <stdio.h>
#include <lpg_std_read.h>
static unit lambda_1(string_ref const arg0);
static unit lambda_1(string_ref const arg0)
{
    unit const r_3 = unit_impl;
    fwrite(arg0.data, 1, arg0.length, stdout);
    return r_3;
}
int main(void)
{
    string_ref const r_3 = read_impl();
    unit const r_4 = lambda_1(r_3);
    string_ref_free(&r_3);
    return 0;
}
