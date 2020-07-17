#include "bool.h"
#include <stdio.h>
#include <limits.h>
#include <criterion/criterion.h>
#include <inttypes.h>


Test(unit, creat_bb) { // x^y = (!x | !y)&(x | y)

    for (uint64_t i = 0; i < SHRT_MAX; i++)
    {
        struct bigbool* x = uint64_from_bool(i);
        cr_assert_neq(x, NULL);
        for (uint64_t j = 0; j < SHRT_MAX; j++)
        {
            struct bigbool* y = uint64_from_bool(i);
            cr_assert_neq(y, NULL);

            struct bigbool* x_y = operation_bb(x, XOR, y);
            cr_assert_neq(x_y, NULL);

            struct bigbool* _x = invertbb(x);
            cr_assert_neq(_x, NULL);

            struct bigbool* _y = invertbb(x);
            cr_assert_neq(_y, NULL);

            struct bigbool* _x_or__y = operation_bb(_x, OR, _y);
            cr_assert_neq(_x_or__y, NULL);

            struct bigbool* x_or_y = operation_bb(x, OR, y);
            cr_assert_neq(x_or_y, NULL);

            struct bigbool* res = operation_bb(_x_or__y, AND, x_or_y);
            cr_assert_neq(res, NULL);
  
            cr_assert_eq(bool_from_uint64(x_y), bool_from_uint64(res));

            free_bigbool(res);
            free_bigbool(x_or_y);
            free_bigbool(_x_or__y);
            free_bigbool(_y);
            free_bigbool(_x);
            free_bigbool(x_y);
            free_bigbool(y);
        }
        free_bigbool(x);
    }
}
