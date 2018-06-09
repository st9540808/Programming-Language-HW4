#include <stdio.h>
#include "matrix.h"

int main(int argc, char const *argv[])
{
    matrix_t A, B;
    matrix_t C;

    scanf("%d %d", &A.row, &A.col);
    read(&A);
    scanf("%d %d", &B.row, &B.col);
    read(&B);

    naive(&C, &A, &B);

    print(&A);
    printf("\n");
    print(&B);
    printf("\n");
    print(&C);

    return 0;
}
