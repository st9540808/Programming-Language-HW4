#include <stdio.h>
#include <string.h>
#include <time.h>
#include "matrix.h"
#include "strassen.h"

#define PRINTTIME(str) { \
    secs = end.tv_sec - start.tv_sec; \
    nsecs = end.tv_nsec - start.tv_nsec; \
    if (nsecs < 0) { \
        secs--; \
        nsecs += 1000000000; \
    } \
    printf(str ": %f s\n", (secs * 1e9 + nsecs) / 1e9); \
}

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

    // clock_gettime(CLOCK_MONOTONIC, &start);
    // naive(&C, &A, &B);
    // clock_gettime(CLOCK_MONOTONIC, &end);
    // PRINTTIME("naive");
    // print(&C); puts("");

    // clear(&C);
    // clock_gettime(CLOCK_MONOTONIC, &start);
    // naive_opt(&C, &A, &B);
    // clock_gettime(CLOCK_MONOTONIC, &end);
    // PRINTTIME("naive_opt");
    // print(&C); puts("");

    // clear(&C);    
    // clock_gettime(CLOCK_MONOTONIC, &start);
    // strassen(&C, &A, &B);
    // clock_gettime(CLOCK_MONOTONIC, &end);
    // PRINTTIME("strassen");
    // print(&C); puts("");

    // clear(&C);    
    // clock_gettime(CLOCK_MONOTONIC, &start);
    strassen_opt(&C, &A, &B);
    // clock_gettime(CLOCK_MONOTONIC, &end);
    // PRINTTIME("strassen_opt");
    print(&C); // puts("");

    free_memory(&A);
    free_memory(&B);
    free_memory(&C);

    return 0;
}
