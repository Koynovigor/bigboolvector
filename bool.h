#include <inttypes.h>
#include <stddef.h> 

#ifndef BOOL_H
#define BOOL_H

#define ERR_OK 0
#define ERR_MEM 1
#define ERR_ARG 2 
#define ERR_WRT 3
#define ERR_EMP 4
#define ERR_BIG 5
#define ERR_MOD 6

extern int errno;

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
//если функция вернула NULL, то errno указывает на возникшую ошибку:
//ERR_MEM -- невозможность выделить память 
//ERR_WRT не правильно введён вектор. EXE: 101011
char* scan_vector();

//Если ф-ия вернула -1, то war == NULL
//В остальных случаях возвращает длину вектора
size_t len_bb(struct bigbool *war);

//Транслирует строку в структуру BigBool
//Динамически выделяет память под BigBool
//Для очистки использовать функцию int free_bigbool(struct bigbool *war)
//Проверяет корректность вектора
//если функция вернула NULL, то errno указывает на возникшую ошибку:
//ERR_MEM -- невозможность выделить память 
//ERR_ARG -- передан нулевой вектор
//ERR_WRT -- вектор некорректен
struct bigbool *bool_from_char(char *vector);

//Транслирует структуру BigBool в строку
//Динамически выделяет память под строку
//Есть проверка только на war == NULL
//если функция вернула NULL, то errno указывает на возникшую ошибку:
//ERR_MEM -- невозможность выделить память 
//ERR_ARG -- передан нулевой указатель
//ERR_EMP -- передана пустая структура
char *char_from_bool(struct bigbool *war);

//Транслирует структуру BigBool в uint64_t
//Есть проверка только на war == NULL
//если функция вернула NAN, то errno указывает на возникшую ошибку:
//ERR_ARG -- передан нулевой указатель
//ERR_BIG -- слишком большой вектор
uint64_t uint64_from_bool(struct bigbool *war);

//Транслирует uint64_t d структуру BigBool 
//Динамически выделяет память под BigBool
//Для очистки использовать функцию int free_bigbool(struct bigbool *war)
//если функция вернула NAN, то errno указывает на возникшую ошибку:
//ERR_MEM -- невозможность выделить память 
struct bigbool *bool_from_uint64(uint64_t vector);

//war реалоком расширяется на n бит
//то есть справа дописывается n нулей
//если n < 0, вызывается ф-ия struct bigbool* shift_right(struct bigbool *war, int n)
//если n = 0, возвращает исходную структуру
//если функция вернула NULL, то errno указывает на возникшую ошибку:
//ERR_MEM -- невозможность выделить память 
//ERR_ARG -- передан нулевой указатель
struct bigbool* shift_left(struct bigbool *war, size_t n);

//если n < 0, вызывается ф-ия struct bigbool* shift_left(struct bigbool *war, int n)
//если n = 0, возвращает исходную структуру
//если n >= длины вектора, то вектор становится нулевым, а last_bit и last_byte = 1
//память освобождается реалоком
//если функция вернула NULL, то errno указывает на возникшую ошибку:
//ERR_MEM -- невозможность выделить память 
//ERR_ARG -- передан нулевой указатель
struct bigbool* shift_right(struct bigbool *war, size_t n);

//если n < 0, вызывается ф-ия struct bigbool* cyclic_shift_right(struct bigbool *war, int n);
//если n = 0, возвращает исходную структуру
//если n >= длины вектора, то n берётся по модулю длины вектора
//если функция вернула NULL, то был передан нулевой указатель
struct bigbool* cyclic_shift_left(struct bigbool *war, int n);

//если n < 0, вызывается ф-ия struct bigbool* cyclic_shift_left(struct bigbool *war, int n);
//если n = 0, возвращает исходную структуру
//если n >= длины вектора, то n берётся по модулю длины вектора
//если функция вернула NULL, то был передан нулевой указатель
struct bigbool* cyclic_shift_right(struct bigbool *war, int n);

//если функция вернула 1, то передан нулевой указатель
int free_bigbool(struct bigbool *war);

// if | - mode = OR; if & - mode = AND; if ^ - mode = XOR;
// возвращает указатель на результат
// a и b остаются неизменными, результат в новой структуру, котороя динамически выделяется на куче
//Для очистки использовать функцию int free_bigbool(struct bigbool *war)
//если функция вернула NULL, то errno указывает на возникшую ошибку:
//ERR_MEM -- невозможность выделить память 
//ERR_ARG -- передан нулевой указатель
//ERR_MOD -- неправильно указана операция. EXE: operation_bb(a, XOR, b);
struct bigbool* operation_bb(struct bigbool* a, enum mode_of_operation mode, struct bigbool* b);

// а остаётся неизменной, результат в новой структуре, которая динамически выделятся на куча
//Для очистки использовать функцию int free_bigbool(struct bigbool *war)
// возвращает указатель на результат
//если функция вернула NULL, то errno указывает на возникшую ошибку:
//ERR_MEM -- невозможность выделить память 
//ERR_ARG -- передан нулевой указатель
struct bigbool* invertbb(struct bigbool* a);

#endif