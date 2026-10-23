#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>

typedef enum {
    OK,
    ERR_ARGS,
    ERR_FORMAT,
    ERR_RANGE,
    ERR_OVERFLOW,
    ERR_IO,
    ERR_ALLOC
} Status;

typedef struct {
    int    is_file;
    FILE  *fp;
    char  *buf;
    size_t cap;
    size_t pos;
    int    overflow;
} Buf;

static Status buf_putc(Buf *b, char c) {
    if (b->is_file) {
        if (fputc((unsigned char)c, b->fp) == EOF) return ERR_IO;
        return OK;
    }
    if (b->pos + 1 >= b->cap) { b->overflow = 1; return ERR_OVERFLOW; }
    b->buf[b->pos++] = c;
    return OK;
}

static Status buf_puts(Buf *b, const char *s) {
    if (!s) return ERR_ARGS;
    while (*s) {
        Status status = buf_putc(b, *s++);
        if (status != OK) 
            return status;
    }
    return OK;
}

typedef struct {
    int val;
    const char *sym;
} Roman;

const Roman ROMAN[] = {
    {1000,"M"}, {900,"CM"}, {500,"D"}, {400,"CD"},
    {100,"C"}, {90,"XC"}, {50,"L"}, {40,"XL"},
    {10,"X"}, {9,"IX"}, {5,"V"}, {4,"IV"}, {1,"I"}
};

int digit_val(char c, int upper) {
    if (c >= '0' && c <= '9') return c - '0';
    if (upper) {
        if (c >= 'A' && c <= 'Z') return c - 'A' + 10;
    } else {
        if (c >= 'a' && c <= 'z') return c - 'a' + 10;
    }
    return -1;
}

Status Ro_flag(int v, const Roman *tbl, Buf *b) {
    if (!tbl) return ERR_ARGS;
    if (v <= 0 || v > 3999) return ERR_RANGE;

    for (size_t i = 0; i < 13; ++i) {
        while (v >= tbl[i].val) {
            Status status = buf_puts(b, tbl[i].sym);
            if (status != OK) return status;
            v -= tbl[i].val;
        }
    }

    return OK;
}

Status Ze_flag(unsigned int n, Buf *b) {
    if (n == 0) return buf_putc(b, '1');

    const size_t max_fib = 64;
    unsigned int fib[64];

    int count = 2;
    fib[0] = 1; fib[1] = 2;

    while ((unsigned long long)fib[count - 1] + fib[count - 2] <= n) {
        if ((size_t)count >= max_fib) return ERR_RANGE;
        unsigned long long tmp = (unsigned long long)fib[count - 1] + fib[count - 2];
        if (tmp > UINT_MAX) return ERR_OVERFLOW;
        fib[count] = tmp;
        count++;
    }

    char bits[64] = {0};
    unsigned int r = n;
    for (int i = count-1; i >= 0; --i) {
        if (fib[i] <= r) {
            bits[i] = '1';
            r -= fib[i];
        } else bits[i] = '0';
    }

    for (int i = 0; i < count; ++i) {
        Status status = buf_putc(b, bits[i]);
        if (status != OK) return status;
    }
    return buf_putc(b, '1');
}

Status Cv_CV_flag(int v, int base, int upper, Buf *b) {
    if (base < 2 || base > 36) base = 10;
    const char *digit = upper ? "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ" : "0123456789abcdefghijklmnopqrstuvwxyz";

    unsigned int uv;
    int sign = 0;
    if ( v < 0) {
        sign = 1;
        uv = (unsigned int)(-(long long)v);
    } else uv = (unsigned int)v;

    char tmp[64];
    int k = 0;
    if (uv == 0) tmp[k++] = '0';
    while (uv){
        tmp[k++] = digit[uv % base];
        uv /= base; 
    }

    if (sign) {
        Status status = buf_putc(b, '-');
        if (status != OK) return status;
    }

    while (k--){
        Status status = buf_putc(b, tmp[k]);
        if (status != OK) return status;
    }

    return OK; 
}

Status to_TO_flag(const char *s, int base, int upper, Buf *b) {
    if (!s) return ERR_ARGS;
    if (base < 2 || base > 36) base = 10;

    long long res = 0;
    int sign = 1;
    const char *p = s;
    if (*p == '-') {
        sign = -1;
        p++;
    }

    while (*p){
        int d = digit_val(*p, upper);
        if (d < 0 || d >= base) break;
        if (res > (LLONG_MAX - d) / base) return ERR_OVERFLOW;
        res = res * base + d;
        p++;
    }

    if (res * (long long)sign < LLONG_MIN) return ERR_OVERFLOW;
    res = (long long) sign * res;

    char tmp[32];
    int n = snprintf(tmp, sizeof tmp, "%lld", res);
    if (n < 0) return ERR_FORMAT;
    return buf_puts(b, tmp);

    return OK;
}

Status mi_mu_md_mf_flag(const unsigned char *bytes, size_t n, Buf *b) {
    if(!bytes) return ERR_ARGS;
    for (size_t i = 0; i < n; i++) {
        if (i) {
            Status status = buf_putc(b, ' ');
            if (status != OK) return status;
        }
        for (int k = 7; k >= 0; k--) {
            Status status = buf_putc(b, (bytes[i] >> k) & 1 ? '1' : '0');
            if (status != OK) return status;
        }
    }
    return OK;
}

Status poexali(int is_file, FILE *file, Buf *b, const char *format, va_list args, int *out_len) {
    if (!format) return ERR_ARGS;
    if (is_file && !file) return ERR_ARGS;
    if (!is_file && (!b || !b->buf)) return ERR_ARGS;

    Buf bf;
    if (is_file) bf = (Buf){1, file, NULL, 0, 0, 0};
    else bf = (Buf){0, NULL, b->buf, b->cap, b->pos, 0};

    const char *p = format;
    Status status = OK;

    while (*p){
        if (*p != '%') {
            status = buf_putc(&bf, *p);
            if (status != OK) return status;
            p++;
            continue;
        }

        p++;
        if (*p == '%') {
            status = buf_putc(&bf, '%');
            if (status != OK) return status;
            p++;
            continue;
        }

        if (p[0] == 'R' && p[1] == 'o') {
            int v = va_arg(args, int);
            status = Ro_flag(v, ROMAN, &bf);
            if (status != OK) return status;
            p += 2;
        } else if (p[0] == 'Z' && p[1] == 'r') {
            unsigned int v = va_arg(args, unsigned int);
            status = Ze_flag(v, &bf);
            if (status != OK) return status;
            p += 2;
        } else if (p[0] == 'C' && (p[1] == 'v' || p[1] == 'V')) {
            int upper = (p[1] == 'V');
            int v = va_arg(args, int);
            int base = va_arg(args, int);
            status = Cv_CV_flag(v, base, upper, &bf);
            if (status != OK) return status;
            p+=2;
        } else if (p[0] == 't' && (p[1] == 'o' || p[1] == 'O')) {
            int upper = (p[1] == 'O');
            const char *s = va_arg(args, const char *);
            int base = va_arg(args, int);
            if (!s) return ERR_IO;
            status = to_TO_flag(s, base, upper, &bf);
            if (status != OK) return status;
            p += 2;
        } else if (p[0] == 'm' && (p[1]=='i' || p[1]=='u' || p[1]=='d' || p[1]=='f')) {
            char c = p[1];
            if (c == 'i') {
                int v = va_arg(args, int);
                status = mi_mu_md_mf_flag((const unsigned char*)&v, sizeof(v), &bf);
            } else if (c == 'u') {
                unsigned int v = (va_arg(args, unsigned int));
                status = mi_mu_md_mf_flag((const unsigned char*)&v, sizeof(v), &bf);
            } else if (c == 'd') {
                double v = va_arg(args, double);
                status = mi_mu_md_mf_flag((const unsigned char*)&v, sizeof(v), &bf);
            } else if (c == 'f') {
                float v = (float)va_arg(args, double);
                status = mi_mu_md_mf_flag((const unsigned char*)&v, sizeof(v), &bf);
            }
            if (status != OK) return status;
            p += 2;

        } else {
            char spec[64]; int si = 0;
            spec[si++] = '%';

            while (*p && strchr("-+ #0", *p) && si < 60) spec[si++] = *p++;
            while (*p && (isdigit((unsigned char)*p) || *p == '.') && si < 60) spec[si++] = *p++;
            while (*p && strchr("hljztL", *p) && si < 60) spec[si++] = *p++;

            if (!*p) return ERR_FORMAT;

            char conv = *p++;
            switch (conv) {
                case 'd': case 'i': case 'o': case 'u': case 'x': case 'X':
                case 'c': case 's': case 'p': case 'f': case 'F': case 'e': 
                case 'E': case 'g': case 'G': case 'a': case 'A':
                    break;
                default:
                    return ERR_FORMAT;
            }

            spec[si++] = conv; 
            spec[si] = '\0';

            va_list args_copy;
            va_copy(args_copy, args);
            int need = vsnprintf(NULL, 0, spec, args_copy);
            va_end(args_copy);

            if (need < 0) return ERR_FORMAT;

            char *tmp = malloc(need + 1);
            if (tmp == NULL) return ERR_ALLOC;

            int n = vsnprintf(tmp, need + 1, spec, args);
            if (n < 0) {
                free(tmp);
                return ERR_FORMAT;
            }

            status = buf_puts(&bf, tmp);
            free(tmp);
            if (status != OK) return status;
        }
    }

    if (!is_file) {
    b->pos = bf.pos;
    b->overflow = bf.overflow;
    }

    if (out_len) {
        if (is_file) *out_len = 0;
        else *out_len = (int)b->pos;
    }

    return OK;
}

Status oversprint(char *str, size_t cap, const char *format, int *out_len, ...) {
    if (!str || !format) return ERR_ARGS;
    if (cap == 0) return ERR_ARGS;
    Buf b = {0, NULL, str, cap, 0 , 0};
    
    str[0] = '\0';

    va_list args;
    va_start(args, out_len);
    Status status = poexali(0, NULL, &b, format, args, out_len);
    va_end(args);

    str[b.pos < cap ? b.pos : cap - 1] = '\0';

    if (status == OK && b.overflow) status = ERR_OVERFLOW;
    if (out_len) 
        *out_len = (int)b.pos;
    return status;
}

Status overfprint(FILE *file, const char *format, int *out_len, ...) {
    if (!file || !format) return ERR_ARGS;

    va_list args;
    va_start(args, out_len);
    Status status = poexali(1, file, NULL, format, args, out_len);
    va_end(args);

    if (status == OK && ferror(file)) status = ERR_IO;
    return status;
}

int main() {
    char buf[128]; int len = 0;

    printf("oversprint.\n");

    Status status = oversprint(buf, sizeof buf, "Roman %Ro, Zeck %Zr, base %Cv", &len, 3999, 42u, 1000, 16);
    if (status == OK) printf("%s\n", buf);
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = oversprint(buf, 5, "Roman %Ro", &len, 2024);
    if (status == OK) printf("%s\n", buf);
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 
                 
    status = oversprint(buf, sizeof buf, "%to", &len, (const char*)NULL, 16);
    if (status == OK) printf("%s\n", buf);
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = oversprint(buf, sizeof buf, "%Q", &len);
    if (status == OK) printf("%s\n", buf);
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = oversprint(buf, sizeof buf, "Mem: %mu", &len, 15654654654);
    if (status == OK) printf("%s\n", buf);
    else fprintf(stderr, "Ошибка выполнения %d.\n", status); 



    printf("\noverfprintf.\n");

    status = overfprint(stdout, "Roman 2024: %Ro\n", &len, 2024);
    if (status != OK) fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = overfprint(stdout, "Roman 5000: %Ro\n", &len, 5000);
    if (status != OK) fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    status = overfprint(stdout, "Zeck 100: %Zr\n", &len, 100u);
    if (status != OK) fprintf(stderr, "Ошибка выполнения %d.\n", status); 
    
    status = overfprint(stdout, "Base10 -255: base%d %Cv\n", &len, 36, -255, 36);
    if (status != OK) fprintf(stderr, "Ошибка выполнения %d.\n", status); 
    
    status = overfprint(stdout, "base: %to\n", &len, "ff", 16);
    if (status != OK) fprintf(stderr, "Ошибка выполнения %d.\n", status); 
    
    status = overfprint(stdout, "NULL str %to\n", &len, (const char*)NULL, 16);
    if (status != OK) fprintf(stderr, "Ошибка выполнения %d.\n", status); 
  
    int x = 0x01020304;
    status = overfprint(stdout, "%mi\n", &len, x);
    if (status != OK) fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    double d = 1.0;
    status = overfprint(stdout, "1.0: %md\n", &len, d);
    if (status != OK) fprintf(stderr, "Ошибка выполнения %d.\n", status); 
    
    status = overfprint(stdout, "%d %s %.2f\n", &len, 42, "hi", 3.14159);
    if (status != OK) fprintf(stderr, "Ошибка выполнения %d.\n", status); 

    return status;
}


