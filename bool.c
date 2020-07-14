#include "bool.h"
#include <math.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#define NO 0
#define YES 1

int errno = ERR_OK;
int beginning = YES;
int delete = NO;

int check_symbol(int symbol)
{
    if (((symbol - '0') | 0x1) != 0x1)
    {
        errno = ERR_WRT;
        return 0;
    }
    if (((symbol - '0') == 0x0) && (beginning))
    {
        delete = YES;
        return 1;
    }
    if (((symbol - '0') == 0x1) && (beginning))
    {
        beginning = NO;
        return symbol;
    }
    return symbol;
}

char *scan_vector()
{
    int count = 0;
    int count_max = 128;
    int symbol;
    char *str = calloc(count_max, sizeof(char));
    if (str == NULL)
    {
        errno = ERR_MEM;
        return NULL;
    }

    beginning = YES;
    delete = NO;
    while ((symbol = fgetc(stdin)) != '\n')
    {
        switch (check_symbol(symbol))
        {
        case 0:
            return NULL;
            break;
        case 1:
            continue;
            break;
        }
        str[count] = symbol;
        count++;
        if (count == count_max)
        {
            str = (char *)realloc(str, count_max + 128);
            if (str == NULL)
            {
                errno = ERR_MEM;
                return NULL;
            }
            count_max += 128;
        }      
    }
    if ((strlen(str) == 0) && delete)
    {
        str[0] = '0';
    }
    return str;
}

size_t len_bb(struct bigbool *war)
{
    if (war == NULL)
    {
        errno = ERR_ARG;
        return -1;
    }
    
    return (((war->last_byte - 1) * 8) + war->last_bit);
}

struct bigbool *empty_bool(size_t len)
{
    struct bigbool *war = (struct bigbool *)calloc(1, sizeof(struct bigbool));
    if (war == NULL)
    {
        errno = ERR_MEM;
        return NULL;
    }
    war->last_bit = len%8;
    war->last_byte = len/8;
    if (war->last_bit == 0)
    {
        war->last_bit = 8;
    }
    else
    {
        war->last_byte++;
    }
    war->parts = (uint8_t *)calloc(war->last_byte, sizeof(uint8_t));
    if (war->parts == NULL)
    {
        errno = ERR_MEM;
        return NULL;
    }
return war;
}

struct bigbool *bool_from_char(char *vector)
{
    size_t len = strlen(vector);
    if (len == 0)
    {
        errno = ERR_ARG;
        return NULL;
    }
    beginning = YES;
    delete = NO;
    //Возникает вопрос почему бы после проверки символа сразу заносить его в структуру?
    //Тогда придётся после каждого восьмого символа использовать realloc для расширения структуры
    //Что весьма странно
    //Поэтому сначала я считаю фактическую длину(без первых незначащих нулей)
    //Потом выделяю структуру для вектора новой длины
    //И потом записываю вектор в структуру 
    for (size_t i = 0; i < strlen(vector); i++)
    {
        switch (check_symbol(vector[i]))
        {
        case 0:
            return NULL;
            break;
        case 1:
            len --;
            break;
        }
    }
    if (len == 0)
    {
        struct bigbool *war = empty_bool(1);
        if (war == NULL)
        {
            return NULL;
        }
        war->parts[0] = 0;
        return war;
    }

    struct bigbool *war = empty_bool(len);
    if (war == NULL)
    {
        return NULL;
    }
    beginning = YES;
    delete = NO;
    int j = -1;
    for (size_t i = 0; i < strlen(vector); i++)
    {
        if (check_symbol(vector[i]) == 1)
        {
            continue;
        }
        if (i % 8 == 0)
        {
            j++;
        }
        char a = vector[i];
        war->parts[j] = war->parts[j] | (uint8_t)((uint8_t)(a - '0') << (i % 8));
    }
    return war;
};

char *char_from_bool(struct bigbool *war)
{
    if (war == NULL)
    {
        errno = ERR_ARG;
        return NULL;
    }
    
    if ((war->last_bit == 0) && (war->last_byte == 0))
    {
        errno = ERR_EMP;
        return NULL;
    }

    size_t len = len_bb(war);
    if (len == -1)
    {
        return NULL;
    }
    
    char *vector = (char *)calloc(len + 1, sizeof(char));
    if (vector == NULL)
    {
        errno = ERR_MEM;
        return NULL;
    }

    size_t k = -1;
    for (size_t i = 0; i < len; i++)
    {
        if (i % 8 == 0)
        {
            k++;
        }
        uint8_t a = war->parts[k];
        vector[i] = (((a & ((uint8_t)1 << (i%8))) >> (i%8)) + '0');
    }
    vector[len] = 0x0;
    return vector;
}

uint64_t uint64_from_bool(struct bigbool *war)
{
    if (war == NULL)
    {
        errno = ERR_ARG;
        return -1;
    }

    if ((len_bb(war)) > 64)
    {
        errno = ERR_BIG;
        return -1;
    }

    uint64_t vector = 0;
    int k = 0;
    for (int i = war->last_bit - 1 ; i >= 0; i--)
    {
        vector |= ((uint64_t)(((uint8_t)(1 << i) & war->parts[war->last_byte - 1]) >> i) << k);
        k++;
    }

    for (int i = war->last_byte - 2; i >= 0; i--)
    {
        for (int j = 7; j >= 0; j--)
        {
            vector |= ((uint64_t)(((uint8_t)(1 << j) & war->parts[i]) >> j) << k);
            k++;
        }
    }
    
return vector;
}

struct bigbool *bool_from_uint64(uint64_t vector)
{
    int len = 0;
    for (len = 63; len >= 0; len--)
    {
        if (((((uint64_t)1 << len) & vector) >> len) == 1)
        {
            break;
        } 
    }
    if (len == -1)
    {
        len ++;
    }
    len ++;

    struct bigbool *war = empty_bool(len);
    if (war == NULL)
    {
        return NULL;
    }
    
    for (int i = 0; i < war->last_byte - 1; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            war->parts[i] |= (((((uint64_t)1 << (len - 1)) & vector) >> (len - 1)) << j);
            len --;
        }
    }

    for (int i = 0; i < war->last_bit; i++)
    {
        war->parts[war->last_byte - 1] |= ((((1 << (len - 1)) & vector) >> (len - 1)) << i);
        len --;
    }
    
return war;
}

struct bigbool *shift_left(struct bigbool *war, size_t n)
{
    if (n == 0)
    {
        return war;
    }
    
    if (n < 0)
    {
        n *= -1;
        return shift_right(war, n);
    }

    if (war == NULL)
    {
        errno = ERR_ARG;
        return NULL;
    }
    
    size_t len = len_bb(war) + n;
    size_t count = war->last_byte;
    war->last_byte = len/8 + 1;
    war->last_bit = len % 8;
    if (war->last_bit == 0)
    {
        war->last_byte --;
        war->last_bit = 8;
    }

    war->parts = (uint8_t *)realloc(war->parts, war->last_byte);
    if (war->parts == NULL)
    {
        errno = ERR_MEM;
        return NULL;
    }

    for (size_t i = count; i < war->last_byte; i++)
    {
        war->parts[i] = 0;
    }

return war;
}

struct bigbool *shift_right(struct bigbool *war, size_t n)
{
    if (war == NULL)
    {
        fprintf(stderr, "Передан нулевой указатель в фукцию shift_right");
        return NULL;
    }

    if (n == 0)
    {
        return war;
    }

    if (n < 0)
    {
        n *= -1;
        return shift_left(war, n);
    }

    if (n >= len_bb(war))
    {
        war->parts = (uint8_t *)realloc(war->parts, 1);
        if (war->parts == NULL)
        {
            perror("Ресурсы памяти исчерпаны");
            return NULL;
        }
        war->parts[0] = 0;
        war->last_bit = 1;
        war->last_byte = 1;
    return war;
    }

    size_t len = len_bb(war) - n;
    war->last_byte = len/8 + 1;
    war->last_bit = len % 8;
    if (war->last_bit == 0)
    {
        war->last_byte --;
        war->last_bit = 8;
    }

    war->parts = (uint8_t *)realloc(war->parts, war->last_byte);
    if (war->parts == NULL)
    {
        perror("Ресурсы памяти исчерпаны");
        return NULL;
    } 
    
    uint8_t delite = 0;
    for (int i = 7; i >= war->last_bit; i--)
    {
        delite |= 1 << i;
    }
    war->parts[war->last_byte - 1] &=  ~(delite);

return war;
}

struct bigbool* cyclic_shift_left(struct bigbool *war, int n)
{
    if (n == 0)
    {
        return war;
    }

    if (n < 0)
    {
        n *= -1;
        return cyclic_shift_right(war, n);
    }

    if (war == NULL)
    {
        return NULL;
    }

    n = n%(len_bb(war));
    int k = war->last_bit - 1;
    int bit = n % 8;
    // Сначала побитовый сдвиг на n%8, чтобы сдвинуть байты, которые больше не будут менятся 
    for (int i = 0; i < bit; i++)
    {
        for (int j = k - 1; j >= 0; j--)
        {
            uint8_t x = (war->parts[war->last_byte - 1] >> k) & 1;
            uint8_t y = (war->parts[war->last_byte - 1] >> j) & 1;
            x = x ^ y;
            x = (x << k) | (x << j);
            war->parts[war->last_byte - 1] = war->parts[war->last_byte - 1] ^ x;
        }
        for (int i = war->last_byte - 2; i >= 0; i--)
        {
            for (int j = 7; j >= 0; j--)
            {
                uint8_t x = (war->parts[war->last_byte - 1] >> k) & 1;
                uint8_t y = (war->parts[i] >> j) & 1;
                x = x ^ y; 
                war->parts[war->last_byte - 1] = war->parts[war->last_byte - 1] ^ (x << k);
                war->parts[i] = war->parts[i] ^ (x << j);
            }
        } 
    }

    int byte = n/8;
    //Здесь я слвигаю эти байты на свои места
    for (int i = 0; i < byte; i++)
    {
        for (int j = war->last_byte - 3; j >= 0; j--)
        {
            uint8_t x = war->parts[war->last_byte - 2]; 
            uint8_t y = war->parts[j];
            x = (uint8_t)(x ^ y);
            y = (uint8_t)(x ^ y);
            x = (uint8_t)(x ^ y);
            war->parts[war->last_byte - 2] = (uint8_t)x;
            war->parts[j] = y;
        }
    }

    //Здесь сдвигаю биты, но не во всём векторе, а только в некоторых байтах
    for (int i = 0; i < war->last_bit; i++)
    {
        for (int t = war->last_byte - byte - 1; t < war->last_byte - 1; t++)
        {
            for (int j = 0; j < 8; j++)
            {
                uint8_t x = (war->parts[war->last_byte - 1] >> k) & 1;
                uint8_t y = (war->parts[t] >> j) & 1;
                x = x ^ y; 
                war->parts[war->last_byte - 1] = war->parts[war->last_byte - 1] ^ (x << k);
                war->parts[t] = war->parts[t] ^ (x << j);
            } 
        } 

        for (int j = 0; j < k; j++)
        {
            uint8_t x = (war->parts[war->last_byte - 1] >> k) & 1;
            uint8_t y = (war->parts[war->last_byte - 1] >> j) & 1;
            x = x ^ y;
            x = (x << k) | (x << j);
            war->parts[war->last_byte - 1] = war->parts[war->last_byte - 1] ^ x;
        }
    }  
return war;
}

struct bigbool* cyclic_shift_right(struct bigbool *war, int n)
{
    if (n == 0)
    {
        return war;
    }

    if (n < 0)
    {
        n *= -1;
        return cyclic_shift_left(war, n);
    }

    if (war == NULL)
    {
        return NULL;
    }

    n = n%(len_bb(war));
    int k = war->last_bit - 1;
    int bit = n % 8;
    for (int i = 0; i < bit; i++)
    { 
        for (int i = 0; i < war->last_byte - 1; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                uint8_t x = (war->parts[war->last_byte - 1] >> k) & 1;
                uint8_t y = (war->parts[i] >> j) & 1;
                x = x ^ y; 
                war->parts[war->last_byte - 1] = war->parts[war->last_byte - 1] ^ (x << k);
                war->parts[i] = war->parts[i] ^ (x << j);
            } 
        }

        for (int j = 0; j < k; j++)
        {
            uint8_t x = (war->parts[war->last_byte - 1] >> k) & 1;
            uint8_t y = (war->parts[war->last_byte - 1] >> j) & 1;
            x = x ^ y;
            x = (x << k) | (x << j);
            war->parts[war->last_byte - 1] = war->parts[war->last_byte - 1] ^ x;
        }
    }

    int byte = n/8;
    for (int i = 0; i < war->last_bit; i++)
    {
        for (int j = k - 1; j >= 0; j--)
        {
            uint8_t x = (war->parts[war->last_byte - 1] >> k) & 1;
            uint8_t y = (war->parts[war->last_byte - 1] >> j) & 1;
            x = x ^ y;
            x = (x << k) | (x << j);
            war->parts[war->last_byte - 1] = war->parts[war->last_byte - 1] ^ x;
        }
        for (int t = war->last_byte - 2; t > war->last_byte - 2 - byte; t--)
        {
            for (int j = 7; j >= 0; j--)
            {
                uint8_t x = (war->parts[war->last_byte - 1] >> k) & 1;
                uint8_t y = (war->parts[t] >> j) & 1;
                x = x ^ y; 
                war->parts[war->last_byte - 1] = war->parts[war->last_byte - 1] ^ (x << k);
                war->parts[t] = war->parts[t] ^ (x << j);
            }
        }
    }

    for (int i = 0; i < byte; i++)
    {
        for (int j = 0; j < war->last_byte - 2; j++)
        {
            uint8_t x = war->parts[war->last_byte - 2]; 
            uint8_t y = war->parts[j];
            x = (uint8_t)(x ^ y);
            y = (uint8_t)(x ^ y);
            x = (uint8_t)(x ^ y);
            war->parts[war->last_byte - 2] = (uint8_t)x;
            war->parts[j] = y;
        }
    }
    return war;
}

int free_bigbool(struct bigbool *war)
{
    if ((war == NULL) || (war->parts == NULL))
    {
        return 1;
    }
    free(war->parts);
    free(war);
return 0;
}

struct bigbool* expansion_bb(struct bigbool* war, int n)
{
    if (war == NULL)
    {
        return NULL;
    }
    if (n == 0)
    {
        return war;
    }
    if (n < 0)
    {
        return NULL;
    }
    
    int len = len_bb(war);
    if (shift_left(war, n) == NULL)
    {
        return NULL;
    }

    if (cyclic_shift_left(war, len) == NULL)
    {
        return NULL;
    }
return war;
}

struct bigbool* operation_bb(struct bigbool* a, enum mode_of_operation mode, struct bigbool* b)
{
    if ((a == NULL) || (b == NULL))
    {
        errno = ERR_ARG;
        return NULL;
    }

    if ((mode != OR) && (mode != AND) && (mode != XOR))
    {
        errno = ERR_MOD;
        return NULL;
    }

    struct bigbool* c = NULL;
    if (len_bb(b) > len_bb(a))
    {
        c = a;
        a = b;
        b = c;
    }

    c = empty_bool(len_bb(a));
    if (c == NULL)
    {
        return NULL;
    }
    
    if (expansion_bb(b, (len_bb(a) - len_bb(b))) == NULL)
    {
        return NULL;
    }
    
    for (int i = 0; i < c->last_byte; i++)
    {
        switch (mode)
        {
        case AND:
            c->parts[i] = a->parts[i] & b->parts[i];
            break;
        
        case OR:
            c->parts[i] = a->parts[i] & b->parts[i];
            break;

        case XOR:
            c->parts[i] = a->parts[i] ^ b->parts[i];
            break;
        }
    }
return c;
}

struct bigbool* invertbb(struct bigbool* a)
{
    if (a == NULL)
    {
        errno = ERR_ARG;
        return NULL;
    }

    struct bigbool* c = (struct bigbool *)calloc(1, sizeof(struct bigbool));
    if (c == NULL)
    {
        errno = ERR_MEM;
        return NULL;
    }
    c->last_bit = a->last_bit;
    c->last_byte = a->last_byte;
    c->parts = calloc(c->last_byte, sizeof(uint8_t));

    for (int i = 0; i < a->last_byte; i++)
    {
        c->parts[i] = (uint8_t)~(a->parts[i]);
    }

    for (int i = a->last_bit; i < 8; i++)
    {
        c->parts[c->last_byte-1] &= ~(1 << i);
    }
    
return c;
}
