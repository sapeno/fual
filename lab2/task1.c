// не юзать стрингх
#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdint.h>

typedef enum {
    OK,
    ERR_FLAG,
    ERR_ARGS,
    ERR_OVERFLOW,
    ERR_NUM, 
    ERR_STRING, 
    ERR_ALLOC
} Status;

Status correct_flag(const char *flag, char *result) {
    if (flag == NULL) return 0;
    if (flag[0] != '-') return ERR_FLAG;
    if (flag[1] == '\0' || flag[2] != '\0') return ERR_FLAG;
    *result = flag[1];
    return OK;
}

Status correct_uint(const char *str, unsigned int *result) {
    if (!str || *str == '\0' || !result) return ERR_ARGS;
    if (str[0] == '-') return ERR_ARGS;
    char *endptr;
    unsigned long val = strtoul(str, &endptr, 10);
    if (endptr == str ||*endptr != '\0') return ERR_ARGS;
    if (val > UINT_MAX) return ERR_ARGS;
    *result = (unsigned int)val;
    return OK;
}

Status correct_str(char *str) {
    if (!str || *str == '\0' || isspace(*str)) return ERR_ARGS;
    while (*str) {
        if ((unsigned char)*str == 0xD0 || (unsigned char)*str == 0xD1) {
            return ERR_ARGS;
        }
        str++;
    }
    return OK;
}

Status flag_l(const char *str, size_t *result) {
    if (!str || !result) return ERR_ARGS;
    
    size_t len = 0;
    while(str[len] != '\0'){
        if (len + 1 > SIZE_MAX) return ERR_OVERFLOW;
        len++;
    }

    *result = len;
    return OK;
}

Status flag_r(const char *str, char *result, int len) {
    if (!str || !result || len < 0) return ERR_ARGS;

    for (int i = 0; i < len; i++) {
        result[i] = str[len - 1 - i];
    }
    result[len] = '\0';
    return OK;
}

Status flag_u(const char *str, char *result, int len) {
    if (!str || !result || len < 0) return ERR_ARGS;

    for (int i = 0; i < len; i++) {
        if (i % 2 != 0 && str[i] >= 'a' && str[i] <= 'z') {
            result[i] = toupper(str[i]);
        } else result[i] = str[i];
    }
    result[len] = '\0';
    return OK;
}

Status flag_n(const char *str, char *result, int len) {
    if (!str || !result || len < 0) return ERR_ARGS;

    int n = 0;
    for (int i = 0; i < len; i++) if (isdigit(str[i])) result[n++] = str[i];
    
    for (int i = 0; i < len; i++) if (isalpha(str[i])) result[n++] = str[i];

    for (int i = 0; i < len; i++) if (!isalnum(str[i])) result[n++] = str[i];

    result[n] = '\0';
    return OK;
}

Status flag_c(unsigned int seed, char **strs, int count, char *result, size_t res_sz) {
    if (!result || res_sz == 0) return ERR_ARGS;
    if (count < 0 || !strs) return ERR_ARGS;

     if (count == 0) {
        result[0] = '\0';
        return OK;
    }

    size_t total_len = 0;
    for (int i = 0; i < count; i++) {
        if (!strs[i]) return ERR_ARGS;
        size_t len = 0;
        flag_l(strs[i], &len);
        total_len += len;   
    }

    if (total_len + 1 > res_sz) return ERR_OVERFLOW;

    int *order = (int *)malloc((size_t)count * sizeof(int));
    int *used  = (int *)calloc((size_t)count, sizeof(int));
    if (!order || !used) {
        free(order); free(used); 
        return ERR_ALLOC;
    }

    srand(seed);
    for (int i = 0; i < count; i++) {
        int idx;
        do {
            idx = rand() % count;
        } while (used[idx]);
        used[idx] = 1;
        order[i] = idx;
    }

    int pos = 0;
    for (int i = 0; i < count; i++) {
        const char *src = strs[order[i]];
        for (int j = 0; src[j]; j++) {
            result[pos++] = src[j];
        }
    }
    result[pos] = '\0';

    free(order);
    free(used);
    return OK;

}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Некорректный ввод.\n");
        return ERR_ARGS;
    }

    Status status = correct_str(argv[2]);
    char *str;
    if (status == OK) str = argv[2];
    else return status;


    char flag;
    status = correct_flag(argv[1], &flag);
    if ( status != OK) {
        printf("Неверный флаг.\n");
        return status;
    }

    if (flag == 'c') {
        if (argc < 4) {
            fprintf(stderr, "Некорректный ввод для флага c.\n");
            return ERR_ARGS;
        }

        unsigned int seed;
        status = correct_uint(argv[3], &seed);
        if (status != OK) {
            fprintf(stderr, "Неккоректное значение для seed.\n");
            return status;
        }

        int count = 1 + (argc - 4);
        char **strs = (char **)malloc(sizeof(char *) * (size_t)count);;
        if (!strs) {
            fprintf(stderr, "Не удалось выделить память.\n");
            return ERR_ALLOC;
        }

        strs[0] = str;              
        for (int i = 4; i < argc; i++) {
            strs[i - 3] = argv[i];  
        }

        size_t total_len = 0;
        for (int i = 0; i < count; i++) {
            size_t len = 0;
            flag_l(strs[i], &len);
            total_len += len;
        }

        if (total_len + 1 > SIZE_MAX) {
            fprintf(stderr, "Переполнение.\n");
            return ERR_OVERFLOW;
        }

        char *result = (char *)malloc(total_len + 1);
        if (!result) {
            fprintf(stderr, "Не удалось выделить память.\n");
            return ERR_ALLOC;
        }

        status = flag_c(seed, strs, count, result, total_len + 1);
        if (status == OK)
            printf("%s\n", result);
        else 
            fprintf(stderr, "Ошибка при выполнении.\n");

        free(result);
        return status;
    }
    else {
        if (argc != 3) {
            fprintf(stderr, "Некорректный ввод.\n");
            return ERR_ARGS;
        }

        size_t len = 0;
        status = flag_l(str, &len);
        if (status != OK) return status;

        if (flag == 'l') {
            printf("%zu\n", len);
            return OK;
        }

        if (len + 1 > SIZE_MAX) {
            fprintf(stderr, "Переполнение.\n");
            return ERR_OVERFLOW;
        }

        char *result = (char *)malloc(len + 1);
        if (!result) {
            fprintf(stderr, "Не удалось выделить память.\n");
            return ERR_ALLOC;
        }

        switch (flag){
        case 'r':
            status = flag_r(str, result, (int)len);
            break;
        case 'u':
            status = flag_u(str, result,(int)len);
            break;
        case 'n':
            status = flag_n(str, result,(int)len);
            break;
        default:
            printf ("Такого флага не существует.\n");
            free(result);
            return ERR_FLAG;
            break;
        }

        if (status == OK){
            printf("%s\n", result);
        } else {
            fprintf(stderr, "Ошибка выполнения");
        }

        free(result);
        return status;

    }

}
