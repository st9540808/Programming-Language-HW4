#include <stdio.h>
#include "matrix.h"

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

    // print(&A);
    // printf("\n");
    // print(&B);
    // printf("\n");
    print(&C);

    free_memory(&A);
    free_memory(&B);
    free_memory(&C);

    return 0;
}
