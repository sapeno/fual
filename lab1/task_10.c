#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

typedef enum {
    OK,
    ERR_ALLOC,
    ERR_BASE,
    ERR_NUM,
    ERR_OVERFLOW,
    ERR_EMPTY_INPUT,
    ERR_IO
} Status;

Status correct_bas(const int base) {
    if (base < 2 || base > 36)
        return ERR_BASE;
    return OK;
}

int correct_num(const char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    return ERR_NUM;
}
