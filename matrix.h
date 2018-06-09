#ifndef _MATRIX_H
#define _MATRIX_H

#define ROWSIZE 10
#define COLSIZE 10

typedef struct matrix {
    int data[ROWSIZE][COLSIZE];
    int row, col;
} matrix_t;

typedef struct sub_matrix {
    int data[ROWSIZE / 2 + 1][COLSIZE / 2 + 1];
    int row, col;
    int start_pos_i, start_pos_j;
} sub_matrix_t;

typedef struct Args {
    matrix_t *C;
    int block_i,  block_j;
} Args_t;

void read(matrix_t *Mat);
void print(matrix_t *Mat);
void print_sub(sub_matrix_t *Mat);
void addition(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B);
void multiply(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B);
void* naive_thread(void *arg);
void naive(matrix_t *C, matrix_t *A, matrix_t *B);

#endif
