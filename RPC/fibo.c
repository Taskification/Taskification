#include <stdio.h>
#include <mpi_accelerator.h>

int fibo(int n)
{
    if( n < 2)
    {
        return n;
    }

    return fibo(n - 1) + fibo(n - 2);
}   


int fibo_lin(int n)
{
    int i = 0, j = 1, k;

    for(k = 0 ; k < n ; k++)
    {
        int tmp = i;
        i = j;
        j = j + tmp;

    }

    return i;
}


int main(int argc, char ** argv)
{
    MPI_Init(&argc, &argv);
    MPIX_Accelerator_init();

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int i;

    for( i = 0 ; i < 47 ; i++ )
    {
        printf("Fib(%d) = %d %d\n", i, fibo(i), fibo_lin(i));
    }

    MPIX_Accelerator_finalize();
    MPI_Finalize();

    return 0;
}