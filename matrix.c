#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include "matrix.h"

static sub_matrix_t subA[2][2], subB[2][2];

void read(matrix_t *Mat)
{
    int elem;
    for (int i = 0; i < Mat->row; i++) {
        for (int j = 0; j < Mat->col; j++)
            scanf("%d", &Mat->data[i][j]);
    }
}

void print(matrix_t *Mat)
{
    for (int i = 0; i < Mat->row; i++) {
        for (int j = 0; j < Mat->col; j++)
            printf("%3d ", Mat->data[i][j]);
        puts("");
    }
}

void print_sub(sub_matrix_t *Mat)
{
    for (int i = 0; i < Mat->row; i++) {
        for (int j = 0; j < Mat->col; j++)
            printf("%3d ", Mat->data[i][j]);
        puts("");
    }
}

void allocate(matrix_t *Mat)
{
    Mat->data = malloc(sizeof *Mat->data * ROWSIZE);
}

void allocate_sub(sub_matrix_t *Mat)
{
    Mat->data = malloc(sizeof *Mat->data * (ROWSIZE / 2));
}

void free_memory(matrix_t *Mat)
{
    free(Mat->data);
}

void free_memory_sub(sub_matrix_t *Mat)
{
    free(Mat->data);
}

void copy_sub(sub_matrix_t *dest, const sub_matrix_t *src)
{
    memcpy(dest->data, src->data, sizeof *src->data * src->row);
}

void addition(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B)
{
    for (int i = 0; i < A->row; i++) {
        for (int j = 0; j < A->col; j++)
            C->data[i][j] = A->data[i][j] + B->data[i][j];
    }
}

void subtract(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B)
{
    for (int i = 0; i < A->row; i++) {
        for (int j = 0; j < A->col; j++)
            C->data[i][j] = A->data[i][j] - B->data[i][j];
    }
}

/**
 * matrix multiply for submatrices
 */
void multiply(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B)
{
    int elem;
    for (int i = 0; i < A->row; i++) {
        for (int j = 0; j < A->col; j++) {
            elem = 0;
            for (int k = 0; k < A->col; k++)
                elem += A->data[i][k] * B->data[k][j];
            C->data[i][j] += elem;
        }
    }
}

// cache friendly version
void multiply_opt(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B)
{
    int elem;
    for (int i = 0; i < A->row; i++) {
        for (int k = 0; k < A->col; k++) {
            elem = A->data[i][k];
            for (int j = 0; j < A->col; j++)
                C->data[i][j] += elem * B->data[k][j];
        }
    }
}

///////////////////////////////////////////////////////////////////////////////////
// naive matrix multiplication
///////////////////////////////////////////////////////////////////////////////////

void* naive_thread(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    int block_i = Args->block_i, block_j = Args->block_j;
    matrix_t *C = Args->C;
    sub_matrix_t subMat;

    allocate_sub(&subMat);

    // initialize subMat
    subMat.row = subA[block_i][block_j].row;
    subMat.col = subA[block_i][block_j].col;
    subMat.start_pos_i = subA[block_i][block_j].start_pos_i;
    subMat.start_pos_j = subA[block_i][block_j].start_pos_j;
    memset(&subMat.data[0][0], 0, sizeof *subMat.data * (ROWSIZE / 2));

    for (int i = 0; i < 2; i++) {
        multiply(&subMat, &subA[block_i][i], &subB[i][block_j]);
    }
    
    // copy result to matrix C
    for (int i = 0; i < subMat.row; i++) {
        for (int j = 0; j < subMat.col; j++)
            C->data[subMat.start_pos_i + i][subMat.start_pos_j + j] = subMat.data[i][j];
    }

    free_memory_sub(&subMat);
}

void naive(matrix_t *C, matrix_t *A, matrix_t *B)
{
    // check if A & B are square matrices
    if (A->row != A->col || B->row != B->col || A->row != B->row)
        return;

    pthread_t thread[2][2];
    Args_t Args[2][2];

    C->row = A->row;
    C->col = A->col;

    // set size for submatrices
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            allocate_sub(&subA[i][j]);
            allocate_sub(&subB[i][j]);

            subA[i][j].row = subB[i][j].row = A->row / 2;
            subA[i][j].col = subB[i][j].col = A->col / 2;
            subA[i][j].start_pos_i = subB[i][j].start_pos_i = A->row / 2 * i;
            subA[i][j].start_pos_j = subB[i][j].start_pos_j = A->col / 2 * j;
        }
    }

    // split A and B to subA and subB respectively
    for (int i = 0; i < A->row; i++) {
        for (int j = 0; j < A->col; j++) {
            subA[i/(A->row/2)][j/(A->col/2)].data[i%(A->row/2)][j%(A->col/2)] =
               A->data[i][j];
            subB[i/(A->row/2)][j/(A->col/2)].data[i%(A->row/2)][j%(A->col/2)] =
               B->data[i][j];
        }
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            Args[i][j].C = C;
            Args[i][j].block_i = i;
            Args[i][j].block_j = j;
            pthread_create(&thread[i][j], NULL, naive_thread, (void*) &Args[i][j]);
        }
    }
    
    // wait for all threads to complete
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            pthread_join(thread[i][j], NULL);
        }
    }

    for (sub_matrix_t *a = &subA[0][0], *b = &subB[0][0]; a != &subA[2][0]; a++, b++) {
        free_memory_sub(a);
        free_memory_sub(b);
    }
}
