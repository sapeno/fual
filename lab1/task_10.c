#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

typedef enum {
    OK,
    ERR_ALLOC,
    ERR_OVERFLOW,
    ERR_ARGS
} Status;

void print_in_base(long long num, int base) {
    if (num == 0) {
        printf("0");
        return;
    }
    int is_neg = 0;
    if (num < 0) {
        is_neg = 1;
        num = -num;
    }
    char buf[BUFSIZ]; 
    int i = 0;
    while (num > 0) {
        int rem = num % (long long)base;
        if (rem < 10) buf[i++] = '0' + rem;
        else buf[i++] = 'A' + (rem - 10);
        num /= base;
    }
    if (is_neg) printf("-");
    while (i > 0) printf("%c", buf[--i]);
}


Status parse(const char *str, int base, long long *out_val) {
    if (!str || !out_val || base < 2 || base > 36) return ERR_ARGS;
    int start = 0;
    int sign = 1;

    if (str[0] == '-') { 
        sign = -1; 
        start = 1; 
    }

    unsigned long long acc = 0;
    for (int i = start; str[i] != '\0'; ++i) {
        int d = 0;
        char c = str[i];

        if (c >= '0' && c <= '9') d = c - '0';
        else if (c >= 'A' && c <= 'Z') d = c - 'A' + 10;
        else return ERR_ARGS;

        if (d >= base) return ERR_ARGS;
        if (acc > (ULLONG_MAX - d) / base) return ERR_OVERFLOW;
        acc = acc * base + d;
    }

    if (sign == 1 && acc > LLONG_MAX) return ERR_OVERFLOW;

    if (sign == -1 && acc > (unsigned long long)LLONG_MAX + 1) return ERR_OVERFLOW;
    if (sign == -1) {
        if (acc == (unsigned long long)LLONG_MAX + 1) return ERR_OVERFLOW;

        else *out_val = -(long long)acc;
    } else {
        *out_val = (long long)acc;
    }

    return OK;
}

char* read_dynamic(void) {
    size_t cap = 16; 
    size_t len = 0;
    char *buf = malloc(cap * sizeof(char));
    if (!buf) return NULL;

    int c = getchar();
    while (c != EOF && isspace(c)) {
        c = getchar();
    }
    if (c == EOF) {
        free(buf);
        return NULL;
    }

    while (c != EOF && !isspace(c)) {
        if (len + 1 >= cap) {
            cap *= 2;
            char *new_buf = realloc(buf, cap);
            if (!new_buf) {
                free(buf);
                return NULL;
            }
            buf = new_buf;
        }
        buf[len++] = (char)c;
        c = getchar();
    }
    buf[len] = '\0';

    return buf;
}

int main() {
    int base;
    printf("Введите основание СС (2-36): ");
    if (scanf("%d", &base) != 1 || base < 2 || base > 36) {
        fprintf(stderr, "Неверное основание.\n");
        return 1;
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }

    long long max_val = 0;
    long long sum = 0;
    int count = 0;

    printf("Вводите числа (для завершения введите Stop):\n");

    while (1) {
        char *buf = read_dynamic();
        if (!buf) break;

        if (strcmp(buf, "Stop") == 0) {
            free(buf);
            break;
        }

        long long val;
        Status status = parse(buf, base, &val);

        if (status != OK) {
            fprintf(stderr, "Ошибка выполнения: %d.\n", status);
            free(buf);
            continue; 
        }

        free(buf);

        long long abs_val = (val < 0) ? -val : val;
        if ((val > 0 && sum > LLONG_MAX - val) || (val < 0 && sum < LLONG_MIN - val)) {
            fprintf(stderr, "Ошибка выполнения\n");
            return ERR_OVERFLOW;
        }

        sum += val;

        if (count == 0 || abs_val > max_val) {
            max_val = abs_val;
        }
        count++;
    }

    if (count == 0) {
        printf("Числа не введены.\n");
        return 0;
    }

    printf("\nМаксимальное по модулю: ");
    print_in_base(max_val, base);
    printf("\n");

    printf("Сумма: ");
    print_in_base(sum, base);
    printf("\n");

    int bases[] = {9, 18, 27, 36};
    printf("\nПредставления в других СС:\n");

    for (int i = 0; i < 4; ++i) {
        printf("\n%d-чная\n", bases[i]);
        printf("Максимум: "); print_in_base(max_val, bases[i]); printf("\n");
        printf("Сумма:    "); print_in_base(sum, bases[i]); printf("\n");
    }

}