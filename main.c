#include <stdio.h>
#include <string.h>
#include <time.h>
#include "matrix.h"
#include "strassen.h"

int main(void)
{
    struct timespec start, end;
    long nsecs, secs;
    matrix_t A, B;
    matrix_t C;

    allocate(&A);
    allocate(&B);
    allocate(&C);

    scanf("%d %d", &A.row, &A.col);
    read(&A);
    scanf("%d %d", &B.row, &B.col);
    read(&B);

    clock_gettime(CLOCK_MONOTONIC, &start);
    naive(&C, &A, &B);
    clock_gettime(CLOCK_MONOTONIC, &end);

    secs = end.tv_sec - start.tv_sec;
    nsecs = end.tv_nsec - start.tv_nsec;
    if (nsecs < 0) {
        secs--;
        nsecs += 1000000000;
    }
    printf("naive: %f s", (secs * 1e9 + nsecs) / 1e9);
    print(&C);
    puts("");

    // reset C
    memset(C.data, 0, sizeof *C.data * ROWSIZE);

    clock_gettime(CLOCK_MONOTONIC, &start);
    strassen(&C, &A, &B);
    clock_gettime(CLOCK_MONOTONIC, &end);

    secs = end.tv_sec - start.tv_sec;
    nsecs = end.tv_nsec - start.tv_nsec;
    if (nsecs < 0) {
        secs--;
        nsecs += 1000000000;
    }
    printf("strassen: %f s", (secs * 1e9 + nsecs) / 1e9);
    print(&C);

    free_memory(&A);
    free_memory(&B);
    free_memory(&C);

    return 0;
}
