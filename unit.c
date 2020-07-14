#include "bool.h"
#include <stdio.h>
#include <limits.h>
#include <criterion/criterion.h>
#include <inttypes.h>

Test(unit, creat_bb) {

    for (uint64_t i = (ULLONG_MAX - 1000); i <  ULLONG_MAX; i++)
    {
        struct bigbool* war = bool_from_uint64(i);
        cr_assert_neq(war, NULL);
        char* str = char_from_bool(war);
        cr_assert_neq(str, NULL);
        struct bigbool* war2 = bool_from_char(str);
        cr_assert_neq(war2, NULL);

        cr_assert_eq(uint64_from_bool(war2), i);

        free_bigbool(war2);
        free(str);
        free_bigbool(war);
    }
}


Test(unit, shift_bb) {

for (uint64_t k = USHRT_MAX - 1000; k < USHRT_MAX; k++)
{
    for (uint64_t i = 0; i < 15; i++)
    {
        struct bigbool* war = bool_from_uint64(k);
        cr_assert_neq(war, NULL);

        uint64_t res = 1;
        for (int j = 0; j < i; j++)
        {
            res = (uint64_t)1 << i;
        }
        
        cr_assert_eq(uint64_from_bool(shift_left(war, i)), (uint64_t)(k * res));

        cr_assert_eq(uint64_from_bool(shift_right(war, i)), k);
        free_bigbool(war);
    }
  
    for (size_t i = 0; i < 10; i++)
    {
        struct bigbool* war1 = bool_from_uint64(k);
        cr_assert_neq(war1, NULL);
        struct bigbool* war2 = bool_from_uint64(k);
        cr_assert_neq(war2, NULL);

        cr_assert_eq(uint64_from_bool(shift_left(war1, -i)), uint64_from_bool(shift_right(war2, i)));
        
        free_bigbool(war1);
        free_bigbool(war2);
    }

    for (int i = 0; i < 10; i++)
    {
        struct bigbool* war1 = bool_from_uint64(k);
        cr_assert_neq(war1, NULL);

        for (int j = len_bb(war1); j < len_bb(war1) + 10; j++)
        {
            cr_assert_eq(uint64_from_bool(shift_right(war1, j)), 0);
        } 

        free_bigbool(war1);
    }
}
    struct bigbool* war = bool_from_char("101011110101101010101010101011001110101010100110101001101010110101011101001");
    cr_assert_neq(war, NULL);

    char* str1 = char_from_bool(war);
    cr_assert_str_eq(str1, "101011110101101010101010101011001110101010100110101001101010110101011101001");
    free(str1);
    
    shift_left(war, 10);

    char* str2 = char_from_bool(war);
    cr_assert_str_eq(str2, "1010111101011010101010101010110011101010101001101010011010101101010111010010000000000");
    free(str2);

    shift_right(war, 15);

    char* str3 = char_from_bool(war);
    cr_assert_str_eq(str3, "1010111101011010101010101010110011101010101001101010011010101101010111");
    free(str3);

    cyclic_shift_left(war, 30);

    char* str4 = char_from_bool(war);
    cr_assert_str_eq(str4, "0011101010101001101010011010101101010111101011110101101010101010101011");
    free(str4);

    cyclic_shift_right(war, 10);

    char* str5 = char_from_bool(war);
    cr_assert_str_eq(str5, "1010101011001110101010100110101001101010110101011110101111010110101010");
    free(str5);

    free_bigbool(war);
}
