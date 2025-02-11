#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <pthread.h>
#include <string.h>
#include <stdbool.h>
#include <complex.h>

#define MAX_INPUT 128

int row, column;
int **arr_int_1, **arr_int_2;
double **arr_float_1, **arr_float_2;
double complex **arr_complex_1, **arr_complex_2;
bool INT = false, FLO = false, COM = false;

typedef struct {
    pthread_mutex_t mutex;
    char input_1[MAX_INPUT];
    char input_2[MAX_INPUT];
    char input_3[MAX_INPUT];
} SharedMemory;

void pick(const char *input) {
    int i = 0;
    while (input[i] != ':') {
        i++;
    }
    i++;

    while (input[i] != '\0') {
        if (input[i] >= '0' && input[i] <= '9') {
            if (input[i + 1] == '.' && (input[i + 2] >= '0' && input[i + 2] <= '9')) {
                FLO = true;
                break;
            }
            else if (input[i + 1] == ','){
                INT = true;
                break;
            }
        }
        else if (input[i] == 'I' || input[i] == 'i') {
            COM = true;
            break;
        }
        i++;
    }
}

void get_row_column(const char *input) {
    int i = 0, j = 0;
    bool column_ = false, row_ = false;
    char num[MAX_INPUT];

    while (input[i] != '\0') {
        if (input[i] >= '0' && input[i] <= '9') {
            num[j] = input[i];
            j++;
        }
        else if (column_) {
            num[j] = '\0';
            column = atoi(num);
            break;
        }
        else if (row_) {
            num[j] = '\0';
            row = atoi(num);
            memset(num, 0, MAX_INPUT);
            row_ = false, column_ = true;
            j = 0;
        }
        i++;
        row_ = true;
    }
}

void make_metrics_int(int **arr, const char *input) {
    int i = 0, j = 0, z = 0;
    char num[MAX_INPUT];
    int num_index = 0;

    while (input[i] != ':') {
        i++;
    }
    i++;

    while (input[i] != '\0') {
        if (input[i] >= '0' && input[i] <= '9') {
            num[num_index] = input[i];
            num_index++;
        }
        else if (input[i] == ',' || input[i] == '\0') {
            num[num_index] = '\0';
            arr[j][z] = atoi(num);
            z++;
            if (z >= column) {
                z = 0;
                j++;
            }
            num_index = 0;
        }
        i++;
    }
    if (num_index > 0) {
        num[num_index] = '\0';
        arr[j][z] = atoi(num);
    }
}

void make_metrics_float(double **arr, const char *input) {
    int i = 0, j = 0, z = 0;
    char num[MAX_INPUT];
    int num_index = 0;

    while (input[i] != ':') {
        i++;
    }
    i++;

    while (input[i] != '\0') {
        if ((input[i] >= '0' && input[i] <= '9') || input[i] == '.') {
            num[num_index] = input[i];
            num_index++;
        }
        else if (input[i] == ',' || input[i] == '\0') {
            num[num_index] = '\0';
            arr[j][z] = atof(num);
            z++;
            if (z >= column) {
                z = 0;
                j++;
            }
            num_index = 0;
        }
        i++;
    }
    if (num_index > 0) {
        num[num_index] = '\0';
        arr[j][z] = atof(num);
    }
}

void make_metrics_complex(double complex **arr, const char *input) {
    int i = 0, j = 0, z = 0;
    char num_1[MAX_INPUT], num_2[MAX_INPUT];
    int num_index_1 = 0, num_index_2 = 0;

    while (input[i] != ':') {
        i++;
    }
    i++;

    while (input[i] != '\0') {
        if ((input[i] >= '0' && input[i] <= '9') || input[i] == '.' || input[i] == 'I' || input[i] == 'i') {
            num_1[num_index_1] = input[i];
            num_index_1++;
        }
        else if (input[i] == '+') {
            i++;
            while (input[i] != 'i') {
                num_2[num_index_2] = input[i];
                num_index_2++;
                i++;
            }
        }
        else if (input[i] == ',' || input[i] == '\0') {
            num_1[num_index_1] = '\0';
            num_2[num_index_2] = '\0';
            arr[j][z] = atof(num_1) + I * atof(num_2);
            z++;
            if (z >= column) {
                z = 0;
                j++;
            }
            num_index_1 = 0;
            num_index_2 = 0;
        }
        i++;
    }
    if (num_index_1 > 0) {
        num_1[num_index_1] = '\0';
        num_2[num_index_2] = '\0';
        arr[j][z] = atof(num_1) + I * atof(num_2);
    }
}

void add(void **arr_1, void **arr_2, char type) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (type == 'i') {
                ((int *)arr_1[i])[j]= ((int *)arr_1[i])[j] + ((int *)arr_2[i])[j];
            }
            else if (type == 'f') {
                ((double *)arr_1[i])[j]= ((double *)arr_1[i])[j] + ((double *)arr_2[i])[j];
            }
            else if (type == 'c') {
                ((double complex *)arr_1[i])[j]= ((double complex *)arr_1[i])[j] + ((double complex *)arr_2[i])[j];
            }
        }
    }
}

void sub(void **arr_1, void **arr_2, char type) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (type == 'i') {
                ((int *)arr_1[i])[j]= ((int *)arr_1[i])[j] - ((int *)arr_2[i])[j];
            }
            else if (type == 'f') {
                ((double *)arr_1[i])[j]= ((double *)arr_1[i])[j] - ((double *)arr_2[i])[j];
            }
            else if (type == 'c') {
                ((double complex *)arr_1[i])[j]= ((double complex *)arr_1[i])[j] - ((double complex *)arr_2[i])[j];
            }
        }
    }
}

void transpose(void **arr, char type) {
    printf("(%d,%d:",row, column);
    for (int i = 0; i < column; i++) {
        for (int j = 0; j < row; j++) {
            if (type == 'i') {
                printf("%d", ((int *)arr[j])[i]);
                if (i != row - 1 || j != column - 1) {
                    printf(",");
                }
            }
            else if (type == 'f') {
                printf("%.1f", ((double *)arr[j])[i]);
                if (i != row - 1 || j != column - 1) {
                    printf(",");
                }
            }
            else if (type == 'c') {
                printf("%.0f + %.0fi ", creal(((double complex *)arr[j])[i]), cimag(((double complex *)arr[j])[i]));
                if (i != row - 1 || j != column - 1) {
                    printf(",");
                }
            }
        }
    }
    printf(")\n");
}

void not(int **arr) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (arr[i][j] == 1) {
                arr[i][j] = 0;
            }
            else if (arr[i][j] == 0) {
                arr[i][j] = 1;
            }
            else {
                printf("ERR\n");
                break;
            }
        }
    }
}

void and(int **arr_1, int **arr_2) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (arr_1[i][j] == 1 && arr_2[i][j] == 1) {
                arr_1[i][j] = 1;
            }
            else if (arr_1[i][j] == 0 || arr_2[i][j] == 0) {
                arr_1[i][j] = 0;
            }
            else {
                printf("ERR\n");
                break;
            }
        }
    }
}

void or(int **arr_1, int **arr_2) {
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (arr_1[i][j] == 1 || arr_2[i][j] == 1) {
                arr_1[i][j] = 1;
            }
            else if (arr_1[i][j] == 0 && arr_2[i][j] == 0){
                arr_1[i][j] = 0;
            }
            else {
                printf("ERR\n");
                break;
            }
        }
    }
}

void print(void **arr, char type) {
    printf("(%d,%d:",row, column);
    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (type == 'i') {
                printf("%d", ((int *)arr[i])[j]);
                if (i != row - 1 || j != column - 1) {
                    printf(",");
                }
            }
            else if (type == 'f') {
                printf("%.1f", ((double *)arr[i])[j]);
                if (i != row - 1 || j != column - 1) {
                    printf(",");
                }
            }
            else if (type == 'c') {
                printf("%.0f+%.0fi", creal(((double complex *)arr[i])[j]), cimag(((double complex *)arr[i])[j]));
                if (i != row - 1 || j != column - 1) {
                    printf(",");
                }
            }
        }
    }
    printf(")\n");
}

void mul(void **arr_1, void **arr_2, char type) {
    void **arr_3;
    arr_3 = (void **)malloc(row * sizeof(void *));
    for (int i = 0; i < row; i++) {
        if (type == 'i') {
            arr_3[i] = (int *)malloc(column * sizeof(int));
        }
        else if (type == 'f') {
            arr_3[i] = (double *)malloc(column * sizeof(double));
        }
        else if (type == 'c') {
            arr_3[i] = (double complex *)malloc(column * sizeof(double complex));
        }
    }

    int sum_1;
    double sum_2;
    double complex sum_3;

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (type == 'i') {
                sum_1 = 0;
                for (int z = 0; z < column; z++) {
                    sum_1 += ((int *)arr_1[i])[z] * ((int *)arr_2[z])[j];
                }
                ((int *)arr_3[i])[j] = sum_1;
            }
            else if (type == 'f') {
                sum_2 = 0.0;
                for (int z = 0; z < column; z++) {
                    sum_2 += ((double *)arr_1[i])[z] * ((double *)arr_2[z])[j];
                }
                ((double *)arr_3[i])[j] = sum_2;
            }
            else if (type == 'c') {
                sum_3 = 0.0 + 0.0*I;
                for (int z = 0; z < column; z++) {
                    sum_3 += ((double complex *)arr_1[i])[z] * ((double complex *)arr_2[z])[j];
                }
                ((double complex *)arr_3[i])[j] = sum_3;
            }
        }
    }

    for (int i = 0; i < row; i++) {
        for (int j = 0; j < column; j++) {
            if (type == 'i') {
                ((int *)arr_int_1[i])[j] = ((int *)arr_3[i])[j];
            } else if (type == 'f') {
                ((double *)arr_float_1[i])[j] = ((double *)arr_3[i])[j];
            } else if (type == 'c') {
                ((double complex *)arr_complex_1[i])[j] = ((double complex *)arr_3[i])[j];
            }
        }
    }

    for (int i = 0; i < row; i++) {
        free(arr_3[i]);
    }
    free(arr_3);
}

void free_up(void **arr, char type) {
    for (int i = 0; i < row; i++) {
        if (type == 'i') {
            free((int *)arr[i]);
        }
        else if (type == 'f') {
            free((double *)arr[i]);
        }
        else if (type == 'c') {
            free((double complex *)arr[i]);
        }
    }
    free(arr);
}

int main() {
    key_t key = ftok("shmfile", 65);
    int shmid = shmget(key, sizeof(SharedMemory), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(1);
    }
    SharedMemory *shm = (SharedMemory *) shmat(shmid, NULL, 0);
    if (shm == (void *) -1) {
        perror("shmat");
        exit(1);
    }

    pthread_mutex_lock(&shm->mutex);

    get_row_column(shm->input_1);
    pick(shm->input_1);

    if (INT) {
        arr_int_1 = (int **) malloc(row * sizeof(int *));
        for (int i = 0; i < row; i++) {
            arr_int_1[i] = (int *) malloc(column * sizeof(int));
        }
        make_metrics_int(arr_int_1, shm->input_1);

    } else if (FLO) {
        arr_float_1 = (double **) malloc(row * sizeof(double *));
        for (int i = 0; i < row; i++) {
            arr_float_1[i] = (double *) malloc(column * sizeof(double));
        }
        make_metrics_float(arr_float_1, shm->input_1);

    } else if (COM) {
        arr_complex_1 = (double complex **) malloc(row * sizeof(double complex *));
        for (int i = 0; i < row; i++) {
            arr_complex_1[i] = (double complex *) malloc(column * sizeof(double complex));
        }
        make_metrics_complex(arr_complex_1, shm->input_1);
    }

    //******************************************************************************************//

    if (strcmp(shm->input_2, "NOT") == 0 || strcmp(shm->input_2, "not") == 0) {
        not(arr_int_1);
        print((void **) arr_int_1, 'i');
    } else if (strcmp(shm->input_2, "TRANSPOSE") == 0 || strcmp(shm->input_2, "transpose") == 0) {
        if (INT) {
            transpose((void **) arr_int_1, 'i');
        } else if (FLO) {
            transpose((void **) arr_float_1, 'f');
        } else if (COM) {
            transpose((void **) arr_complex_1, 'c');
        }
    } else {
        get_row_column(shm->input_2);
        if (INT) {
            arr_int_2 = (int **) malloc(row * sizeof(int *));
            for (int i = 0; i < row; i++) {
                arr_int_2[i] = (int *) malloc(column * sizeof(int));
            }
            make_metrics_int(arr_int_2, shm->input_2);

        } else if (FLO) {
            arr_float_2 = (double **) malloc(row * sizeof(double *));
            for (int i = 0; i < row; i++) {
                arr_float_2[i] = (double *) malloc(column * sizeof(double));
            }
            make_metrics_float(arr_float_2, shm->input_2);

        } else if (COM) {
            arr_complex_2 = (double complex **) malloc(row * sizeof(double complex *));
            for (int i = 0; i < row; i++) {
                arr_complex_2[i] = (double complex *) malloc(column * sizeof(double complex));
            }
            make_metrics_complex(arr_complex_2, shm->input_2);
        }

        //******************************************************************************************//

        int operation;
        if (INT) {
            operation = 1;
        }
        else if (FLO) {
            operation = 2;
        }
        else if (COM) {
            operation = 3;
        }

        switch (operation) {
            case 1:
                if (strcmp(shm->input_3,"ADD") == 0 || strcmp(shm->input_3, "add") == 0) {
                    add((void **)arr_int_1, (void **)arr_int_2, 'i');
                    print((void **)arr_int_1, 'i');
                }
                else if (strcmp(shm->input_3,"SUB") == 0 || strcmp(shm->input_3, "sub") == 0) {
                    sub((void **)arr_int_1, (void **)arr_int_2, 'i');
                    print((void **)arr_int_1, 'i');
                }
                else if (strcmp(shm->input_3, "AND") == 0 || strcmp(shm->input_3, "and") == 0) {
                    and(arr_int_1,arr_int_2);
                    print((void **)arr_int_1, 'i');
                }
                else if (strcmp(shm->input_3, "OR") == 0 || strcmp(shm->input_3, "or") == 0) {
                    or(arr_int_1,arr_int_2);
                    print((void **)arr_int_1, 'i');
                }
                else if (strcmp(shm->input_3, "MUL") == 0 || strcmp(shm->input_3, "mul") == 0) {
                    mul((void **)arr_int_1, (void **)arr_int_2, 'i');
                    print((void **)arr_int_1, 'i');
                }
                break;
            case 2:
                if (strcmp(shm->input_3,"ADD") == 0 || strcmp(shm->input_3, "add") == 0) {
                    add((void **)arr_float_1, (void **)arr_float_2, 'f');
                    print((void **)arr_float_1, 'f');
                }
                else if (strcmp(shm->input_3,"SUB") == 0 || strcmp(shm->input_3, "sub") == 0) {
                    sub((void **)arr_float_1, (void **)arr_float_2, 'f');
                    print((void **)arr_float_1, 'f');
                }
                else if (strcmp(shm->input_3, "MUL") == 0 || strcmp(shm->input_3, "mul") == 0) {
                    mul((void **)arr_float_1, (void **)arr_float_2, 'f');
                    print((void **)arr_float_1, 'f');
                }
                break;
            case 3:
                if (strcmp(shm->input_3,"ADD") == 0 || strcmp(shm->input_3, "add") == 0) {
                    add((void **)arr_complex_1, (void **)arr_complex_2, 'c');
                    print((void **)arr_complex_1, 'c');
                }
                else if (strcmp(shm->input_3,"SUB") == 0 || strcmp(shm->input_3, "sub") == 0) {
                    sub((void **)arr_complex_1, (void **)arr_complex_2, 'c');
                    print((void **)arr_complex_1, 'c');
                }
                else if (strcmp(shm->input_3, "MUL") == 0 || strcmp(shm->input_3, "mul") == 0) {
                    mul((void **)arr_complex_1, (void **)arr_complex_2, 'c');
                    print((void **)arr_complex_1, 'c');
                }
                break;
            default:
                printf("ERR\n");
        }
        if (INT) {
            free_up((void **)arr_int_1, 'i');
        }
        else if (FLO) {
            free_up((void **)arr_float_1, 'f');
        }
        else if (COM) {
            free_up((void **)arr_complex_1, 'c');
        }
    }

    if (INT) {
        free_up((void **)arr_int_2, 'i');
    }
    else if (FLO) {
        free_up((void **)arr_float_2, 'f');
    }
    else if (COM) {
        free_up((void **)arr_complex_2, 'c');
    }

    pthread_mutex_unlock(&shm->mutex);

    if (shmdt(shm) == -1) {
        perror("shmdt");
        exit(1);
    }

    return 0;
}
