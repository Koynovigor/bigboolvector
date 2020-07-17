#include "bool.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *scan_vector()
{
    int count = 0;
    int count_max = 128;
    int symbol;
    char *str = calloc(count_max, sizeof(char));
    if (str == NULL)
    {
        perror("Не хватает места");
        return NULL;
    }

    int beginning = 1;
    int delete = 0;
    while ((symbol = fgetc(stdin)) != '\n')
    {
        if (((symbol - '0') | 0x1) != 0x1)
        {
            fprintf(stderr, "Не правильно введён вектор\n EXE: 101011");
            return NULL;
        }

        if (((symbol - '0') == 0x0) && (beginning))
        {
            delete ++;
            continue;
        }
        if (((symbol - '0') == 0x1) && (beginning))
        {
            beginning--;
        }
        
        str[count] = symbol;
        count++;
        if (count == count_max)
        {
            str = (char *)realloc(str, count_max + 128);
            if (str == NULL)
            {
                perror("Ресурсы пямяти исчерпаны");
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

int len_bb(struct bigbool *war)
{
    if (war == NULL)
    {
        fprintf(stderr, "В функцию len_bb передан нулевой указатель");
        return -1;
    }
    
    return (((war->last_byte - 1) * 8) + war->last_bit);
}

struct bigbool *char_from_bool(char *vector)
{
    int len = strlen(vector);
    if (len == 0)
    {
        fprintf(stderr,"Вы передали нулевой указатель в функцию char_from_bool");
        return NULL;
    }

    struct bigbool *war = (struct bigbool *)calloc(1, sizeof(struct bigbool));
    if (war == NULL)
    {
        perror("Ресурсы пямяти исчерпаны");
        return NULL;
    }

    war->last_bit = (uint8_t)len % 8;
    war->last_byte = (uint8_t)len / 8;
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
        perror("Ресурсы пямяти исчерпаны");
        return NULL;
    }

    int j = -1;
    for (int i = 0; i < len; i++)
    {
        if (i % 8 == 0)
        {
            j++;
        }
        char a = vector[i];

        war->parts[j] = war->parts[j] | (uint8_t)((uint8_t)(a - '0') << (i % 8));
    }
    return war;
};

char *bool_from_char(struct bigbool *war)
{
    if (war == NULL)
    {
        fprintf(stderr, "Передан нулевой указатель в фукцию bool_from_char");
        return NULL;
    }
    
    if ((war->last_bit == 0) && (war->last_byte == 0))
    {
        fprintf(stderr, "Передана пустая структура в фукцию bool_from_char");
        return NULL;
    }

    if ((war->last_bit == 1) && (war->last_byte == 1))
    {
        char *vector = (char *)calloc(2, sizeof(char));
        if (vector == NULL)
        {
            perror("Ресурсы пямяти исчерпаны");
            return NULL;
        }
        vector[0] = (int)war->parts[0] + '0';
        vector[1] = 0x0;
        return vector;
    }

    int len = len_bb(war);
    if (len == -1)
    {
        return NULL;
    }
    
    char *vector = (char *)calloc(len + 1, sizeof(char));
    if (vector == NULL)
    {
        perror("Ресурсы пямяти исчерпаны");
        return NULL;
    }

    int k = -1;
    for (int i = 0; i < len; i++)
    {
        if (i % 8 == 0)
        {
            k++;
        }
        uint8_t a = war->parts[k];
        vector[i] = ((int)((a & (uint8_t)(1 << i % 8)) >> i % 8) + '0');
    }
    vector[len] = 0x0;
    return vector;
}

uint64_t bool_from_uint64(struct bigbool *war)
{
    if (war == NULL)
    {
        fprintf(stderr, "Передан нулевой указатель в фукцию bool_from_uint64");
        return ERROR_BAD_POINTER;
    }

    if ((len_bb(war)) > 64)
    {
        fprintf(stderr, "Введён слишком большой вектор");
        return ERROR_TOO_BIG;
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

struct bigbool *uint64_from_bool(uint64_t vector)
{
    int len = 0;
    for (len = 63; len >= 0; len--)
    {
        if ((((uint64_t)(1 << len) & vector) >> len) == 1)
        {
            break;
        } 
    }
    if (len == -1)
    {
        len++;
    }
    len ++;
    struct bigbool *war = (struct bigbool *)calloc(1, sizeof(struct bigbool));
    if (war == NULL)
    {
        perror("Ресурсы пямяти исчерпаны");
        return NULL;
    }
    war->last_bit = (uint8_t)len%8;
    war->last_byte = (uint8_t)len/8;
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
        perror("Ресурсы пямяти исчерпаны");
        return NULL;
    }

    for (int i = 0; i < war->last_byte - 1; i++)
    {
        for (int j = 0; j < 8; j++)
        {
            war->parts[i] |= ((((1 << (len - 1)) & vector) >> (len - 1)) << j);
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

struct bigbool *shift_left(struct bigbool *war, int n)
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
        fprintf(stderr, "Передан нулевой указатель в фукцию shift_left");
        return NULL;
    }
    
    int len = len_bb(war) + n;
    int count = war->last_byte;
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

    for (int i = count; i < war->last_byte; i++)
    {
        war->parts[i] = 0;
    }

return war;
}

struct bigbool *shift_right(struct bigbool *war, int n)
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

    int len = len_bb(war) - n;
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
        fprintf(stderr, "Передан нулевой указатель в фукцию cyclic_shift_left");
        return NULL;
    }

    n = n%(len_bb(war));
    for (int i = 0; i < n; i++)
    {
        int k = war->last_bit - 1;
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
        fprintf(stderr, "Передан нулевой указатель в фукцию cyclic_shift_right");
        return NULL;
    }

    n = n%(len_bb(war));
    
    for (int i = 0; i < n; i++)
    {
        int k = war->last_bit - 1;
            
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
return war;
}

void free_bigbool(struct bigbool *war)
{
    if ((war == NULL) || (war->parts == NULL))
    {
        fprintf(stderr, "Передан нулевой указатель в фукцию free_bigbool");
        return;
    }
    free(war->parts);
    free(war);
}

struct bigbool* expansion_bb(struct bigbool* war, int n)
{
    if (war == NULL)
    {
        fprintf(stderr, "Передан нулевой указатель в фукцию expansion_bb");
        return NULL;
    }
    if (n == 0)
    {
        return war;
    }
    if (n < 0)
    {
        fprintf(stderr, "Ошибка: расширение на отрицательное чисо");
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

struct bigbool* operation_bb(struct bigbool* a, int mode, struct bigbool* b)
{
    if ((a == NULL) || (b == NULL))
    {
        fprintf(stderr, "Передан нулевой(ые) указатель(и) в фукцию and_bb");
        return NULL;
    }

    if ((mode != OR) && (mode != AND) && (mode != XOR))
    {
        fprintf(stderr, "if '|' - mode = OR; if '&' - mode = AND; if '^' - mode = XOR;");
        return NULL;
    }

    struct bigbool* c = (struct bigbool *)calloc(1, sizeof(struct bigbool));
    if (c == NULL)
    {
        perror("Ресурсы памяти исчерпаны");
        return NULL;
    }

    if (len_bb(a) > len_bb(b))
    {
        c->last_bit = a->last_bit;
        c->last_byte = a->last_byte;
        if (expansion_bb(b, (len_bb(a) - len_bb(b))) == NULL)
        {
            return NULL;
        }
    }
    else
    {
        c->last_bit = b->last_bit;
        c->last_byte = b->last_byte;
        if (expansion_bb(a, (len_bb(b) - len_bb(a))) == NULL)
        {
            return NULL;
        }
    }
    c->parts = calloc(c->last_byte, sizeof(uint8_t));
    if (c == NULL)
    {
        perror("Не достаточно ресурсов памяти");
        return NULL;
    }
    
    if (mode == AND)
    {
        for (int i = 0; i < c->last_byte; i++)
        {
            c->parts[i] = a->parts[i] & b->parts[i];
        }
    }
    if (mode == OR)
    {
        for (int i = 0; i < c->last_byte; i++)
        {
            c->parts[i] = a->parts[i] | b->parts[i];
        }
    }
    if (mode == XOR)
    {
        for (int i = 0; i < c->last_byte; i++)
        {
            c->parts[i] = a->parts[i] ^ b->parts[i];
        }
    }
    
return c;
}

struct bigbool* invertbb(struct bigbool* a)
{
    if (a == NULL)
    {
        fprintf(stderr, "Передан нулевой указатель в фукцию invert_bb");
        return NULL;
    }

    struct bigbool* c = (struct bigbool *)calloc(1, sizeof(struct bigbool));
    if (c == NULL)
    {
        perror("Ресурсы памяти исчерпаны");
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
