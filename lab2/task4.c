#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define MAX_NAME_LEN 256

typedef enum {
    OK,
    ERR_INPUT,
    ERR_OVERFLOW,
    ERR_ALLOC
} Status;

typedef struct{
    char filename[MAX_NAME_LEN];
    size_t size;
    int importance;
} File;

typedef struct {
    long long max_importance;
    int take;
} Cell;

typedef struct {
    File *selected_files;
    int count;
    long long total_size;
    long long total_importance;
} Result;


Status read_safe_string(char *buffer, int max_len) {
    if (scanf("%255s", buffer) != 1) return ERR_OVERFLOW;
    return OK;
}

Status solve(const File *files, int n, int capacity, Result *result) {

    if (files == NULL || result == NULL) return ERR_INPUT;
    if (n < 0 || capacity < 0) return ERR_INPUT;

    result->selected_files = NULL;
    result->count = 0;
    result->total_size = 0;
    result->total_importance = 0;

    if (n == 0) return OK;

    Cell **buf = (Cell **)malloc((n + 1) * sizeof(Cell *));
    if (buf == NULL) return ERR_ALLOC;

    for (int i = 0; i <= n; i++) {
        buf[i] = (Cell *)calloc(capacity + 1, sizeof(Cell));
        if (buf[i] == NULL) {
            for (int j = 0; j < i; j++) free(buf[j]);
            free(buf);
            return ERR_ALLOC;
        }
    }

    for (int i = 1; i <= n; i++) {
        int current_size = files[i - 1].size;
        int current_imp = files[i - 1].importance;

        for (int w = 0; w <= capacity; w++) {
            buf[i][w] = buf[i - 1][w];
            buf[i][w].take = 0;

            if (current_size <= w) {
                long long val_if_take = buf[i - 1][w - current_size].max_importance + current_imp;
                
                if (val_if_take > buf[i][w].max_importance) {
                    buf[i][w].max_importance = val_if_take;
                    buf[i][w].take = 1;
                }
            }
        }
    }

    result->selected_files = (File *)malloc(n * sizeof(File));
    if (result->selected_files == NULL) {
        for (int i = 0; i <= n; i++) free(buf[i]);
        free(buf);
        return ERR_ALLOC;
    }

    result->total_importance = buf[n][capacity].max_importance;

    int w = capacity;
    for (int i = n; i > 0 && w >= 0; i--) {
        if (buf[i][w].take) {
            result->selected_files[result->count] = files[i - 1];
            result->count++;
            
            w -= files[i - 1].size;
            result->total_size += files[i - 1].size;
        }
    }

    for (int i = 0; i < result->count / 2; i++) {
        File temp = result->selected_files[i];
        result->selected_files[i] = result->selected_files[result->count - 1 - i];
        result->selected_files[result->count - 1 - i] = temp;
    }

    for (int i = 0; i <= n; i++) {
        free(buf[i]);
    }
    free(buf);

    return OK;
}

int main() {
    int n, capacity;
    
    if (scanf("%d %d", &n, &capacity) != 2) {
        fprintf(stderr, "Некорректный ввод\n");
        return ERR_INPUT;
    }

    if (n <= 0 || capacity < 0) {
        fprintf(stderr, "Неккоректное количество файдов или вместимость.\n");
        return ERR_INPUT;
    }

    File *files = (File *)malloc(n * sizeof(File));
    if (files == NULL) {
        fprintf(stderr, "Не удалось выделить память.\n");
        return ERR_ALLOC;    
    }

    for (int i = 0; i < n; i++) {
        if (scanf("%s %zu %d", files[i].filename, &files[i].size, &files[i].importance) != 3) {
            fprintf(stderr, "Некорректный ввод\n");
            free(files);
            return ERR_INPUT;
        }

        if (files[i].size == 0 || files[i].importance < 0) {
            fprintf(stderr, "Некорректный ввод\n");
            free(files);
            return ERR_INPUT;
        }
    }
    Result result;

    Status status = solve(files, n, capacity, &result);
    if (status == OK) {
        printf("TOTAL_SIZE %lld\n", result.total_size);
        printf("TOTAL_IMPORTANCE %lld\n", result.total_importance);
        printf("FILES %d\n", result.count);
        
        for (int i = 0; i < result.count; i++) {
            printf("%s\n", result.selected_files[i].filename);
        }
    } else fprintf(stderr, "Ошибка выполнения.\n");

    free(result.selected_files);
    free(files);

    return status;
}


