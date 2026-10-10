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
    char *endptr = NULL;
    double val = strtod(s, &endptr);
    if (endptr == s || *endptr != '\0') return ERR_ARGC;
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

Status sum_a(const double x, double eps, double *result) {
    double term = 1.0;
    double sum = 1.0;
    int n = 0;
    while (fabs(term) >= eps && n < 10000000) {
        n++;
        double tmp = 0.0;
        Status status = mul(term, x, &tmp);
        if (status != OK) return status;

        status = divn(tmp, (double)n, &term, eps);
        if (status != OK) return status;

        status = add(sum, term, &sum);
        if (status != OK) return status;
    }

    if (n >= 10000000) return ERR_ITER_LIMIT;

    *result = sum;
    return OK;
}

Status sum_b(const double x, double eps, double *result) {
    int n = 0;
    double term = 1.0;
    double sum = 1.0;

    double x2 = 0.0;
    Status status = mul(x, x, &x2);
    if (status != OK) return status;

    while (fabs(term) >= eps && n < 10000000) {
        n++;
        double denom = (double)(2 * n - 1) * (double)(2 * n);
        double tmp;

        status = mul(term, -x2, &tmp);
        if(status != OK) return status;

        status = divn(tmp, denom, &term, eps);
        if (status != OK) return status;

        status = add(sum, term, &sum);
        if (status != OK) return status;
    }
    if (n >= 10000000) return ERR_ITER_LIMIT;

    *result = sum;
    return OK;
}

Status sum_c(const double x, const double eps, double *result) {
    int n = 0;
    double sum = 1.0;
    double term = 1.0;

    double x2 = 0.0;
    Status status = mul(x, x, &x2);
    if (status != OK) return status;

    while (fabs(term) >= eps && n < 10000000) {
        n++;
        double n3 = (double)n * (double)n * (double)n;
        double num;

        status = mul(27.0, n3, &num);
        if (status != OK) return status;
        status = mul(num, x2, &num);
        if (status != OK) return status;

        double denom = (double)(3 * n - 2) * (double)(3 * n - 1) * (double)(3 * n);

        double tmp;
        status = mul(term, num, &tmp);
        if (status != OK) return status;

        status = divn(tmp, denom, &term, eps);
        if (status != OK) return status;

        status = add(sum, term, &sum);
        if (status != OK) return status;
    }
    if (n >= 10000000) return ERR_ITER_LIMIT;

    *result = sum;
    return OK;
}

Status sum_d(const double x, const double eps, double *result) {
    double x2 = 0.0;
    Status status = mul(x, x, &x2);
    if (status != OK) return status;

    double term;
    status = divn(-x2, 2.0, &term, eps);
    if (status != OK) return status;

    double sum = term;
    int n = 1;

    while (fabs(term) >= eps && n < 10000000) {
        n++;
        double num;
        status = mul(-x2, (double)(2 * n - 1), &num);
        if (status != OK) return status;

        double tmp;
        status = mul(term, num, &tmp);
        if (status != OK) return status;

        status = divn(tmp, (double)(n * 2), &term, eps);
        if (status != OK) return status;

        status = add(sum, term, &sum);
        if (status != OK) return status;
    }

    if (n >= 10000000) return ERR_ITER_LIMIT;

    *result = sum;
    return OK;
}

void print_result(const char *name, const double result, const double eps){
    printf("%s %.15f, eps = %.1e\n", name, result, eps);
}


int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Некорректный ввод аргументов.\n");
        return ERR_ARGC;
    }

    double eps;
    Status status = correct_epsilon(argv[1], &eps);
    if (status != OK) {
        printf("Некорректное epsilon.\n");
        return status;
    }
    
    double x;
    status = correct_double(argv[2], &x);
    if (status != OK) {
        printf("Некорректное x.\n");
        return status;
    }

    printf("Пункт 1.\n");

    {
        double result = 0.0;
        status = sum_a(x, eps, &result);
        if (status == OK) {
            print_result("a)", result, eps);
        } else {
            printf("Ошибка выполнения.\n");
            return status;
        }
    }

    {
        double result = 0.0;
        status = sum_b(x, eps, &result);
        if (status == OK) {
            print_result("b)", result, eps);
        } else {
            printf("Ошибка выполнения.\n");
            return status;
        }
    }

    {
        double result = 0.0;
        status = sum_c(x, eps, &result);
        if (status == OK) {
            print_result("c)", result, eps);
        } else {
            printf("Ошибка выполнения.\n");
            return status;
        }
    }

    {
        double result = 0.0;
        status = sum_d(x, eps, &result);
        if (status == OK) {
            print_result("d)", result, eps);
        } else {
            printf("Ошибка выполнения.\n");
            return status;
        }
    }

    return OK;
}
