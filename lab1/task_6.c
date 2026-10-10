#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>

typedef enum {
    OK = 0,
    ERR_ARGS,       
    ERR_ALLOC,      
    ERR_RANGE,     
    ERR_MATH       
} Status;

typedef struct { double x, y; } Point;

bool is_convex(int n, ...) {
    if (n < 3) return false;
    va_list args;
    va_start(args, n);
    
    Point p0 = va_arg(args, Point);
    Point p1 = va_arg(args, Point);
    Point p2 = va_arg(args, Point);
    
    double cross = (p1.x - p0.x) * (p2.y - p1.y) - (p1.y - p0.y) * (p2.x - p1.x);
    double sign = (cross > 0) ? 1.0 : (cross < 0 ? -1.0 : 0.0);
    
    Point prev2 = p1, prev1 = p2;
    bool convex = true;
    
    for (int i = 3; i < n; ++i) {
        Point cur = va_arg(args, Point);
        double c = (prev1.x - prev2.x) * (cur.y - prev1.y) - (prev1.y - prev2.y) * (cur.x - prev1.x);
        
        if (sign != 0 && c != 0) {
            double cur_sign = (c > 0) ? 1.0 : -1.0;
            if (cur_sign != sign) convex = false;
        }
        prev2 = prev1; prev1 = cur;
    }
    va_end(args);
    return convex;
}

double gorner(double x, int n, ...) {
    if (n < 0) return NAN;
    va_list args;
    va_start(args, n);
    
    double result = va_arg(args, double);
    for (int i = 1; i <= n; ++i) {
        result = result * x + va_arg(args, double);
    }
    va_end(args);
    return result;
}

Status is_kaprekar(const char *s, int base, bool *out_res) {
    if (!s || !out_res || base < 2 || base > 36) return ERR_ARGS;
    *out_res = false;

    unsigned long long k = 0;
    for (int i = 0; s[i] != '\0'; ++i) {
        int d = 0;
        char c = s[i];
        if (c >= '0' && c <= '9') d = c - '0';
        else if (c >= 'A' && c <= 'Z') d = c - 'A' + 10;
        else if (c >= 'a' && c <= 'z') d = c - 'a' + 10;
        else return ERR_ARGS;

        if (d >= base) return ERR_ARGS;
        if (k > (ULLONG_MAX - d) / base) return ERR_RANGE;
        k = k * base + d;
    }

    if (k == 0) { 
        *out_res = true; 
        return OK; 
    }

    if (k > (unsigned long long)UINT_MAX) return ERR_RANGE;
    unsigned long long sq = k * k;
    
    unsigned long long divisor = base;
    while (divisor <= sq) {
        unsigned long long right = sq % divisor;
        unsigned long long left = sq / divisor;
        if (right != 0 && left + right == k) {
            *out_res = true;
            return OK;
        }
        if (divisor > ULLONG_MAX / base) break;
        divisor *= base;
    }
    return OK;
}

double geom_mean(int count, ...) {
    if (count <= 0) return NAN;
    va_list args;
    va_start(args, count);
    
    double prod = 1.0;
    for (int i = 0; i < count; ++i) {
        double val = va_arg(args, double);
        if (val < 0.0) {
            va_end(args); 
            return NAN; 
        }
        prod *= val;
    }
    va_end(args);
    return pow(prod, 1.0 / count);
}

double fast_pow(double base, int exp) {
    if (exp == 0) return 1.0;
    if (exp < 0) return 1.0 / fast_pow(base, -exp);
    
    if (exp % 2 == 0) {
        double half = fast_pow(base, exp / 2);
        return half * half;
    } else {
        return base * fast_pow(base, exp - 1);
    }
}

typedef double (*MathFunc)(double);

Status dih(MathFunc f, double a, double b, double eps, double *out_root) {
    if (!f || !out_root || eps <= 0.0) return ERR_ARGS;
    
    double fa = f(a);
    double fb = f(b);
    
    if (fa * fb > 0.0) return ERR_MATH;
    
    while ((b - a) > eps) {
        double mid = (a + b) / 2.0;
        double fmid = f(mid);
        
        if (fmid == 0.0) break;
        
        if (fa * fmid < 0.0) {
            b = mid;
            fb = fmid;
        } else {
            a = mid;
            fa = fmid;
        }
    }
    
    *out_root = (a + b) / 2.0;
    return OK;
}

static double f1(double x) { return x*x - 2.0; }          
static double f2(double x) { return x*x*x - x - 2.0; }    

int main(void) {
    printf("1.\n");

    Point sq[] = {{0,0}, {2,0}, {2,2}, {0,2}};
    Point bad[] = {{0,0}, {2,0}, {1,0.5}, {2,2}, {0,2}};
     
    printf("%s\n", is_convex(4, sq[0], sq[1], sq[2], sq[3]) ? "Выпуклый" : "Не выпуклый");
    printf("%s\n", is_convex(4, bad[0], bad[1], bad[2], bad[3], bad[4]) ? "Выпуклый" : "Не выпуклый");

    printf("\n2.\n");
    double val = gorner(2.0, 3, 2.0, -3.0, 4.0, -5.0); //7
    printf("%g\n", val);

    printf("\n3.\n");
    bool res;
    const char *k_tests[] = {"9", "45", "55", "99", "10"};
    for (int i = 0; i < 5; ++i) {
        Status status = is_kaprekar(k_tests[i], 10, &res);
        if (status == OK) printf("%s: %s\n", k_tests[i], res ? "Да" : "Нет");
        else printf("%s: Ошибка выполнения %d\n", k_tests[i], status);
    }
    Status status = is_kaprekar("111", 2, &res);
    if (status == OK) printf("111: %s\n", res ? "Да" : "Нет");
    else printf("Ошибка выполнения %d\n", status);

    status = is_kaprekar("10", 2, &res);
    if (status == OK) printf("10: %s\n", res ? "Да" : "Нет");
    else printf("Ошибка выполнения %d\n", status);

    printf("\n4.\n");
    printf("%g\n", geom_mean(3, 2.0, 4.0, 8.0)); //4
    printf("%g\n", geom_mean(2, -2.0, 4.0)); //проверка на нан

    printf("\n5.\n");
    printf("2^10 = %g\n", fast_pow(2.0, 10));
    printf("2^-3 = %g\n", fast_pow(2.0, -3));

    printf("\n6.\n");
    double root;
    status = dih(f1, 1.0, 2.0, 1e-6, &root);
    if (status == OK) printf("x^2 - 2 = 0: %g\n", root);
    
    status = dih(f2, 1.0, 2.0, 1e-6, &root);
    if (status == OK) printf("x^3 - x - 2 = 0: %g\n", root);
    
    status = dih(f1, 2.0, 3.0, 1e-6, &root);
    if (status == ERR_MATH) printf("x^2 - 2 = 0: на [2, 3] корней нет\n");

    return 0;
}