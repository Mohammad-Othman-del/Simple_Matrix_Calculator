#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <complex.h>
#include <stdbool.h>

#define MAX_INPUT 128

typedef enum { INTEGER, REAL, COMPLEX } MatrixType;

typedef struct {
    void ***data;
    int row;
    int column;
    MatrixType type;
} Matrix;

typedef struct {
    Matrix matrix1;
    Matrix matrix2;
    char operation;
    Matrix result;
} ThreadData;

MatrixType type(const char *input) {
    if (strstr(input, "i") != NULL) {
        return COMPLEX;
    }
    if (strstr(input, ".") != NULL) {
        return REAL;
    }
    return INTEGER;
}

Matrix parse_matrix(const char *input) {
    Matrix matrix;
    int rows, cols;

    if (sscanf(input, "(%d,%d:", &rows, &cols) != 2) {
        fprintf(stderr, "Invalid matrix format.\n");
        exit(EXIT_FAILURE);
    }

    matrix.row = rows;
    matrix.column = cols;
    matrix.type = type(input);
    matrix.data = malloc(rows * sizeof(void **));
    for (int i = 0; i < rows; i++) {
        matrix.data[i] = malloc(cols * sizeof(void *));
    }

    char *dataStr = strchr(input, ':') + 1;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            switch (matrix.type) {
                case INTEGER:
                    matrix.data[i][j] = malloc(sizeof(int));
                    sscanf(dataStr, "%d", (int *)matrix.data[i][j]);
                    break;
                case REAL:
                    matrix.data[i][j] = malloc(sizeof(double));
                    sscanf(dataStr, "%lf", (double *)matrix.data[i][j]);
                    break;
                case COMPLEX:
                    matrix.data[i][j] = malloc(sizeof(double complex));
                    double real, imag;
                    sscanf(dataStr, "%lf%lf%*c", &real, &imag);
                    *((double complex *)matrix.data[i][j]) = real + imag * I;
                    break;
            }
            dataStr = strchr(dataStr, ',') + 1;
        }
    }

    return matrix;
}

void *performOperation(void *arg) {
    ThreadData *data = (ThreadData *)arg;
    Matrix result;
    result.row = data->matrix1.row;
    result.column = data->matrix1.column;
    result.type = data->matrix1.type;
    result.data = malloc(result.row * sizeof(void **));
    for (int i = 0; i < result.row; i++) {
        result.data[i] = malloc(result.column * sizeof(void *));
    }

    for (int i = 0; i < result.row; i++) {
        for (int j = 0; j < result.column; j++) {
            switch (data->matrix1.type) {
                case INTEGER:
                    result.data[i][j] = malloc(sizeof(int));
                    if (data->operation == '+') {
                        *((int *)result.data[i][j]) = *((int *)data->matrix1.data[i][j]) + *((int *)data->matrix2.data[i][j]);
                    }
                    else if (data->operation == '*') {
                        for (int z = 0; z < result.row; z++) {
                            *((int *)result.data[i][j]) += *((int *)data->matrix1.data[i][z]) * *((int *)data->matrix2.data[z][j]);
                        }
                    }
                    else if (data->operation == '&') {
                        if (*(int *)data->matrix1.data[i][j] == 1 && *(int *) data->matrix2.data[i][j] == 1) {
                            *(int *)result.data[i][j] = 1;
                        }
                        else {
                            *(int *)result.data[i][j] = 0;
                        }
                    }
                    else if (data->operation == '|') {
                        if (*(int *)data->matrix1.data[i][j] == 1 || *(int *) data->matrix2.data[i][j] == 1) {
                            *(int *)result.data[i][j] = 1;
                        }
                        else {
                            *(int *)result.data[i][j] = 0;
                        }
                    }
                    break;
                case REAL:
                    result.data[i][j] = malloc(sizeof(double));
                    if (data->operation == '+') {
                        *((double *)result.data[i][j]) = *((double *)data->matrix1.data[i][j]) + *((double *)data->matrix2.data[i][j]);
                    }
                    else if (data->operation == '*') {
                        for (int z = 0; z < data->matrix1.row; z++) {
                            *((double *)result.data[i][j]) += *((double *)data->matrix1.data[i][z]) * *((double *)data->matrix2.data[z][j]);
                        }
                    }
                    break;
                case COMPLEX:
                    result.data[i][j] = malloc(sizeof(double complex));
                    if (data->operation == '+') {
                        *((double complex *)result.data[i][j]) = *((double complex *)data->matrix1.data[i][j]) + *((double complex *)data->matrix2.data[i][j]);
                    }
                    else if (data->operation == '*') {
                        for (int z = 0; z < data->matrix1.row; z++) {
                            *((double complex *)result.data[i][j]) += *((double complex *)data->matrix1.data[i][z]) * *((double complex *)data->matrix2.data[z][j]);
                        }
                    }
                    break;
            }
        }
    }

    data->result = result;
    return NULL;
}

void print(Matrix matrix) {
    printf("(%d,%d:", matrix.row, matrix.column);
    for (int i = 0; i < matrix.row; i++) {
        for (int j = 0; j < matrix.column; j++) {
            switch (matrix.type) {
                case INTEGER:
                    printf("%d", *((int *)matrix.data[i][j]));
                    if (i != matrix.row - 1 || j != matrix.column - 1) {
                        printf(",");
                    }
                    break;
                case REAL:
                    printf("%.1f", *((double *)matrix.data[i][j]));
                    if (i != matrix.row - 1 || j != matrix.column - 1) {
                        printf(",");
                    }
                    break;
                case COMPLEX:
                    printf("%.0f+%.0fi", creal(*((double complex *)matrix.data[i][j])), cimag(*((double complex *)matrix.data[i][j])));
                    if (i != matrix.row - 1 || j != matrix.column - 1) {
                        printf(",");
                    }
                    break;
            }
        }
    }
}

void free_up(Matrix matrix) {
    for (int i = 0; i < matrix.row; i++) {
        for (int j = 0; j < matrix.column; j++) {
            free(matrix.data[i][j]);
        }
        free(matrix.data[i]);
    }
    free(matrix.data);
}


int main() {
    char input[MAX_INPUT];
    char operation[0];
    int num_of_matrices = 0;
    Matrix *matrices = NULL;

    while(1) {
        while (true) {
            fgets(input, sizeof(input), stdin);
            input[strcspn(input, "\n")] = 0;

            if (strcmp(input, "ADD") == 0 || strcmp(input, "MUL") == 0 ||
                strcmp(input, "AND") == 0 || strcmp(input, "OR") == 0 ||
                    strcmp(input, "END") == 0) {
                if (strcmp(input, "ADD") == 0) {
                    operation[0] = '+';
                }
                if (strcmp(input, "MUL") == 0) {
                    operation[0] = '*';
                }
                if (strcmp(input, "AND") == 0) {
                    operation[0] = '&';
                }
                if (strcmp(input, "OR") == 0) {
                    operation[0] = '|';
                }
                break;
            }

            matrices = realloc(matrices, (num_of_matrices + 1) * sizeof(Matrix));
            matrices[num_of_matrices] = parse_matrix(input);
            num_of_matrices++;
        }

        if (strcmp(input, "END") == 0) {
            break;
        }

        while (num_of_matrices > 1) {
            int numPairs = num_of_matrices / 2;
            pthread_t *threads = malloc(numPairs * sizeof(pthread_t));
            ThreadData *threadData = malloc(numPairs * sizeof(ThreadData));
            Matrix *results = malloc(numPairs * sizeof(Matrix));

            for (int i = 0; i < numPairs; i++) {
                threadData[i].matrix1 = matrices[i * 2];
                threadData[i].matrix2 = matrices[i * 2 + 1];
                threadData[i].operation = operation[0];

                pthread_create(&threads[i], NULL, performOperation, &threadData[i]);
            }

            for (int i = 0; i < numPairs; i++) {
                pthread_join(threads[i], NULL);
                results[i] = threadData[i].result;
                free_up(matrices[i * 2]);
                free_up(matrices[i * 2 + 1]);
            }

            free(matrices);
            num_of_matrices = numPairs;
            matrices = results;

            free(threadData);
            free(threads);
        }

        print(matrices[0]);
        printf(")\n");

        free_up(matrices[0]);
    }
    free(matrices);
    return 0;
}