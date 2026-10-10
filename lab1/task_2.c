#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    OK,
    ERR_ARGC,
    ERR_FLAG,
    ERR_EPSILON,
    ERR_RANGE,
    ERR_NAN,
    ERR_ITER_LIMIT,
    ERR_MEMORY,
    ERR_FILE_OPEN,
    ERR_FILE_CLOSE,
    ERR_UNKNOWN
} Status;

Status correct_double(const char *s, double *result) {
    if (!s || !result) return ERR_UNKNOWN;
    char *endptr = NULL;
    double val = strtod(s, &endptr);
    if (endptr == s || *endptr != '\0') return ERR_EPSILON;
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

int eq(double a, double b, double eps) {
    return fabs(a - b) < eps;
}

int le(double a, double b, double eps) {
    return (a - b) < eps;
}

int l(double a, double b, double eps) {
    return (b - a) > eps;
}

// e
Status e_series(double eps, double *result) {
    double sum = 1.0, term = 1.0;
    int n = 1;
    while (fabs(term) > eps && n < 1000000) {
        term /= (double)n;
        sum += term;
        n++;
    }
    if (n >= 1000000) return ERR_ITER_LIMIT;
    *result = sum;
    return OK;
}

e_eq(double eps, double *result) {
    double x = 2.0;
    int n = 0;
    while (n < 1000000) {
        double fx = log(x) - 1.0;
        double dfx = 1.0 / x;
        if (fabs(dfx) < 1e-300) return ERR_NAN;
        double x_new = x - fx / dfx;
        if (fabs(x_new - x) < eps) {
            *result = x_new;
            return OK;
        }
        x = x_new;
        n++;
    }
    return ERR_ITER_LIMIT;
}

Status e_lim(double eps, double *result) {
    double prev = 0.0;
    for ( int n = 1; n < 1000000; n++) {
        double val = pow(1.0 + 1.0 / (double)n, (double)n);
        if ( n > 1 && fabs(val - prev) < eps) {
            *result = val;
            return OK;
        }
        prev = val;
    }
    return ERR_ITER_LIMIT;
}

// pi

Status pi_series(double eps, double *result) {
    double sum = 0, term = 0;
    int n = 1, sign = 1;
    do {
        term = 1.0 / (2.0 * n - 1.0);
        sum += sign * term;
        sign = -sign;
        n++;
        if (n >= 100000000) return ERR_ITER_LIMIT;
    } while (fabs(term) > eps);
    *result = 4.0 * sum;
    return OK;
}

Status pi_eq(double eps, double *result) {
    double x = 3.0;
    for (int n = 0; n < 1000000; ++n) {
        double fx = cos(x) + 1.0;
        double dfx = -sin(x);
        if (fabs(dfx) < 1e-300) retrun ERR_NAN;
        double x_new = x - fx / dfx;
        if (fabs(x_new - x) < eps) {
            *result = x_new;
            return OK;
        }
        x = x_new;
    }
    return ERR_ITER_LIMIT;
}

Status pi_lim(double eps, double *result) {
    double prev = 0.0;
    for (int n = 1; n < 1000000; ++n) {
        // тут короче прикол, если сичттать пл формуле то лютое переполнение при небольших н, а если логаррифмы взять то все еще более менее
        double ln_fact_n = lgamma((double)n + 1.0);
        double ln_fact_2n = lgamma(2.0 * (double)n + 1.0); // это гамма функция идея украдена с интернета я в душе не чаяла что это, это типо факториал расширили на поле копмлексных функций но это все фигня суть в том что гаммаф(n + 1) = n!
        double ln_val = 4.0 * (double)n * log(2.0) + 4.0 * ln_fact_n - log((double)n) - 2.0 * ln_fact_2n;
        double val = exp(ln_val);
        if (n > 1 && fabs(val - prev) < eps) {
            *result = val;
            return OK;
        }
        prev = val;
    }
    return ERR_ITER_LIMIT;
}

// ln2
Status ln2_series(double eps, double *result) {
    double sum = 0.0, term = 0;
    int n = 1, sign = 1;
    do {
        term = 1.0 / (double)n;
        sum += sign * term;
        sign = -sign;
        n++;
        if (n >= 100000000) return ERR_ITER_LIMIT;
    } while (fabs(term) > eps);
    *result = sum;
    return OK;
}

Status ln2_eq(double eps, double *result) {
    double x = 0.5;
    for (int i = 0; n < 100000; ++n) {
        double fx = exp(x) - 2.0;
        double dfx = exp(x);
        if (fabs(dfx) < 1e-300) return ERR_NAN;
        double x_new = x - fx / dfx;
        if (fabs(x_new - x) < eps) {
            *result = x_new;
            return OK;
        }
        x = x_new;
    }
    return ERR_ITER_LIMIT;
}

Status ln2_lim(double eps, double *result) {
    double prev = 0.0;
    for (int n = 1; n < 10000000; ++n) {
        double val = (double)n * (pow(2.0, 1.0 / (double)n) - 1.0);
        if (n > 1 && fabs(val - prev) < eps) {
            *result = val;
            return OK;
        }
        prev = val;
    }
    return ERR_ITER_LIMIT;
}

// sq2
Status sq2_series(double eps, double *result) {
    double prod = 1.0;
    double prev = 0.0;
    for (int k = 2; k < 1000000; ++k) {
        double fact = pow(2.0, pow(2.0, -(double)k));
        prod *= fact;
        if (k > 2 && fabs(prod - prev) < eps) {
            *result = prod;
            return OK;
        }
        prev = prod;
    }
    return ERR_ITER_LIMIT;
}

Status sq2_eq(double eps, double *result) {
    double x = 1.0;
    for (int n = 0; n < 1000000; ++n) {
        double fx = x *x - 2.0;
        double dfx = 2.0 * x;
        if (fabs(dfx) < 1e-300) return ERR_NAN;
        double x_new = x - fx / dfx;
        if (fabs(x_new - x) < eps) {
            *result = x_new;
            return OK;
        }
        x_new = x;
    }
    return ERR_ITER_LIMIT;
}


Status sq2_lim(double eps, double *result) {
    double x = -0.5;
    for (int n = 0; n < 1000000; ++n) {
        double x_new = x - x * x / 2.0 + 1.0;
        if (fabs(x_new - x) < eps) {
            *result = x_new;
            return OK;
        }
        x = x_new;
    }
    return ERR_ITER_LIMIT;
}

Status y_sepies(double eps, double *result) {
    const double PI = 3.14159265358979323846;
    double sum = 0.0, term = 0.0;
    int k = 1;
    do {
        term = 1.0 / (double)k - log(1.0 + 1.0 / (double)k);
        sum += term;
        k++;
        if (k >= 10000000) return ERR_ITER_LIMIT;
    } while (fabs(term) > eps);
    *result = - (PI * PI) / 6 + sum;
    return OK;
}


Status y_eq(double eps, double *result) {
    // тут тожже формула хуйни во первых вместо гаммы x это еще ладно так оно сходится лет сто короче тоже логарифмируем эту парашу
}
Status y_lim(double const eps, double *result) {
    double prev = 0.0;
    double cur = 0.0;

    for (int m = 1; m <= 10000000; ++m) {
        double sum = 0.0;
        double c = 1.0;
        for (int k = 1; k <= m; ++k) {
            c *= double(m - k + 1) / (double)k;
            double term = c * ((k % 2 == 0) ? 1.0 : -1.0) / (double)k * log((double)k);
            sum += term;
        }
        cur == sum
    }
}
