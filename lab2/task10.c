#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>

typedef enum {
    OK = 0,
    ERR_ARGS,     
    ERR_ALLOC,
    ERR_OVERFLOW

} Status;


Status polinom(double eps, double a, double ** result, int n, ...) {
    if (!result) return ERR_ARGS;
    *result = NULL;

    if (eps <= 0.0) return ERR_ARGS;
    if (n < 0) return ERR_ARGS;

    size_t count = (size_t)n + 1;
    if (count > (size_t)-1 / sizeof(double)) return ERR_ALLOC;

    double *g = calloc(count, sizeof(double));
    if (g == NULL) return ERR_ALLOC;

    va_list args;
    va_start(args, n);

    for (int i = 0; i <= n; ++i) {
        double c = va_arg(args, double);
        if (!isfinite(c)){
            va_end(args);
            free(g);
            return ERR_ARGS;
        }

        g[i] = c;
    }

    va_end(args);

    for (int i = 0; i < n; i++) {
        for (int j = n - 1; j >= i; j--){
            double next = g[j] + a * g[j + 1];
            if (!isfinite(next)) {
                free(g);
                return ERR_OVERFLOW;
            }
            g[j] = next;
        }
    }

    for (int i = 0; i <= n; ++i) {
        if (fabs(g[i]) < eps) {
            g[i] = 0.0;
        }
    }

    *result = g;
    return OK;  
}

void printym(int n, double a, const double *g) {
    printf("При a = %g и степени = %d\n", a, n);
    for (int i = 0; i <= n; i++) {
        printf("g%d = %g\n", i, g[i]);
    }
}

int main(void) {
    double eps = 1e-9;
    double *g = NULL;
    Status status;

    printf("Ввод коэф: f0, f1, ..., fn-1, fn.\n");

    printf("\n");
    status = polinom(eps, 5.0, &g, 0, 1.540);
    if (status == OK) {
        printym(0, 2.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }

    printf("\n");
    status = polinom(eps, 1.0, &g, 1, 2.0, 3.0);
    if (status == OK) {
        printym(1, 1.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }

    printf("\n");
    status = polinom(eps, 1.0, &g, 2, 1.0, -2.0, 1.0);
    if (status == OK) {
        printym(2, 1.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }

    printf("\n");
    status = polinom(eps, 0.0, &g, 2, 5.0, 4.0, 3.0);
    if (status == OK) {
        printym(2, 0.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }

    printf("\n");
    status = polinom(eps, -2.0, &g, 2, -4.0, 0.0, 2.0);
    if (status == OK) {
        printym(2, -2.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }

    /*printf("\n");
    status = polinom(eps, 2.0, &g, 50, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0);
    if (status == OK) {
        printym(50, 2.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }

    printf("\n");
    status = polinom(eps, 1e5, &g, 4, 1.0, 1.0, 1.0, 1.0);
    if (status == OK) {
        printym(4, 1e5, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }*/

    printf("\n");
    status = polinom(-0.0, -2.0, &g, 2, -4.0, 0.0, 2.0);
    if (status == OK) {
        printym(2, -2.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }

    printf("\n");
    status = polinom(-1e-6, -2.0, &g, 2, -4.0, 0.0, 2.0);
    if (status == OK) {
        printym(2, -2.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }

    printf("\n");
    status = polinom(eps, -2.0, &g, -4, -4.0, 0.0, 2.0);
    if (status == OK) {
        printym(2, -2.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }

    printf("\n");
    status = polinom(-0.0, -2.0, NULL, 2, -4.0, 0.0, 2.0);
    if (status == OK) {
        printym(2, -2.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }

    printf("\n");
    status = polinom(1e-7, -1.0, &g, 4, 1.0, 4.0, 6.0, 4.0, 1.0);
    if (status == OK) {
        printym(4, -1.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }

    /*printf("\n");
    status = polinom(1e-5, 1e25, &g, 10, 1e50, 1e50, 1e50, 1e50, 1e50, 1e50, 1e50, 1e50, 1e50, 1e50, 1e50);
    if (status == OK) {
        printym(10, 1e25, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }*/

    printf("\n");
    status = polinom(eps, -2.0, &g, 2, -0.0, -0.0, -0.0);
    if (status == OK) {
        printym(2, -2.0, g);
        free(g); 
        g = NULL;
    } else {
        fprintf(stderr, "Ошибка выполнения %d\n", status);
    }
    return OK;
}