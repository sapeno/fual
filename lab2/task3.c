#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

typedef enum {
    OK,
    ERR_ARGS,
    ERR_OVERFLOW,
    ERR_ALLOC, 
    ERR_FILE
} Status;

size_t len(const char *str) {
    
    size_t len = 0;
    while(str[len] != '\0') len++;

    return len;
}

void printing(long long line, long long pos) {
    printf("Строка: %lld, позиция: %lld\n", line, pos);
    return;
}

void no_same() {
    printf("Нет вхождений.\n");
}

int who_is_byt(unsigned char ch) {
    if ((ch & 0x80) == 0x00) return 1;
    if ((ch & 0xE0) == 0xC0) return 2;
    return 1;
}

Status serch(const char *path, const char *str) {
    size_t str_len = len(str);

    if (str_len == 0) {
        return ERR_ARGS;
    }

    FILE *file = fopen(path, "r");

    if (!file) return ERR_FILE;

    size_t cap = str_len + 60000;
    char *buf = (char *)malloc(cap);
    if (buf == NULL) {
        fclose(file);
        return ERR_FILE;
    }

    size_t i = 0;
    size_t have = 0;

    int found = 0;
    long long cur_line = 1;
    long long cur_char = 1;


    for (;;) {
        if (i > 0) {
            memmove(buf, buf + i, have - i);
            have -= i;
            i = 0;
        }
    
        size_t got = fread(buf + have, 1, 60000, file);
        have += got;
        if (ferror(file)) {
            free(buf);
            fclose(file);
            return ERR_FILE;
        }

        int eof = (got == 0);

        size_t limit = (eof) ? have : (have >= str_len - 1 ? have - (str_len - 1) : 0);

        while (i + str_len <= have && i < limit) {
            int same = 1;
            for (size_t j = 0; j < str_len; j++) {
                if (buf[i + j] != str[j]) {
                    same = 0;
                    break;
                }
            }

            if (same) {
                printing(cur_line, cur_char);
                found = 1;
            }

            // if (buf[i] == '\r' && i + 1 < have && buf[i + 1] == '\n') {
            //     i++; 
            if (buf[i] == '\n') {
                cur_line++;
                cur_char = 1;
                i++;
            }else {
                cur_char++;
                int step = who_is_byt((unsigned char)buf[i]);
                if (i + step > have) step = 1; 
                i += step;
            }
        }

        if (eof) break;
    }

    free(buf);
    fclose(file);

    if (found == 0) no_same();

    return OK;
}

Status myFunc(const char *str, int file_count, ...) {
    va_list args;
    va_start(args, file_count);

    Status status = OK;
    for (int i = 0; i < file_count; i++) {
        const char *path = va_arg(args, const char *);
        printf("%s\n", path);
        status = serch(path, str);
        if (status != OK) return status;
    }

    va_end(args);
    return OK;
}


int main() {
    Status status = OK;

    status = myFunc("\n", 15, "linux_test", "tmp");
    if (status != OK) {
        fprintf(stderr, "Ошибка выполнения.\n");
        return status;
    }

    printf("\n\n");
    
    status = myFunc("5", 2, "test1.txt", "test2.txt");
    if (status != OK) {
        fprintf(stderr, "Ошибка выполнения.\n");
        return status;
    }
    return 0; 
}