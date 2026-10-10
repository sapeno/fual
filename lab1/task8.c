#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>

typedef enum {
    OK,
    ERR_OVERFLOW,
    ERR_ARGS, 
    ERR_FILE
} Status;

int digit_value(int ch) {
    if (ch >= '0' && ch <= '9') return ch - '0';
    if (ch >= 'a' && ch <= 'z') return ch - 'a' + 10;
    if (ch >= 'A' && ch <= 'Z') return ch - 'A' + 10;
    return -1;
}

int min_base(const char *num) {
    int max_digit = 0;
    for (const char *p = num; *p; ++p) {
        int d = digit_value((unsigned char)*p);
        if (d < 0) return -1;
        if (d > max_digit) max_digit = d;
    }
    int base = max_digit + 1;
    return base < 2 ? 2 : base;
}

void scips_zeros(const char *num, char *out, size_t out_size) {
    const char *p = num;
    while (*p == '0' && *(p + 1) != '\0') ++p;
    strncpy(out, p, out_size - 1);
    out[out_size - 1] = '\0';
}

int to_decimal(const char *num, int base, unsigned long long *out) {
    unsigned long long acc = 0;
    for (const char *p = num; *p; ++p) {
        int d = digit_value((unsigned char)*p);
        if (d < 0 || d >= base) return 0;
        if (acc > (ULLONG_MAX - (unsigned long long)d) / (unsigned long long)base)
            return 0;
        acc = acc * (unsigned long long)base + (unsigned long long)d;
    }
    *out = acc;
    return 1;
}

int read_num(FILE *file_in, char *num, size_t size) {
    int c;
    while ((c = fgetc(file_in)) != EOF && isspace((unsigned char)c)) {}
    if (c == EOF) return 0;

    size_t len = 0;
    do {
        if (len + 1 >= size) {
            while ((c = fgetc(file_in)) != EOF && !isspace((unsigned char)c)) { }
            return -1;
        }
        num[len++] = (char)c;
        c = fgetc(file_in);
    } while (c != EOF && !isspace((unsigned char)c));
    num[len] = '\0';
    return 1;
}

Status do_num(const char *num, FILE *fout) {
    int base = min_base(num);
    if (base < 0) {
        fprintf(stderr, "Недопустимый символ в токене '%s'\n", num);
        return ERR_ARGS;
    }

    char clean[BUFSIZ];
    scips_zeros(num, clean, sizeof clean);

    unsigned long long dec = 0;
    if (!to_decimal(clean, base, &dec)) {
        fprintf(stderr, "Переполнение при переводе токена '%s' (основание %d)\n", clean, base);
        return ERR_OVERFLOW;
    }

    fprintf(fout, "%s %d %llu\n", clean, base, dec);
    return OK;
}

Status foo(FILE *file_in, FILE *file_out) {
    char tok[BUFSIZ];
    int r;
    while ((r = read_num(file_in, tok, sizeof tok)) != 0) {
        if (r < 0) {
            fprintf(stderr, "Слишком длинное число\n");
            return ERR_OVERFLOW;
        }
        Status status = do_num(tok, file_out);
        if (status != OK) return status;
    }
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Входной файл, выходной файл\n");
    }

    struct stat p1, p2;
    if (stat(argv[1], &p1) != 0) return 0;
    if (stat(argv[2], &p2) != 0) return 0;
    if (p1.st_dev == p2.st_dev && p1.st_ino == p2.st_ino) {
        fprintf(stderr, "Файлы одинаковые, меняй.\n");
        return ERR_ARGS;
    }

    FILE *file_in = fopen(argv[1], "r");
    if (!file_in) {
        fprintf(stderr, "Не удалось открыть файл\n");
        return ERR_FILE;
    }
    FILE *file_out = fopen(argv[2], "w");
    if (!file_out) {
        fprintf(stderr, "Не удалось открыть файл\n");
        return ERR_FILE;

    }

    Status status = foo(file_in, file_out);
    if (status != OK) {
        fprintf(stderr, "Ошибка выполнения\n");
        return status;
    }

    fclose(file_in);
    fclose(file_out);

    return OK;
}