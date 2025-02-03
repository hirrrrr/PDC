#include <stdio.h>
#include <omp.h>

int main()
{
    int n = 9999999;
    long long sum = 0;

    //printf("Enter n: ");
    //scanf("%d", &n);

    omp_set_num_threads(5);
    #pragma omp parallel for
    for(int i = 1; i <= n; i++)
    {
        #pragma omp critical
        {
            sum += i;
        }
    }

    printf("Sum of first %d natural numbers: %lld\n", n, sum);
}

///////////////////

#include <stdio.h>
#include <omp.h>

int main()
{
    int n = 6;
    int a[6] = {1, 3, 5, 7, 9, 11};
    int b[6] = {2, 4, 6, 8, 10, 12};
    int c[6];
    int d[6];
    c[0] = a[0] + b[0];

    for(int i = 1; i < n; i++)
        c[i] = a[i] + b[i] + c[i-1];

    int prefix_sum = 0;
    #pragma omp parallel
    {
        int thread_sum = 0;
        int thread_prefix = 0;

        #pragma omp for schedule(static)
        for(int i = 0; i < n; i++) {
            thread_sum += a[i] + b[i];
            d[i] = thread_sum;
        }

        #pragma omp single
        {
            for(int i = 1; i < n; i++)
            {
                d[i] += d[i-1];
            }
        }
    }

    printf("Sequential:\n");
    for(int i = 0; i < 6; i++)
    {
        printf("%d ", c[i]);
    }
    printf("\n");

    printf("Parallel:\n");
    for(int i = 0; i < 6; i++)
    {
        printf("%d ", d[i]);
    }
    printf("\n");

    int is = 0;
    for(int i = 0; i < 6; i++)
    {
        if(c[i] != d[i])
        {
            is = 1;
            break;
        }
    }

    if(is == 1)
        printf("\nFor sequential and parallel the results are not the same");
    else
        printf("\nFor sequential and parallel the results are the same");
}
//////////////////////////////////

#include<stdio.h>
#include<omp.h>

int main()
{
    int n = 10;
    long long sum = 0, sum1 = 0;

    omp_set_num_threads(8);
    #pragma omp parallel
    {
        for(int i = 1; i <= n; i++)
        {
            sum++;
        }
    }

    #pragma omp parallel for
    for(int i = 1; i <= n; i++)
    {
        #pragma omp critical
        sum1++;
    }

    printf("Race Condition: Sum: %lld\n", sum);
    printf("No Race Condition: Sum: %lld\n", sum1);
}
////////////////////////////////////////////

#include <stdio.h>
#include <omp.h>

int main()
{
    int sharedVar = 0, privateVar, n = 7;
    omp_set_num_threads(n);
    #pragma omp parallel private(privateVar) shared(sharedVar)
    {
        int tid = omp_get_thread_num();
        privateVar = tid;
        printf("Private variable : %d \t T_ID: %d\n", privateVar, tid);
        #pragma omp critical
        {
            sharedVar += tid;
        }
    }
    printf("Shared Variable value : %d\n", sharedVar);
}
///////////////////////////////////////////////

