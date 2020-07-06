#include <inttypes.h>

#ifndef BOOL_H
#define BOOL_H

#define ERROR_BAD_POINTER 1
#define ERROR_TOO_BIG 2

enum mode_of_operation {AND, OR, XOR};

struct bigbool 
{
    uint8_t* parts;
    int last_bit;
    int last_byte;
};

//Читает с клавы только ноль или единицу
//Динамически выделает память для строки
//Не заносит в строку первые незначащие нули
//Возвращает указатель на строку
char* scan_vector();

//Возвращает -1 в случае, если war == NULL
//В остальных случаях возвращает длину вектора
int len_bb(struct bigbool *war);

//Транслирует строку в структуру BigBool
//Динамически выделяет память под BigBool
//Есть проверка только на vector == NULL
//Поэтому нужно заранее проверить корректно ли введён вектор
//Здесь корректность гарантируется функцией "char* scan_vector()"
struct bigbool *char_from_bool(char *vector);

//Транслирует структуру BigBool в строку
//Динамически выделяет память под строку
//Есть проверка только на war == NULL
//Поэтому нужно заранее проверить корректно ли заполнена struct bigbool *war
//Здесь корректность гарантируется функцией
//"struct bigbool *char_from_bool(char *vector);" 
//Или "struct bigbool *uint64_from_bool(uint64_t vector);"
char *bool_from_char(struct bigbool *war);

//Транслирует структуру BigBool в uint64_t
//Есть проверка только на war == NULL
//Поэтому нужно заранее проверить корректно ли заполнена struct bigbool *war
//Здесь корректность гарантируется функцией
//"struct bigbool *char_from_bool(char *vector);" 
//Или "struct bigbool *uint64_from_bool(uint64_t vector);"
uint64_t bool_from_uint64(struct bigbool *war);

//Транслирует uint64_t d структуру BigBool 
//Динамически выделяет память под BigBool
struct bigbool *uint64_from_bool(uint64_t vector);

//war реалоком расширяется на n бит
//то есть справа дописывается n нулей
//если war == NULL, то вернёт NULL
//если n < 0, вызывается ф-ия struct bigbool* shift_right(struct bigbool *war, int n)
//если n = 0, возвращает исходную структуру
struct bigbool* shift_left(struct bigbool *war, int n);

//если war == NULL, то вернёт NULL
//если n < 0, вызывается ф-ия struct bigbool* shift_left(struct bigbool *war, int n)
//если n = 0, возвращает исходную структуру
//если n >= длины вектора, то вектор становится нулевым, а last_bit и last_byte = 1
//память освобождается реалоком
struct bigbool* shift_right(struct bigbool *war, int n);

//есть проверка на war == NULL
//если n < 0, вызывается ф-ия struct bigbool* cyclic_shift_right(struct bigbool *war, int n);
//если n = 0, возвращает исходную структуру
//если n >= длины вектора, то n берётся по модулю длины вектора
struct bigbool* cyclic_shift_left(struct bigbool *war, int n);

//есть проверка на war == NULL
//если n < 0, вызывается ф-ия struct bigbool* cyclic_shift_left(struct bigbool *war, int n);
//если n = 0, возвращает исходную структуру
//если n >= длины вектора, то n берётся по модулю длины вектора
struct bigbool* cyclic_shift_right(struct bigbool *war, int n);

// есть проверка на нулевой указатель
void free_bigbool(struct bigbool *war);

//есть проаерки на war == NULL, n < 0;
//расширение с помощью realloc
struct bigbool* expansion_bb(struct bigbool* war, int n);

// if | - mode = OR; if & - mode = AND; if ^ - mode = XOR;
// возвращает указатель на результат
// a и b остаются неизменными, результат в новой структуру, котороя динамически выделяется на куче
// есть проверки на a и b == NULL, и корректность mode
struct bigbool* operation_bb(struct bigbool* a, int mode, struct bigbool* b);

// а остаётся неизменной, результат в новой структуре, которая динамически выделятся на куча
// возвращает указатель на результат
// есть проверка на a == NULL
struct bigbool* invertbb(struct bigbool* a);


#endif
