#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum {
    OK,
    ERR_ALLOC,
    ERR_RANGE
} Status;

static size_t random_size(int l, int h) {
    return (size_t)(l + rand() % (h - l + 1));
}

Status randomus(int *arr, size_t n, int l, int h) {
    if (!arr) return ERR_ALLOC;
    if (l > h) return ERR_RANGE;
    unsigned range = (unsigned)h - (unsigned)l + 1u;
    for (size_t i = 0; i < n; ++i) {
        arr[i] = l + (int)(rand() %range);
    }
    return OK;
}

int find_nearest(const int *b, size_t nb, int value) {
    int best = b[0];
    long best_diff = labs((long)value - (long)b[0]);
    for (size_t i = 1; i < nb; ++i) {
        long diff = labs((long)value - (long)b[i]);
        if (diff < best_diff) {
            best_diff = diff;
            best = b[i];
        }
    }
    return best;
}

Status make_c(const int *a, size_t na, const int *b, size_t nb, int **out_c, size_t *out_n) {
    if (!a || !b || !out_c || !out_n) return ERR_ALLOC;

    int *c = (int *)malloc(na * sizeof(int));
    if (!c) return ERR_ALLOC;

    for (size_t i = 0; i < na; ++i) {
        int near = find_nearest(b, nb, a[i]);
        c[i] = a[i] + near;
    }

    *out_c = c;
    *out_n = na;
    return OK;
}

void free_all(int *a, int *b, int *c) {
    free(a);
    free(b);
    free(c);
}

void print_arr(const int *arr, size_t n) {
    for (size_t i = 0; i < n; ++i) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}

int main(void) {
    srand((unsigned)time(NULL));

    size_t na = random_size(10, 10000);
    size_t nb = random_size(10, 10000);

    int *a = (int *)malloc(na * sizeof(int));
    if (!a) {
        printf("Не удалось выделить память.\n");
        return ERR_ALLOC;
    }

    int *b = (int *)malloc(nb * sizeof(int));
    if (!b) {
        printf("Не удалось выделить память.\n");
        return ERR_ALLOC;
    }

    Status status = randomus(a, na, -1000, 1000);
    if (status != OK) { 
        free_all(a, b, NULL); 
        return status; 
    }

    status = randomus(b, nb, -1000, 1000);
    if (status != OK) { 
        free_all(a, b, NULL); 
        return status; 
    }

    int *c = NULL;
    size_t nc = 0;
    status = make_c(a, na, b, nb, &c, &nc);
    if (status != OK) { 
        free_all(a, b, NULL); 
        return status; 
    }

    printf("A\n");
    print_arr(a, na);

    printf("B\n");
    print_arr(b, nb);

    printf("C\n");
    print_arr(c, nc);

    free_all(a, b, c);
    return OK;
}