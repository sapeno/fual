#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    OK,
    ERR_ARGS,      
    ERR_DIGIT,     
    ERR_ALLOC      
} Status;

int digit_val(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    return -1;
}

static char val_digit(int v) {
    return (v < 10) ? (char)('0' + v) : (char)('a' + v - 10);
}

Status correct_num(const char *s, int base) {
    if (!s) return ERR_ARGS;
    if (*s == '\0') return ERR_DIGIT;

    for (const char *p = s; *p; ++p) {
        int d = digit_val(*p);
        if (d < 0 || d >= base) return ERR_DIGIT;
    }
    return OK;
}


Status add_base(const char *a, const char *b, int base, char **out) {
    if (!a || !b || !out) return ERR_ARGS;
    if (base < 2 || base > 36) return ERR_ARGS;

    size_t len_a = strlen(a);
    size_t len_b = strlen(b);
    size_t maxlen = (len_a > len_b ? len_a : len_b) + 1;   

    char *res = malloc(maxlen + 1);
    if (!res) return ERR_ALLOC;

    size_t ia = len_a;   
    size_t ib = len_b;
    size_t ip = maxlen; 
    int carry = 0;

    while (ia > 0 || ib > 0 || carry) {
        int da = 0, db = 0;
        if (ia > 0) da = digit_val(a[--ia]); 
        if (ib > 0) db = digit_val(b[--ib]); 

        int sum = da + db + carry;
        carry = sum / base;
        res[--ip] = val_digit(sum % base);
    }

    size_t start = ip;
    while (start + 1 < maxlen && res[start] == '0') start++;

    size_t len = maxlen - start;
    memmove(res, res + start, len);
    res[len] = '\0';

    *out = res;
    return OK;
}

static Status sum_base(int base, int count, char **result, ...) {
    if (!result) return ERR_ARGS;
    *result = NULL;
    
    if (base < 2 || base > 36) return ERR_ARGS;
    if (count < 0) return ERR_ARGS;

    char *acc = malloc(2);
    if (!acc) return ERR_ALLOC;
    acc[0] = '0'; acc[1] = '\0';

    va_list ap;
    va_start(ap, result);

    Status status = OK;
    for (int i = 0; i < count; ++i) {
        const char *s = va_arg(ap, const char *);

        status = correct_num(s, base);
        if (status != OK) break;

        char *next = NULL;
        status = add_base(acc, s, base, &next);
        free(acc);
        if (status != OK) { acc = NULL; break; }
        acc = next;
    }

    va_end(ap);

    if (status != OK) {
        free(acc);
        return status;
    }

    *result = acc;
    return OK;
}


int main(void) {
    char *res = NULL;
    Status status;

    status = sum_base(10, 3, &res, "10", "20", "-60");
    if (status == OK) { 
        printf("10: 123 + 456 + 789 = %s\n", res); 
        free(res); 
        res = NULL; 
    }
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = sum_base(16, 3, &res, "1a", "2b", "3c");
    if (status == OK) { 
        printf("16: 1a + 2b + 3c = %s\n", res); 
        free(res); 
        res = NULL; 
    }
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = sum_base(2, 3, &res, "101", "11", "1");
    if (status == OK) { 
        printf("2: 101 + 11 + 1 = %s\n", res); 
        free(res); 
        res = NULL; 
    }
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = sum_base(10, 2, &res, "999999999999999999999999999999", "1");
    if (status == OK) { 
        printf("10: 999999999999999999999999999999 + 1 = %s\n", res); 
        free(res); 
        res = NULL; }
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 
    
    status = sum_base(36, 2, &res, "z", "1");
    if (status == OK) { 
        printf("36: z + 1 = %s\n", res); 
        free(res);
        res = NULL; 
        }
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = sum_base(10, 3, &res, "0", "0", "0");
    if (status == OK) { printf("10: 0 + 0 + 0 = %s\n", res);
        free(res); 
        res = NULL; 
    }
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = sum_base(10, 2, &res, "000123", "0077");
    if (status == OK) { 
        printf("10: 000123 + 0077 = %s\n", res); 
        free(res); 
        res = NULL; 
    }
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = sum_base(2, 1, &res, "12a");
    if (status == OK) { printf("10: 12a = %s\n", res); 
        free(res); 
        res = NULL; 
    }
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = sum_base(37, 1, &res, "1");
    if (status == OK) { 
        printf("37: %s\n", res); 
        free(res); 
        res = NULL; }
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = sum_base(10, 1, &res);
    if (status == OK) { printf("10: (empty) = %s\n", res); 
        free(res); 
        res = NULL; 
    }
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    return status;
}