#include <stdio.h>
#include <string.h>
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

void addition(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B)
{
    for (int i = 0; i < A->row; i++) {
        for (int j = 0; j < A->col; j++)
            C->data[i][j] = A->data[i][j] + B->data[i][j];
    }
}

void multiply(sub_matrix_t *C, sub_matrix_t *A, sub_matrix_t *B)
{
    int elem;
    for (int i = 0; i < A->row; i++) {
        for (int j = 0; j < A->col; j++) {
            elem = 0;
            for (int k = 0; k < A->row; k++)
                elem += A->data[i][k] * B->data[k][j];
            // BE CAREFUL! the value of elem will be added to C->data
            C->data[i][j] += elem;
        }
    }
}

void naive_thread(matrix_t *C, int block_i, int block_j)
{
    sub_matrix_t subMat;

    // initialize subMat
    subMat.row = subA[block_i][block_j].row;
    subMat.col = subA[block_i][block_j].col;
    subMat.start_pos_i = subA[block_i][block_j].start_pos_i;
    subMat.start_pos_j = subA[block_i][block_j].start_pos_j;
    memset(&subMat.data[0][0], 0, sizeof subMat.data);

    printf("%d %d\n", subMat.start_pos_i, subMat.start_pos_j);

    for (int i = 0; i < 2; i++) {
        multiply(&subMat, &subA[block_i][i], &subB[i][block_j]);
    }
    
    // copy result to matrix C
    for (int i = 0; i < subMat.row; i++) {
        for (int j = 0; j < subMat.col; j++)
            C->data[subMat.start_pos_i + i][subMat.start_pos_j + j] = subMat.data[i][j];
    }
}

void naive(matrix_t *C, matrix_t *A, matrix_t *B)
{
    // check if A & B are square matrices
    if (A->row != A->col || B->row != B->col || A->row != B->row)
        return;

    C->row = A->row;
    C->col = A->col;

    // set size for submatrices
    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
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


    naive_thread(C, 0, 0);
    naive_thread(C, 0, 1);
    naive_thread(C, 1, 0);
    naive_thread(C, 1, 1);
}