#include <stdio.h>
#include <omp.h>

int main() {
    int n = 10000;  // Sum of first 10,000 numbers
    int sum = 0;

    omp_set_num_threads(4);  // Set number of threads

    #pragma omp parallel for reduction(+:sum)
    for (int i = 1; i <= n; i++) {
        sum += i;
    }

    printf("Sum of first %d numbers: %d\n", n, sum);
    return 0;
}
