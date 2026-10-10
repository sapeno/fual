#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

typedef enum {
    OK,
    ERR_ARGS,
    ERR_FLAG,
    ERR_RANGE, 
    ERR_ZERO,
    ERR_EPSILON,
    ERR_NUM, 
    ERR_OVERFLOW
} Status;

typedef enum {
    ROOTS_NONE,     
    ROOTS_ONE,         
    ROOTS_TWO,         
    ROOTS_INFINITE,     
    ROOTS_LINEAR        
} Roots;

typedef struct {
    Roots root;
    double x1, x2;
} Result;

int parse_double(const char *s, double *result) {
    if (s == NULL || *s == '\0') return 0;
    char *endptr = NULL;
    double val = strtod(s, &endptr);
    if (endptr == s || *endptr != '\0') return 0;
    if (isinf(val) || isnan(val)) return 0;
    *result = val;
    return 1;
}

int parse_long(const char *s, long *result) {
    if (s == NULL || *s == '\0') return 0;
    char *endptr = NULL;
    long val = strtol(s, &endptr, 10);
    if (endptr == s || *endptr != '\0') return 0;
    *result = val;
    return 1;
}

int correct_flag(const char *flag, char *result) {
    if (flag == NULL) return 0;
    if (flag[0] != '-' && flag[0] != '/') return 0;
    if (flag[1] == '\0' || flag[2] != '\0') return 0;
    *result = flag[1];
    return 1;
}
int is_zero(const double a, const double eps) {
    return fabs(a) < eps;
}
Status quad(const double a, const double b, const double c, const double eps, Result *result) {
    if (is_zero(a, eps)) {
        if (is_zero(b, eps)) {
            if (is_zero(c, eps)) {
                result->root = ROOTS_INFINITE;
            } else {
                result->root = ROOTS_NONE;
            }
        } else {
            result->root = ROOTS_LINEAR;
            result->x1 = -c / b + 0.0;
        }
        return OK;
    }

    double D = b*b - 4.0 * a * c;
    if (D > eps) {
        double sq = sqrt(D);
        result->root = ROOTS_TWO;
        result->x1 = (-b + sq) / (2.0 * a);
        result->x2 = (-b - sq) / (2.0 * a);
    } else if (fabs(D) <= eps) {
        result->root = ROOTS_ONE;
        result->x1 = -b / (2.0 * a) + 0.0;
    } else {
        result->root = ROOTS_NONE;
    }
    return OK;
}

Status mul(const long a, const long b, int *result) {
    if (a == 0 || b == 0) return ERR_ZERO;
    *result = (a % b == 0);
    return OK;
}

Status triangle(const double a, const double b, const double c, const double eps, int *result) {
    if (a <= eps || b <= eps || c <= eps) return ERR_RANGE;
    double s[3] = {a, b, c};
    for (int i = 0; i < 2; ++i) {
        for (int j = i + 1; j < 3; ++j) {
            if (s[i] > s[j]) {
                double t = s[i]; s[i] = s[j]; s[j] = t;
            }
        }
    }
    const double SQRT_DBL_MAX = sqrt(DBL_MAX);
    if (s[0] > SQRT_DBL_MAX || s[1] > SQRT_DBL_MAX || s[2] > SQRT_DBL_MAX)
        return ERR_OVERFLOW;

    double a2 = s[0] * s[0];
    double b2 = s[1] * s[1];
    double c2 = s[2] * s[2];
    if (isinf(a2) || isinf(b2) || isinf(c2))
        return ERR_OVERFLOW;

    double a_b = a2 + b2;
    if (isinf(a_b)) return ERR_OVERFLOW;

    *result = (fabs(a_b - c2) < eps);
    return OK;
}

void print_quad(const Result *result) {
    switch (result->root) {
        case ROOTS_INFINITE:
            printf(", Бесконечное число корней.\n");
            break;
        case ROOTS_NONE:
            printf(", Нет вещественных корней.\n");
            break;
        case ROOTS_LINEAR:
            printf(", x = %.4f\n", result->x1);
            break;
        case ROOTS_ONE:
            printf(", x = %.4f\n", result->x1);
            break;
        case ROOTS_TWO:
            printf(", x1 = %.4f, x2 = %.4f\n", result->x1, result->x2);
            break;
    }
}

Status print_q(const double eps, const double a, const double b, const double c) {
    double coef[3] = {a, b, c};
    int per[6][3] = {
        {0, 1, 2}, {0, 2, 1},
        {1, 0, 2}, {1, 2, 0},
        {2, 0, 1}, {2, 1, 0}
    };

    int print_uni = 0;

    for (int i = 0; i < 6; ++i) {
        double A = coef[per[i][0]];
        double B = coef[per[i][1]];
        double C = coef[per[i][2]];

        int dup = 0;
        for (int j = 0; j < i; ++j) {
            double Aj = coef[per[j][0]];
            double Bj = coef[per[j][1]];
            double Cj = coef[per[j][2]];
            if (Aj == A && Bj == B && Cj == C) {
                dup = 1;
                break;
            }
        }
        if (dup) continue;

        ++print_uni;

        printf("%d: %.4f*x^2 + %.4f*x + %.4f", print_uni, A, B, C );

        Result result;
        Status status = quad(A, B, C, eps, &result);
        if (status != OK) return status;
        print_quad(&result);
    }
    return OK;
}

Status print_m(const long a, const long b) {
    int result = 0;
    Status status = mul(a, b, &result);
    if (status != OK) return status;
    if (result)
        printf("%ld кратно %ld.\n", a, b);
    else
        printf("%ld не кратно %ld.\n", a, b);
    return OK;
}

Status print_t(const double eps, const double a, const double b, const double c) {
    int result = 0;
    Status status = triangle(a, b, c, eps, &result);
    if (status != OK) return status;
    if (result)
        printf("Могут быть сторонами прямоугольного треугольника.\n");
    else
        printf("Не могут быть сторонами прямоугольного треугольника.\n");
    return OK;
}

int main(int argc, char *argv[]) {
    if (argc < 2) return ERR_ARGS;

    char flag;
    if (!correct_flag(argv[1], &flag)) {
        printf("Неверный флаг.\n");
        return ERR_ARGS;
    }

    switch (flag){
        case 'q': {
            if (argc != 6) {
                printf("Необхожимо ввести 4 параметра: эпсилон и трри коэффициентаю\n");
                return ERR_ARGS;
            }
            double eps = 0.0, a = 0.0, b = 0.0, c = 0.0;
            if (!parse_double(argv[2], &eps) || eps <= 0.0) {
                printf("Некорректное эпсилон.\n");
                return ERR_EPSILON;
            }
            if (!parse_double(argv[3], &a) || !parse_double(argv[4], &b) || !parse_double(argv[5], &c)) {
                printf("Некорректные коэффициенты\n");
                return ERR_NUM;
            }
            return print_q(eps, a, b, c);
        }
        case 'm': {
            if (argc != 4) {
                printf("Необхожимо ввести 2 параметра: два ненулевых целых числа.\n");
                return ERR_ARGS;
            }

            long a = 0, b = 0;
            if (!parse_long(argv[2], &a) || !parse_long(argv[3], &b) || a == 0 || b == 0) {
                printf("Некорректные числа.\n");
                return ERR_NUM;
            }
            return print_m(a, b);
        }
        case 't': {
            if (argc != 6) {
                printf("Необхожимо ввести 4 параметра: эпсилон и три длины сторон\n");
                return ERR_ARGS;
            }
            double eps = 0.0, a = 0.0, b = 0.0, c = 0.0;
            if (!parse_double(argv[2], &eps) || eps <= 0.0) {
                printf("Некорректное эпсилон.\n");
                return ERR_EPSILON;
            }
            if (!parse_double(argv[3], &a) || !parse_double(argv[4], &b) || !parse_double(argv[5], &c)) {
                printf("Некорректные длины сторон\n");
                return ERR_NUM;
            }
            if (a <= eps || b <= eps || c <= eps) {
                printf("Длина сторон должна быть положительной.\n");
                return ERR_RANGE;
            }
            return print_t(eps, a, b, c);
        }
    
        default:
            printf ("Такого флага не существует.\n");
            return ERR_FLAG;
    }
}


