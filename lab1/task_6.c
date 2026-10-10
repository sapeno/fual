#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>
#include <math.h>

typedef enum {
    OK,
    ERR_ARGS,
    ERR_FLAG,
    ERR_RANGE, 
    ERR_ZERO,
    ERR_EPSILON,
    ERR_NUM,
    ERR_ALLOC
} Status;

Status correct_epsilon (const double eps) {
    return (eps > 0.0 && eps < 1.0) ? OK : ERR_EPSILON;
}

int eq(double a, double b, const double eps) {
    return fabs(a - b) < eps;
}

int is_convex_polygon(int *result, double eps, const int n, ...) {
    if (n < 3) return ERR_ARGS;
    if (correct_epsilon(eps) != OK) return ERR_EPSILON;

    double *x = (double *)malloc(sizeof(double) * n);
    double *y = (double *)malloc(sizeof(double) * n);

    if (x == NULL || y == NULL) {
        free(x);
        free(y);
        return ERR_ALLOC;
    }

    va_list p;
    va_start(p, n);
    for (int i = 0; i < n; ++i) {
        x[i] = va_arg(p, double);
        y[i] = va_arg(p, double);
    }
    va_end(p);

    int sign = 0;
    int convex;

    for (int i = 0; i < n; i++) {
        int j = (i + 1) % n;
    }
}