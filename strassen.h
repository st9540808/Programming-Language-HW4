#ifndef _STRASSEN_H
#define _STRASSEN_H

#include "matrix.h"

void strassen(matrix_t *C, matrix_t *A, matrix_t *B);
void strassen_opt(matrix_t *C, matrix_t *A, matrix_t *B);

#endif
