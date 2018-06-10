#include <stdio.h>
#include <string.h>
#include "matrix.h"
#include "strassen.h"

int main(void)
{
    matrix_t A, B;
    matrix_t C;

    allocate(&A);
    allocate(&B);
    allocate(&C);

    scanf("%d %d", &A.row, &A.col);
    read(&A);
    scanf("%d %d", &B.row, &B.col);
    read(&B);

    naive(&C, &A, &B);
    print(&C);
    puts("");

    // reset C
    memset(C.data, 0, sizeof *C.data * ROWSIZE);

    strassen(&C, &A, &B);
    print(&C);

    free_memory(&A);
    free_memory(&B);
    free_memory(&C);

    return 0;
}
