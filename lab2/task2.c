#include <stdio.h>
#include <limits.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <float.h>
#include <math.h>
#include <sys/stat.h>

typedef enum {
    OK,
    ERR_FLAG,
    ERR_ARGS,
    ERR_OVERFLOW,
    ERR_NUM, 
    ERR_STRING, 
    ERR_ALLOC, 
    ERR_FILE
} Status;

typedef struct {
    unsigned int id;
    char *name;
    char *surname;
    float salary;
} Employee;

Status correct_flag(const char *flag, char *result) {
    if (flag == NULL) return ERR_FLAG;
    if (flag[0] != '-' && flag[0] != '/') return ERR_FLAG;
    if ((flag[1] != 'a' && flag[1] != 'd') || flag[2] != '\0') return ERR_FLAG;
    *result = flag[1];
    return OK;
}

Status correct_ull(const char *str, unsigned int *result) {
    if (!str || *str == '\0' || !result) return ERR_ARGS;
    if (str[0] == '-') return ERR_ARGS;
    char *endptr;
    unsigned long long val = strtoull(str, &endptr, 10);
    if (endptr == str ||*endptr != '\0') return ERR_ARGS;
    if (val > UINT_MAX) return ERR_ARGS;
    *result = (unsigned int)val;
    return OK;
}

int correct_float(const char *s, float *result) {
    if (s == NULL || *s == '\0') return ERR_ARGS;
    if (s[0] == '-') return ERR_FLAG;
    char *endptr = NULL;
    double val = strtof(s, &endptr);
    if (endptr == s || *endptr != '\0') return ERR_ARGS;
    if (isinf(val) || isnan(val)) return ERR_ARGS;
    if (val > FLT_MAX || val < 0.0f) return ERR_ARGS; 
    *result = (float)val;
    return OK;
}

Status correct_str( char *str) {
    if (!str || *str == '\0' || isspace(*str)) return ERR_ARGS;
    for (size_t i = 0; str[i] != '\0'; i++) {
        if (!((str[i] >= 'a' && str[i] <= 'z') || (str[i] >= 'A' && str[i] <= 'Z'))) return ERR_ARGS;
    }
    return OK;
}


int cmp_a(const void *a, const void *b) {
    const Employee *ea = (const Employee *)a;
    const Employee *eb = (const Employee *)b;
    if (ea->salary < eb->salary) return -1;
    if (ea->salary > eb->salary) return 1;


    int surname_cmp = strcmp(ea->surname, eb->surname);
    if (surname_cmp != 0) return surname_cmp;
    
    int name_cmp = strcmp(ea->name, eb->name);
    if (name_cmp != 0) return name_cmp;

    if (ea->id < eb->id) return -1;
    if (ea->id > eb->id) return 1;

    return 0;
}

int cmp_d(const void *a, const void *b) {
    return -cmp_a(a, b);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Некорректный ввод.\n");
        return ERR_ARGS;
    }

    const char *inpath = argv[1];
    const char *outpath = argv[3];
    
    char flag;
    Status status = correct_flag(argv[2], &flag);
    if (status != OK) {
        fprintf(stderr, "Неверный флаг.\n");
        return ERR_FLAG;
    }

    FILE *file_in = fopen(inpath, "r");
    if (!file_in) {
        fprintf(stderr, "Ошибка открытия файла.\n");
        return ERR_FILE;
    }

    struct stat stat_in, stat_out;
    int has_stat_in = (stat(inpath, &stat_in) == 0);
    int has_stat_out = (stat(outpath, &stat_out) == 0);

    if (has_stat_in && has_stat_out) {
        if (stat_in.st_dev == stat_out.st_dev && stat_in.st_ino == stat_out.st_ino) {
            fprintf(stderr, "Входной и выходной файлы не могут быть одним и тем же файлом.\n");
            fclose(file_in);
            return ERR_FILE;
        }
    }

    else if (strcmp(inpath, outpath) == 0) {
        fprintf(stderr, "Пути к входному и выходному файлам совпадают.\n");
        fclose(file_in);
        return ERR_FILE;
    }

    Employee *emp = NULL;
    size_t capacity = 0;
    size_t count = 0;

    char *tmp_id = NULL;
    char *tmp_name = NULL;
    char *tmp_surname = NULL;
    char *tmp_salary = NULL;

    while (fscanf(file_in, "%ms %ms %ms %ms", &tmp_id, &tmp_name, &tmp_surname, &tmp_salary) == 4){

        unsigned int c_id;
        float c_salary;
        Status status  = OK;
        if (correct_ull(tmp_id, &c_id) != OK) status = ERR_ARGS;
        else if (correct_str(tmp_name) != OK) status = ERR_ARGS;
        else if (correct_str(tmp_surname) != OK) status = ERR_ARGS;
        else if (correct_float(tmp_salary, &c_salary) != OK) status = ERR_ARGS;

        if (status != OK) {
            fprintf(stderr, "Некорректный ввод.\n");
            free(tmp_id);
            free(tmp_name);
            free(tmp_salary);
            free(tmp_surname);

            for (size_t i = 0; i < count; i++) {
                    free(emp[i].name);
                    free(emp[i].surname);
            }
            free(emp);
            fclose(file_in);
            return status;
        }

        free(tmp_id); 
        free(tmp_salary);
        tmp_id = NULL;
        tmp_salary = NULL;

        if (count >= capacity) {
            capacity = capacity == 0 ? 4 : capacity * 2;
            Employee *arr = (Employee *)realloc(emp, capacity * sizeof(Employee));

            if (!arr) {
                fprintf(stderr, "Не удалось выделить память.\n");
                free(tmp_name); free(tmp_surname);
                for (size_t i = 0; i < count; i++) {
                    free(emp[i].name);
                    free(emp[i].surname);
                }
                free(emp);
                fclose(file_in);
                return ERR_ALLOC;
            }
            emp = arr;
        }

        emp[count].id = c_id;
        emp[count].salary = c_salary;
        emp[count].name = tmp_name;
        emp[count].surname = tmp_surname;

        count++;

        tmp_name = NULL;
        tmp_surname = NULL;
    }

    if (!feof(file_in)) {
        free(tmp_id); free(tmp_name); free(tmp_surname); free(tmp_salary);
        fprintf(stderr, "Некорректный формат структуры файла.\n");
        for (size_t i = 0; i < count; i++) {
            free(emp[i].name);
            free(emp[i].surname);
        }
        free(emp);
        fclose(file_in);
        return ERR_FILE;
    }

    fclose(file_in);

    if (count > 1) {
        if (flag == 'a') {
            qsort(emp, count, sizeof(Employee), cmp_a);
        } else if (flag == 'd') {
            qsort(emp, count, sizeof(Employee), cmp_d);
        } else {
            fprintf(stderr, "Такого флага не существует.\n");
            return ERR_FLAG;
        }
    }

    FILE* file_out = fopen(outpath, "w");
    if (!file_out) {
        fprintf(stderr, "Ошибка открытия файла.\n"); 
        for (size_t i = 0; i < count; i++) {
            free(emp[i].name);
            free(emp[i].surname);
        }
        free(emp);
        return ERR_FILE;
    }

    for (size_t i = 0; i < count; i++) {
        fprintf(file_out, "%u %s %s %.2f\n", emp[i].id, emp[i].name, emp[i].surname, emp[i].salary);
    }

    fclose(file_out);

    for (size_t i = 0; i < count; i++) {
        free(emp[i].name);
        free(emp[i].surname);
    }
    free(emp);

    return OK;
}