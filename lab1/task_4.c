#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    OK,
    ERR_ARGS,
    ERR_FLAG,
    ERR_FILE_OPEN,
    ERR_FILE_READ,
    ERR_FILE_WRITE
} Status;

int correct_flag(const char *flag, char *action, int *n) {
    if (flag[0] != '-' && flag[0] != '/') return 0;

    const char *p = flag + 1;
    *n = 0;
    if (*p == 'n') {
        *n = 1;
        ++p;
    }

    if (*p == '\0' || *(p + 1) != '\0') return 0;

    char c = *p;
    if (c != 'd' && c != 'i' && c != 's' && c != 'a') return 0;

    *action = c;
    return 1;
}

int is_digit(const int c) {
    return c >= '0' && c <= '9';
}

Status flag_d(FILE *in, FILE *out) {
    int ch;
    while ((ch = fgetc(in)) != EOF) {
        if (!is_digit(ch)) {
            if (fputc(ch, out) == EOF) return ERR_FILE_WRITE;
        }
    }
    if (ferror(in)) return ERR_FILE_READ;
    return OK;
}

long count_letters(const char *s) {
    long count = 0;
        for (const char *p = s; *p; ++p) {
            unsigned char c = (unsigned char)*p;
            if((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
                ++count;
            }
        }
        return count;
}

Status flag_i(FILE *in, FILE *out) {
    char line[4096];
    while (fgets(line, sizeof(line), in) != NULL) {
        long count = count_letters(line);
        if (fprintf(out, "%ld\n", count) < 0)
            return ERR_FILE_WRITE;
    }
    if (ferror(in)) return ERR_FILE_READ;
    return OK;
}

int is_special(const int p) {
    unsigned char c = (unsigned char)p;
    if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) && !(c >= '0' && c <= '9') && !(c == ' '))
        return 1;
    return 0;
}

Status flag_s(FILE *in, FILE *out) {
    long count = 0;
    int c;
    int has_line = 0;

    while ((c = fgetc(in)) != EOF) {
        has_line = 1;
        if (c == '\n') {
            if (fprintf(out, "%ld\n", count) < 0) return ERR_FILE_WRITE;
            count = 0;
            has_line = 0;
        } else {
            if (is_special(c))
                count++;
        }
    }
    if (ferror(in)) return ERR_FILE_READ;

    if (has_line) {
        if (fprintf(out, "%ld\n", count) < 0) return ERR_FILE_WRITE;
    }
    return OK;
}

Status flag_a(FILE *in, FILE *out) {
    int c;
    while ((c = fgetc(in)) != EOF) {
        if (is_digit(c)) {
            if (fputc(c, out) == EOF) return ERR_FILE_WRITE;
        } else {
            if (fprintf(out, "%X", (unsigned char)c) < 0) return ERR_FILE_WRITE;
        }
    }
    if (ferror(in)) return ERR_FILE_READ;
    return OK;
}

Status make_name(const char *in, char *out_buf, size_t buf_size) {
    const char *slash = strrchr(in, '/');
    const char *name = slash ? slash + 1 : in;

    int n = snprintf(out_buf, buf_size, "out_%s", name);
    if (n < 0 || (size_t)n >= buf_size) return ERR_ARGS;
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        return ERR_ARGS;
    }

    char action;
    int n = 0;
    if (!correct_flag(argv[1], &action, &n)) {
        printf("Некорректный флаг.\n");
        return ERR_FLAG;
    }

    const char *in = NULL;
    char out_buf[1024];
    const char *out;

    if (n) {
        if (argc != 4) {
            printf("Для флага с 'n' укажите входной и выходной файл.\n");
            return ERR_ARGS;
        }

        in = argv[2];
        out = argv[3];
        
        if (strcmp(argv[2], argv[3]) == 0) {
            printf("Входной и выходной файл не должны быть одинковыми.\n");
            return ERR_ARGS;
        }
    } else {
        if (argc != 3) {
            printf("Для флага без 'n' укажите входной.\n");
            return ERR_ARGS;
        }
        in = argv[2];
        Status status = make_name(in, out_buf, sizeof(out_buf));
        if (status != OK) {
            printf("Не удолось создать имя выходного файла.\n");
            return status;
        }
        out = out_buf;
    }

    FILE *in_file = fopen(in, "r");
    if (in_file == NULL) {
        printf("Не удалось открыть входной файл.\n");
        return ERR_FILE_OPEN;
    }

    FILE *out_file = fopen(out, "w");
    if (out_file == NULL) {
        fclose(in_file);
        printf("Не удалось открыть выходной файл.\n");
        return ERR_FILE_OPEN;
    }

    Status status = OK;
    switch (action) {
    case 'd' : 
        status = flag_d(in_file, out_file);
        break;
    case 'i' : 
        status = flag_i(in_file, out_file);
        break;
    case 's' : 
        status = flag_s(in_file, out_file);
        break;
    case 'a' : 
        status = flag_a(in_file, out_file);
        break;
    default: 
        printf("Такого флага не существует.\n");
        status = ERR_FLAG;
        break;
    }

    if (fclose(in_file) != 0) status = ERR_FILE_READ;
    if (fclose(out_file) != 0) status = ERR_FILE_WRITE;

    if (status != OK) printf ("Обработка завершилась с ошибкой, код %d.\n", (int)status);
    return status;
}