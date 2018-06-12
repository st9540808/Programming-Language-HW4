#ifndef _MATRIX_H
#define _MATRIX_H

#define ROWSIZE 4096
#define COLSIZE 4096

typedef struct matrix {
    int (*data)[COLSIZE];
    int row, col;
} matrix_t;

typedef struct sub_matrix {
    int (*data)[COLSIZE / 2];
    int row, col;
    int start_pos_i, start_pos_j;
    int done; // compatible for strassen's method
} sub_matrix_t;

typedef struct Args {
    matrix_t *C;
    int block_i,  block_j;
    void (*multiply)(sub_matrix_t *, sub_matrix_t *, sub_matrix_t *);
} Args_t;
#define DECLARE_MULTIPLY(arg) \
    void (*multiply)(sub_matrix_t *, sub_matrix_t *, sub_matrix_t *) = \
        (arg).multiply;

void read(matrix_t *Mat);
void print(matrix_t *Mat);
void print_sub(sub_matrix_t *Mat);
void allocate(matrix_t *Mat);
void allocate_sub(sub_matrix_t *Mat);
void free_memory(matrix_t *Mat);
void free_memory_sub(sub_matrix_t *Mat);
void copy_sub(sub_matrix_t *dest, const sub_matrix_t *src);
void clear(matrix_t *dest);
void clear_sub(sub_matrix_t *dest);
void addition(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B);
void subtract(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B);
void multiply_opt(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B);
void multiply(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B);
void naive(matrix_t *C, matrix_t *A, matrix_t *B);
void naive_opt(matrix_t *C, matrix_t *A, matrix_t *B);

#endif
