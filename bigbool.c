#include "bool.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{

    struct bigbool* war = uint64_from_bool(0);
    char* str = bool_from_char(war);
    printf("%s\n", str);
    struct bigbool* war2 = char_from_bool(str);

    printf("%ld  %d\n", bool_from_uint64(war2), 0);

return 0;
}



,
