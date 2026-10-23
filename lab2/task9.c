#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>
#include <math.h>

typedef long long ll;

typedef enum {
    OK,
    ERR_ARGS,        
    ERR_BASE,       
    ERR_RANGE, 
    ERR_ALLOC       
} Status;

ll nod(ll a, ll b) {
    while (b) {
        ll t = b;
        b = a % b;
        a = t;
    }

    return a;
}

int is_inf(ll b, int base) {
    if (b <= 0) return 0;

    while (1){
        ll n = nod(b, base);
        if (n == 1) break;
        b /= n;
    }
    
    return (b == 1);
}

Status num_den_fd(double x, ll *num, ll *den) {
    double scaled = x * 1e17; 

    if (scaled != (double)(ll)scaled) return ERR_RANGE;

    char buf[64];

    int n = snprintf(buf, sizeof buf, "%.17f", x);
    if (n < 0) return ERR_ARGS;

    char *dot = strchr(buf, '.');
    if (!dot) return ERR_ARGS;
    *dot = '\0';

    const char *int_part = buf;
    const char *frac_part = dot + 1;

    size_t f_len = strlen(frac_part);

    if (strlen(int_part) > 18 || f_len > 18) return ERR_ARGS;

    *num = 0;
    for (const char *p = int_part; *p; ++p) {
        if (*num > (LLONG_MAX - (*p - '0')) / 10) return ERR_RANGE;
        *num = *num * 10 + (*p - '0');
    }
    for (const char *p = frac_part; *p; ++p) {
        if (*num > (LLONG_MAX - (*p - '0')) / 10) return ERR_RANGE;
        *num = *num * 10 + (*p - '0');
    }

    *den = 1;
    for (size_t i = 0; i < f_len; ++i) 
        *den *= 10;

    if (x > 0.0 && *num == 0) {
        return ERR_RANGE; 
    }

    ll nodus = nod(*num, *den);
    *num /= nodus;
    *den /= nodus;

    return OK;
}

void print_flags(int count, va_list args, int *flags, Status status, int base) {
    if (!flags || count <= 0) return;
    
    if (status == OK){
        for (int i = 0; i < count; ++i) {
            double val = va_arg(args, double);
            printf("В СС %d, чиcло %g: %s\n", base, val, flags[i] ? "конечное" : "бесконечное");
        }
    }else fprintf(stderr, "Ошибка выполнения %d.\n", status); 
}

Status myFunc(int base, const int count, double eps, ...) {

    if (base < 2 || base > 36) return ERR_BASE;
    if (count <= 0) return ERR_ARGS;
    if (eps <= 0.0) return ERR_ARGS;

    int *flag = malloc((size_t)count * sizeof(int));
    if (!flag) return ERR_ALLOC;

    va_list args;
    va_list dest;
    va_start(args, eps);
    va_copy(dest, args);

    Status status = OK;

    for (int i = 0; i < count; ++i) {
        double x = va_arg(args, double);

        if (!isfinite(x) || x <= eps || x >= 1.0 - eps) {
            status = ERR_RANGE;
            break;
        }

        ll num, den;
        status = num_den_fd(x, &num, &den);
        if (status != OK) break;

        flag[i] = is_inf(den, base);
    }

    print_flags(count, dest, flag, status, base);

    va_end(dest);
    va_end(args);

    free(flag);
    flag = NULL;

    return status;

}



int main() {
    double eps = 1e-20;
    Status status;

    status = myFunc(2, 3, eps, 0.5, 0.25, 0.3);

    printf("\n");
    status = myFunc(3, 3, eps, 0.1, 0.2, 0.15);

    printf("\n");
    status = myFunc(10, 1, eps, 0.2);

    printf("\n");
    status = myFunc(5, 2, eps, 0.2, 0.6);

    printf("\n");
    status = myFunc(2, 1, eps, 0.2);
    
    printf("\n");
    status = myFunc(3, 1, eps, 0.2);

    printf("\n");
    status = myFunc(4, 1, eps, 0.6);

    printf("\n");
    status = myFunc(2, 1, eps, 0.999755859375 );

    printf("\n");
    status = myFunc(2, 1, eps, 0.000000000000005 );

    printf("\n");
    status = myFunc(2, 1, eps, 0.000000000931322574615478515625 );

    printf("\n");
    status = myFunc(2, 1, eps, -0.0 );

    printf("\n");
    status = myFunc(2, 1, eps, 1.0 );

    printf("\n");
    status = myFunc(2, 1, eps, nan);
    

    return status;
}

