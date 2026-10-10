#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef enum {
    OK,
    ERR_ARGS,
    ERR_FLAG,
    ERR_RANGE,
    ERR_NO_VAL,
    ERR_OVERFLOW
} Status;


Status correct_num(const char *str, long long *result) {
    if (!str || *str == '\0') return ERR_ARGS;
    char *endptr = NULL;
    long long val = strtoll(str, &endptr, 10);
    if (endptr == str || *endptr != '\0') return ERR_ARGS;
    *result = val;
    return OK;

}

Status h_task(const long long x, int *buf, int *count) {
    if (x <= 0) return ERR_ARGS;

    *count = 0;
    for (int i = 1; i <= 100; i++) {
        if (i % x == 0)
            buf[(*count)++] = i; 
    }
    return (*count > 0) ? OK : ERR_NO_VAL;
}

Status p_task(const long long x, int *result) {
    if (x < 2){
        *result = 0;
        return OK;
    }

    *result = 1;
    for (long long i = 2; i * i <= x; i++) {
        if (x % i == 0){
            *result = 0;
            return OK;
        }
    }
    return OK;
}

Status s_task(const long long x, char *hex_str, size_t max_len) {
    if (x < 0) return ERR_ARGS;

    char tmp[32];
    int len = snprintf(tmp, sizeof(tmp), "%llX", x );

    size_t need_len = (size_t)len + (len > 0 ? (size_t)(len - 1) : 0) + 1;

    if (max_len < need_len) return ERR_ARGS;

    char *p = hex_str;
    for (int i = 0; i < len; ++i) {
        *p++ = tmp[i];
        if (i < len - 1) *p++ = ' ';
    }
    *p = '\0';
    return OK;
}

Status e_task(const long long x, long long result[10][10]) {
    if (x < 1 || x > 10) return ERR_RANGE;

    for (int base = 1; base <= 10; ++base) {
        long long cur = 1;
        for (int exp = 1; exp <= x; ++exp) {
            if (cur > LLONG_MAX / base) return ERR_OVERFLOW;

            cur *= base;
            result[base - 1][exp - 1] = cur;
        }
    }
    return OK;
}

Status a_task(const long long x, long long *result) {
    if (x < 1) return ERR_RANGE;

    *result = 0;
    for (long long i = 1; i <= x; i++) {
        if (*result > LLONG_MAX - i) return ERR_OVERFLOW;
        *result += i;
    }
    return OK;
}

Status f_task(const long long x, long long *result) {
    if (x < 0) return ERR_ARGS;
    if (x == 0) {
        *result = 1;
        return OK;
    }
    
    *result = 1;
    for (long long i = 2; i <= x; ++i) {
        if (*result > LLONG_MAX / i) return ERR_OVERFLOW;
        *result *= i;
    }
    return OK;
}

void print_h(const int *buf, int count) {
    for (int i = 0; i < count; i++) {
        printf("%d ", buf[i]);
    }
     printf("\n");
}

void print_p(const long long x, int is_prime) {
    if (x < 2) {
        printf("Число %lld не является ни простым, ни составным.\n", x);
    } else if (is_prime) {
        printf("Число %lld — простое.\n", x);
    } else {
        printf("Число %lld — составное.\n", x);
    }
}

void print_e(const long long result[10][10], const long long x) {
    for (long long  i = 0; i < 10; ++i) {
        for (long long j = 0; j < x; j++) {
            printf("%lld ", result [i][j]);
        }
        printf("\n");
    }
}



int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Неверное количество аргументов.\n");
        return ERR_ARGS;
    }

    long long x = 0;
    if (correct_num(argv[1], &x) != OK) {
        fprintf(stderr, "Неверный формат числа x.\n");
        return ERR_ARGS;
    }

    const char *flag = argv[2];
    if (flag[0] != '-' && flag[0] != '/') {
        fprintf(stderr, "Флаг должен начинаться с '-', '/'. \n");
        return ERR_ARGS;
    }

    char action = flag[1];
    switch(action) {
        case 'h': {
            int buf[100];
            int count = 0;
            Status status = h_task(x, buf, &count);
            if (status == OK){
                print_h(buf, count);
            } else if (status == ERR_NO_VAL) {
                printf("Нет кратных чисел в пределах 100.\n");
            }else {
                fprintf(stderr, "Ошибка диапазона. \n");
                return ERR_RANGE;
            }
            break;
        }
        case 'p': {
            int result;
            p_task(x, &result);
            print_p(x, result);
            break;
        }
        case 's': {
            char hex_str[64];
            if (s_task(x, hex_str, sizeof(hex_str)) == OK) {
                printf("%s\n", hex_str);
            } else {
                fprintf(stderr, "Ошибка конвертациию\n");
                return ERR_RANGE;
            }
            break;
        }
        case 'e': {
            long long result[10][10] = {0};
            Status status = e_task(x, result);
            if (status == ERR_RANGE) {
                fprintf(stderr, "Для флага -e число x должно быть от 1 до 10.\n");
                return ERR_RANGE;
            } else if (status == ERR_OVERFLOW) {
                fprintf(stderr, "Переполнение во время вычислений.\n");
                return ERR_OVERFLOW;
            }
            print_e(result, (int)x);
            break;
        }
        case 'a': {
            long long result = 0;
            Status status = a_task(x, &result);
            if (status == ERR_OVERFLOW) {
                fprintf(stderr, "Переполнение суммы.\n");
                return ERR_OVERFLOW;
            } else if (status == ERR_RANGE) {
                fprintf(stderr, "Число должно быть натуральным.\n");
                return ERR_RANGE;
            }
            printf("%lld\n", result);
            break;
        }
        case 'f': {
            long long result = 0;
            Status status = f_task(x, &result);
            if (status == ERR_OVERFLOW) {
                fprintf(stderr, "Переполнение факториала.\n");
                return ERR_OVERFLOW;
            } else if (status == ERR_RANGE) {
                fprintf(stderr, "Некорректный диапазон.\n");
                return ERR_RANGE;
            }
            printf("%lld\n", result);
            break;
        }
        default : {
            printf ("Такого флага не существует.\n");
        return ERR_FLAG;
        }
    }
    return OK;
}


