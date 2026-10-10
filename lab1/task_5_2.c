#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    OK,
    ERR_ARGC,
    ERR_EPSILON,
    ERR_RANGE,
    ERR_NAN,
    ERR_ITER_LIMIT,
    ERR_UNKNOWN
} Status;

int eq(const double a, const double b, const double eps) {
    return fabs(a - b) < eps;
}

Status correct_double(const char *s, double *result) {
    if (!s || !result) return ERR_UNKNOWN;
    char *end = NULL;
    double val = strtod(s, &end);
    if (end == s || *end != '\0') return ERR_ARGC;
    if (isnan(val) || isinf(val)) return ERR_NAN;
    *result = val;
    return OK;
}

Status correct_epsilon (const char *s, double *eps) {
    Status status = correct_double(s, eps);

    if (status != OK) return status;
    if (*eps <= 0.0 || *eps >= 1.0) return ERR_EPSILON;
    return OK;
}

Status mul(const double a, const double b, double *result) {
    double res = a * b;
    if (isnan(res) || isinf(res)) return ERR_NAN;

    *result = res;
    return OK;
}

Status divn(const double a, const double b, double *result, double eps) {
    if (eq(b, 0.0, eps)) 
        return ERR_NAN;
    double res = a / b;
    if (isnan(res) || isinf(res)) return ERR_NAN;

    *result = res;
    return OK;
}
Status add(const double a, const double b, double *result) {
    double res = a + b;
    if (isnan(res) || isinf(res)) return ERR_NAN;

    *result = res;
    return OK;
}

// int

Status func_a(const double x, double *result) {
    if (x == 0.0) {
        *result = 1.0;
        return OK;
    }
    if (x < 0.0) return ERR_RANGE;
    double ln1_x = log1p(x);
    return divn(ln1_x, x, result, 1e-15);
}

Status func_b(const double x, double *result) {
    double x2;
    Status status = mul(x, x, &x2);
    if (status != OK) return status;

    double half_x2;
    status = divn(x2, 2, &half_x2, 1e-15);
    if (status != OK) return status;

    double res = exp(-half_x2);
    if (isnan(res) || isinf(res)) return ERR_NAN;

    *result = res;
    return OK;
}

Status func_c(const double x, double *result) {
    if (x >= 1.0) return ERR_RANGE;
    if (x == 1.0) return ERR_RANGE;
    double res = -log1p(-x);
    if (isnan(res) || isinf(res)) return ERR_NAN;

    *result = res;
    return OK;
}

Status func_d(const double x, double *result) {
    if (x < 0.0) return ERR_RANGE;
    if (x == 0.0) {
        *result = 1.0;
        return OK;
    }

    double res = exp(x * log(x));
    if (isnan(res) || isinf(res)) return ERR_NAN;

    *result = res;
    return OK;
}

typedef Status (*FuncPtr) (const double, double *);

Status simpson_met(FuncPtr f, const double a, const double b, const double eps, double *result) {
    if (a > b) return ERR_RANGE;
    int n = 2;
    double prev = 0.0;
    double cur = 0.0;
    int iter = 0;

    const int MAX_ITER = 30;

    while(iter < MAX_ITER) {
        double h;
        Status status = divn(b - a, (double)n, &h, eps);
        if (status != OK) return status;

        double f0, fn;
        status = f(a, &f0);
        if (status != OK) return status;
        status = f(b, &fn);
        if (status != OK) return status;

        double sum_odd = 0.0;
        double sum_even = 0.0;

        for (int i = 1; i < n; ++i) {
            double xi;
            status = add(a, (double)i * h, &xi);
            if (status != OK) return status;

            double fi;
            status = f(xi, &fi);
            if (status != OK) return status;

            if (i % 2 == 1) {
                status = add(sum_odd, fi, &sum_odd);
            } else {
                status = add(sum_even, fi, &sum_even);
            }
            if (status != OK) return status;
        }

        double s;
        status = add(f0, fn, &s);
        if (status != OK) return status;

        double odd_4, even_2;
        status = mul(4.0, sum_odd, &odd_4);
        if (status != OK) return status;
        status = mul(2.0, sum_even, &even_2);
        if (status != OK) return status;

        status = add(s, odd_4, &s);
        if (status != OK) return status;
        status = add(s, even_2, &s);
        if (status != OK) return status;

        double h_3;
        status = divn(h, 3.0, &h_3, eps);
        if (status != OK) return status;
        status = mul(h_3, s, &cur);

        iter++;
        if (iter > 1 && fabs(cur - prev) < eps) {
            *result = cur;
            return OK;
        }

        prev = cur;
        n *= 2;
    }
    return ERR_ITER_LIMIT;
}

void print_result(const char *name, const double result, const double eps){
    printf("%s %.15f, eps = %.1e\n", name, result, eps);
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Некорректный ввод аргументов.\n");
        return ERR_ARGC;
    }

    double eps;
    Status status = correct_epsilon(argv[1], &eps);
    if (status != OK) {
        printf("Некорректное epsilon.\n");
        return status;
    }

    printf("Пункт 2.\n");

    {
        double result = 0.0;
        status = simpson_met(func_a, 0.0, 1.0, eps, &result);
        if (status == OK) {
            print_result("a)", result, eps);
        } else {
            printf("Ошибка выполнения.\n");
            return status;
        }
    }

    {
        double result = 0.0;
        status = simpson_met(func_b, 0.0, 1.0, eps, &result);
        if (status == OK) {
            print_result("b)", result, eps);
        } else {
            printf("Ошибка выполнения.\n");
            return status;
        }
    }

    {
        double result = 0.0;
        const double delta = 1e-8;
        status = simpson_met(func_c, 0.0, 1.0 - delta, eps, &result);
        if (status == OK) {
            double correction = delta * (1.0 - log(delta));
            result += correction;
            print_result("c)", result, eps);
        } else {
            printf("Ошибка выполнения.\n");
            return status;
        }
    }

    {
        double result = 0.0;
        status = simpson_met(func_d, 0.0, 1.0, eps, &result);
        if (status == OK) {
            print_result("d)", result, eps);
        } else {
            printf("Ошибка выполнения.\n");
            return status;
        }
    }
    return OK;
}
