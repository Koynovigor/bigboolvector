#include "bool.h"
#include <stdio.h>
#include <limits.h>
#include <criterion/criterion.h>
#include <inttypes.h>

Test(unit, creat_bb) {

    for (uint64_t i = 0; i < SHRT_MAX; i++)
    {
        struct bigbool* war = uint64_from_bool(i);
        cr_assert_neq(war, NULL);
        char* str = bool_from_char(war);
        cr_assert_neq(str, NULL);
        struct bigbool* war2 = char_from_bool(str);
        cr_assert_neq(war2, NULL);

        cr_assert_eq(bool_from_uint64(war2), i);

        free_bigbool(war2);
        free(str);
        free_bigbool(war);
    }
}


Test(unit, shift_bb) {

for (uint64_t k = 0; k < SHRT_MAX; k++)
{
    for (uint64_t i = 0; i < 15; i++)
    {
        struct bigbool* war = uint64_from_bool(k);
        cr_assert_neq(war, NULL);

        uint64_t res = 1;
        for (int j = 0; j < i; j++)
        {
            res *= 2;
        }
        
        cr_assert_eq(bool_from_uint64(shift_left(war, i)), (uint64_t)(k * res));

        cr_assert_eq(bool_from_uint64(shift_right(war, i)), k);
        free_bigbool(war);
    }
  
    for (int i = -10; i < 10; i++)
    {
        struct bigbool* war1 = uint64_from_bool(k);
        cr_assert_neq(war1, NULL);
        struct bigbool* war2 = uint64_from_bool(k);
        cr_assert_neq(war2, NULL);

        cr_assert_eq(bool_from_uint64(shift_left(war1, -i)), bool_from_uint64(shift_right(war2, i)));
        
        free_bigbool(war1);
        free_bigbool(war2);
    }

    for (int i = 0; i < 10; i++)
    {
        struct bigbool* war1 = uint64_from_bool(k);
        cr_assert_neq(war1, NULL);

        for (int j = len_bb(war1); j < len_bb(war1) + 10; j++)
        {
            cr_assert_eq(bool_from_uint64(shift_right(war1, j)), 0);
        } 

        free_bigbool(war1);
    }
}
}
