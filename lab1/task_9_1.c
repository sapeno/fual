#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>

typedef enum {
    OK,
    ERR_ARGC,
    ERR_PARSE,
    ERR_RANGE,
    ERR_ALLOC
} Status;

Status parse_int(const char *s, int *out) {
    if (!s || !out) return ERR_PARSE;
    char *endptr = NULL;
    long val = strtol(s, &endptr, 10);
    if (endptr == s || *endptr != '\0') return ERR_PARSE;
    if (val < INT_MIN || val > INT_MAX) return ERR_RANGE;
    *out = (int)val;
    return OK;
}

Status randomus(int *arr, size_t n, int a, int b) {
    if (!arr) return ERR_ALLOC;
    if (a > b) return ERR_RANGE;
    unsigned range = (unsigned)b - (unsigned)a + 1u;
    for (size_t i = 0; i < n; ++i) {
        arr[i] = a + (int)(rand() %range);
    }
    return OK;
}

Status swap(int *arr, size_t n) {
    if (!arr || n == 0) return ERR_RANGE;

    size_t ind_min = 0, ind_max = 0;
    for (size_t i = 1; i < n; ++i) {
        if (arr[i] < arr[ind_min]) ind_min = i;
        if (arr[i] > arr[ind_max]) ind_max = i;
    }

    if (ind_max != ind_min) {
        int tmp = arr[ind_min];
        arr[ind_min] = arr[ind_max];
        arr[ind_max] = tmp;
    }
    return OK;
}

void print_arr(const int *arr, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Некорректный ввод аргументов.\n");
        return ERR_ARGC;
    }

    int a = 0, b = 0;
    Status status = parse_int(argv[1], &a);
    if (status != OK) {
        printf("Некорректное a.\n");
        return status;
    }
    status = parse_int(argv[2], &b);
    if (status != OK) {
        printf("Некорректное b.\n");
        return status;
    }
    if (a > b) {
        printf("a должно быть <= b.\n");
        return ERR_RANGE;
    }

    srand((unsigned)time(NULL));
    
    size_t ARR_SZ = 30;
    int arr[ARR_SZ];
    status = randomus(arr, ARR_SZ, a, b);
    if (status != OK) {
        printf("Ошибка выполнения.\n");
        return status;
    }

    printf("До свапа.\n");
    print_arr(arr, ARR_SZ);

    status = swap(arr, ARR_SZ);
    if (status != OK) {
        printf("Ошибка выполнения.\n");
        return status;
    }
    printf("После.\n");
    print_arr(arr, ARR_SZ);

    return OK;
}