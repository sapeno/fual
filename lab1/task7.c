#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>

typedef enum {
    OK = 0,
    ERR_ARGS,
    ERR_FILE,
    ERR_ALLOC,
    ERR_MEMORY
} Status;

typedef struct {
    char *word;
} Lexeme;

typedef struct {
    Lexeme *items;
    size_t count;
    size_t capacity;
} LexemeArray;

void init_lexeme_array(LexemeArray *arr) {
    arr->items = NULL;
    arr->count = 0;
    arr->capacity = 0;
}

void free_lexeme_array(LexemeArray *arr) {
    if (arr->items) {
        for (size_t i = 0; i < arr->count; ++i) {
            free(arr->items[i].word);
        }
        free(arr->items);
    }
    init_lexeme_array(arr);
}

Status push_lexeme(LexemeArray *arr, const char *word, size_t len) {
    if (arr->count >= arr->capacity) {
        size_t new_cap = (arr->capacity == 0) ? 16 : arr->capacity * 2;
        Lexeme *new_items = realloc(arr->items, new_cap * sizeof(Lexeme));
        if (!new_items) return ERR_ALLOC;
        arr->items = new_items;
        arr->capacity = new_cap;
    }
    
    char *new_word = malloc(len + 1);
    if (!new_word) return ERR_ALLOC;
    
    strncpy(new_word, word, len);
    new_word[len] = '\0';
    
    arr->items[arr->count].word = new_word;
    arr->count++;
    return OK;
}

Status read_lexemes(const char *filename, LexemeArray *arr) {
    FILE *f = fopen(filename, "r");
    if (!f) return ERR_FILE;

    init_lexeme_array(arr);
    int c;

    size_t cap = 32;
    size_t len = 0;
    char *buf = malloc(cap * sizeof(char));
    if (buf == NULL) {
        fclose(f);
        return ERR_ALLOC;
    }

    while ((c = fgetc(f)) != EOF) {
        if (isspace((unsigned char)c)) {
            if (len > 0) {
                Status status = push_lexeme(arr, buf, len);
                if (status != OK) {
                    free(buf); 
                    fclose(f); 
                    return status; 
                }
                len = 0;
            }
        } else {
            if (len + 1 >= cap) {
                cap *= 2;
                char *new_buf = realloc(buf, cap);
                if (new_buf == NULL) {
                    free(buf);
                    fclose(f);
                    return ERR_ALLOC;
                }
                buf = new_buf;
            }
            buf[len++] = (char)c;
        }
    }
    if (len > 0) {
        Status status = push_lexeme(arr, buf, len);
        if (status != OK) {
            free(buf); 
            fclose(f); 
            return status; 
        }
    }

    free(buf);
    fclose(f);
    return OK;
}

Status write_lexemes(const char *filename, const LexemeArray *arr) {
    FILE *f = fopen(filename, "w");
    if (!f) return ERR_FILE;

    for (size_t i = 0; i < arr->count; ++i) {
        fprintf(f, "%s", arr->items[i].word);
        if (i < arr->count - 1) fprintf(f, " ");
    }
    fprintf(f, "\n");

    fclose(f);
    return OK;
}

void to_base_str(int num, int base, char *out) {
    char tmp[BUFSIZ];
    int i = 0;
    if (num == 0) { 
        strcpy(out, "0"); 
        return; 
    }
    while (num > 0) {
        int rem = num % base;
        tmp[i++] = (rem < 10) ? ('0' + rem) : ('A' + rem - 10);
        num /= base;
    }
    for (int j = 0; j < i; ++j) 
        out[j] = tmp[i - 1 - j];
    out[i] = '\0';
}

void lexeme_a(char **word_ptr, size_t pos) {
    char *word = *word_ptr;
    size_t len = strlen(word);
    
    char *tmp = malloc((len * 8 + 1) * sizeof(char));
    if (!tmp) return;
    tmp[0] = '\0';

    if (pos % 10 == 0) {
        for (size_t j = 0; j < len; ++j) {
            char c = (char)tolower((unsigned char)word[j]);
            char code[BUFSIZ]; 
            to_base_str((int)c, 4, code);
            strcat(tmp, code);
        }
    } else if (pos % 2 == 0) {
        strcpy(tmp, word);
        for (size_t j = 0; j < len; ++j) {
            tmp[j] = (char)tolower((unsigned char)tmp[j]);
        }
    } else if (pos % 5 == 0) {
        for (size_t j = 0; j < len; ++j) {
            char code[BUFSIZ];
            to_base_str((int)word[j], 8, code);
            strcat(tmp, code);
        }
    } else {
        strcpy(tmp, word);
    }

    free(word);
    *word_ptr = tmp;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "флаг r, file1, file2, out или флаг a, in, out\n");
        return ERR_ARGS;
    }

    const char *flag = argv[1];
    if (flag[0] != '-' && flag[0] != '/') {
        fprintf(stderr, "Ошибка: флаг должен начинаться с '-' или '/'.\n");
        return ERR_ARGS;
    }

    if (strcmp(flag, "-r") == 0) {
        if (argc != 5) {
            fprintf(stderr, "Некорректный ввод.\n");
            return ERR_ARGS;
        }

        struct stat p1, p2, p3;
        if (stat(argv[2], &p1) != 0 || stat(argv[3], &p2) || stat(argv[4], &p3)) {
            fprintf(stderr, "Файла не существует.\n");
            return ERR_ARGS;
        }
        if ((p1.st_dev == p3.st_dev && p1.st_ino == p3.st_ino) || (p2.st_dev == p3.st_dev && p2.st_ino == p3.st_ino)) {
            fprintf(stderr, "Файлы одинаковые, меняй.\n");
            return ERR_ARGS;
        }
        
        LexemeArray arr1, arr2, result;
        Status status = read_lexemes(argv[2], &arr1);
        if (status != OK) { 
            fprintf(stderr, "Ошибка чтения %s\n", argv[2]); 
            return status; 
        }
        
        status = read_lexemes(argv[3], &arr2);
        if (status != OK) { 
            fprintf(stderr, "Ошибка чтения %s\n", argv[3]); 
            free_lexeme_array(&arr1); 
            return status; 
        }

        init_lexeme_array(&result);
        size_t max_len = (arr1.count > arr2.count) ? arr1.count : arr2.count;
        
        for (size_t i = 0; i < max_len; ++i) {
            if (i < arr1.count) {
                push_lexeme(&result, arr1.items[i].word, strlen(arr1.items[i].word));
            }
            if (i < arr2.count) {
                push_lexeme(&result, arr2.items[i].word, strlen(arr2.items[i].word));
            }
        }

        status = write_lexemes(argv[4], &result);
        if (status != OK)   
            fprintf(stderr, "Ошибка записи в %s\n", argv[4]);

        free_lexeme_array(&arr1);
        free_lexeme_array(&arr2);
        free_lexeme_array(&result);
        return status;
    }else if (strcmp(flag, "-a") == 0) {
        if (argc != 4) {
            fprintf(stderr, "Некорректный ввод.\n");
            return ERR_ARGS;
        }

        struct stat p1, p2;
        if (stat(argv[2], &p1) != 0 || stat(argv[3], &p2) != 0) {
            fprintf(stderr, "Файла не существует.\n");
            return ERR_ARGS;
        }

        if (p1.st_dev == p2.st_dev && p1.st_ino == p2.st_ino) {
            fprintf(stderr, "Файлы одинаковые, меняй.\n");
            return ERR_ARGS;
        }

        LexemeArray arr;
        Status status = read_lexemes(argv[2], &arr);
        if (status != OK) { 
            fprintf(stderr, "Ошибка чтения %s\n", argv[2]); 
            return status; 
        }

        for (size_t i = 0; i < arr.count; ++i) {
            lexeme_a(&arr.items[i].word, i + 1);
        }

        status = write_lexemes(argv[3], &arr);
        if (status != OK) 
            fprintf(stderr, "Ошибка записи в %s\n", argv[3]);

        free_lexeme_array(&arr);
        return status;
    }

    fprintf(stderr, " Нет такого флага\n");
    return ERR_ARGS;
}