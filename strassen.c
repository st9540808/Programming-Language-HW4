#include <pthread.h>
#include <string.h>
#include "matrix.h"

static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static sub_matrix_t subA[2][2], subB[2][2];
static sub_matrix_t p[8];

// signal p[i] is ready to use
static void signal_p(int i)
{
    pthread_mutex_lock(&mtx);
    p[i].done = 1;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mtx);
}

static void* p1(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    matrix_t *C = Args->C;
    sub_matrix_t right = { .row = C->row / 2, .col = C->col / 2 };

    allocate_sub(&right);
    subtract(&right, &subB[0][1], &subB[1][1]);
    multiply(&p[1], &subA[0][0], &right);

    signal_p(1);
    free_memory_sub(&right);
    pthread_exit(0);
}

static void* p2(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    matrix_t *C = Args->C;
    sub_matrix_t left = { .row = C->row / 2, .col = C->col / 2 };
    
    allocate_sub(&left);
    addition(&left, &subA[0][0], &subA[0][1]);
    multiply(&p[2], &left, &subB[1][1]);

    signal_p(2);
    free_memory_sub(&left);
    pthread_exit(0);
}

static void* p3(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    matrix_t *C = Args->C;
    sub_matrix_t left = { .row = C->row / 2, .col = C->col / 2 };
    
    allocate_sub(&left);
    addition(&left, &subA[1][0], &subA[1][1]);
    multiply(&p[3], &left, &subB[0][0]);

    signal_p(3);
    free_memory_sub(&left);
    pthread_exit(0);
}

static void* p4(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    matrix_t *C = Args->C;
    sub_matrix_t right = { .row = C->row / 2, .col = C->col / 2 };
    
    allocate_sub(&right);
    subtract(&right, &subB[1][0], &subB[0][0]);
    multiply(&p[4], &subA[1][1], &right);

    signal_p(4);
    free_memory_sub(&right);
    pthread_exit(0);
}

static void* p5(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    matrix_t *C = Args->C;
    sub_matrix_t right = { .row = C->row / 2, .col = C->col / 2 };
    sub_matrix_t left = right;
    
    allocate_sub(&right);
    allocate_sub(&left);
    addition(&right, &subB[0][0], &subB[1][1]);
    addition(&left, &subA[0][0], &subA[1][1]);
    multiply(&p[5], &left, &right);

    signal_p(5);
    free_memory_sub(&right);
    free_memory_sub(&left);
    pthread_exit(0);
}

static void* p6(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    matrix_t *C = Args->C;
    sub_matrix_t right = { .row = C->row / 2, .col = C->col / 2 };
    sub_matrix_t left = right;
    
    allocate_sub(&right);
    allocate_sub(&left);
    addition(&right, &subB[1][0], &subB[1][1]);
    subtract(&left, &subA[0][1], &subA[1][1]);
    multiply(&p[6], &left, &right);

    signal_p(6);
    free_memory_sub(&right);
    free_memory_sub(&left);
    pthread_exit(0);
}

static void* p7(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    matrix_t *C = Args->C;
    sub_matrix_t right = { .row = C->row / 2, .col = C->col / 2 };
    sub_matrix_t left = right;
    
    allocate_sub(&right);
    allocate_sub(&left);
    addition(&right, &subB[0][0], &subB[0][1]);
    subtract(&left, &subA[0][0], &subA[1][0]);
    multiply(&p[7], &left, &right);

    signal_p(7);
    free_memory_sub(&right);
    free_memory_sub(&left);
    pthread_exit(0);
}

static void* strassen_thread_C00(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    int block_i = Args->block_i, block_j = Args->block_j;
    matrix_t *C = Args->C;
    sub_matrix_t subMat = {
        .row = C->row / 2, .col = C->col / 2,
        .start_pos_i = subA[block_i][block_j].start_pos_i,
        .start_pos_j = subA[block_i][block_j].start_pos_j,
    };
    
    pthread_mutex_lock(&mtx);
    while (!(p[2].done == 1) || !(p[4].done == 1) ||
           !(p[5].done == 1) || !(p[6].done == 1))
        pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
    
    // copy result to matrix C
    for (int i = 0; i < subMat.row; i++) {
        for (int j = 0; j < subMat.col; j++)
            C->data[subMat.start_pos_i + i][subMat.start_pos_j + j] =
                p[5].data[i][j] + p[4].data[i][j] - p[2].data[i][j] + p[6].data[i][j];
    }
}

static void* strassen_thread_C01(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    int block_i = Args->block_i, block_j = Args->block_j;
    matrix_t *C = Args->C;
    sub_matrix_t subMat = {
        .row = C->row / 2, .col = C->col / 2,
        .start_pos_i = subA[block_i][block_j].start_pos_i,
        .start_pos_j = subA[block_i][block_j].start_pos_j,
    };

    pthread_mutex_lock(&mtx);
    while (!(p[1].done == 1) || !(p[2].done == 1))
        pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
    
    // copy result to matrix C
    for (int i = 0; i < subMat.row; i++) {
        for (int j = 0; j < subMat.col; j++)
            C->data[subMat.start_pos_i + i][subMat.start_pos_j + j] =
                p[1].data[i][j] + p[2].data[i][j];
    }
}

static void* strassen_thread_C10(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    int block_i = Args->block_i, block_j = Args->block_j;
    matrix_t *C = Args->C;
    sub_matrix_t subMat = {
        .row = C->row / 2, .col = C->col / 2,
        .start_pos_i = subA[block_i][block_j].start_pos_i,
        .start_pos_j = subA[block_i][block_j].start_pos_j,
    };

    pthread_mutex_lock(&mtx);
    while (!(p[3].done == 1) || !(p[4].done == 1))
        pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
    
    // copy result to matrix C
    for (int i = 0; i < subMat.row; i++) {
        for (int j = 0; j < subMat.col; j++)
            C->data[subMat.start_pos_i + i][subMat.start_pos_j + j] =
                p[3].data[i][j] + p[4].data[i][j];
    }
}

static void* strassen_thread_C11(void *arg)
{
    Args_t *Args = (Args_t *) arg;
    int block_i = Args->block_i, block_j = Args->block_j;
    matrix_t *C = Args->C;
    sub_matrix_t subMat = {
        .row = C->row / 2, .col = C->col / 2,
        .start_pos_i = subA[block_i][block_j].start_pos_i,
        .start_pos_j = subA[block_i][block_j].start_pos_j,
    };
    
    pthread_mutex_lock(&mtx);
    while (!(p[1].done == 1) || !(p[3].done == 1) ||
           !(p[5].done == 1) || !(p[7].done == 1))
        pthread_cond_wait(&cond, &mtx);
    pthread_mutex_unlock(&mtx);
    
    // copy result to matrix C
    for (int i = 0; i < subMat.row; i++) {
        for (int j = 0; j < subMat.col; j++)
            C->data[subMat.start_pos_i + i][subMat.start_pos_j + j] =
                p[1].data[i][j] + p[5].data[i][j] - p[3].data[i][j] - p[7].data[i][j];
    }
}

void strassen(matrix_t *C, matrix_t *A, matrix_t *B)
{
    // check if A & B are square matrices
    if (A->row != A->col || B->row != B->col || A->row != B->row)
        return;

    pthread_t thr[7], strassen_thr[2][2];
    Args_t p_args = { .C = C }, Args[2][2];
    void* (*p_thread[7])(void *) = { p1, p2, p3, p4, p5, p6, p7 };
    void* (*strassen_thread[2][2])(void *) = {
        { strassen_thread_C00, strassen_thread_C01 },
        { strassen_thread_C10, strassen_thread_C11 }
    };

    // set done flag to false and initialize
    for (int i = 1; i <= 7; i++) {
        p[i].done = 0;
        p[i].row = A->row / 2;
        p[i].col = A->col / 2;
        allocate_sub(&p[i]);
    }

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

    // calculate p[i]
    for (int i = 0; i < 7; i++) {
        pthread_create(&thr[i], NULL, p_thread[i], (void *) &p_args);    
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            Args[i][j].block_i = i;
            Args[i][j].block_j = j;
            Args[i][j].C = C;
            pthread_create(&strassen_thr[i][j], NULL,
                            strassen_thread[i][j], (void *) &Args[i][j]);
        }
    }

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            pthread_join(strassen_thr[i][j], NULL);
        }
    }

    for (sub_matrix_t *a = &subA[0][0], *b = &subB[0][0]; a != &subA[2][0]; a++, b++) {
        free_memory_sub(a);
        free_memory_sub(b);
    }

    for (int i = 1; i <= 7; i++)
        free_memory_sub(&p[i]);
}
