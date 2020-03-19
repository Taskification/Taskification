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

int fibo_accl(int n, unsigned int max_rank)
{

    if( n < 2)
    {
        return n;
    }

    printf("%d\n",max_rank );

    if(2 < max_rank)
    {
                
        int new_max_rank = max_rank - 2;
        /* Here we split on two tasks */
        /* offload using RPC */
        MPI_Datatype types[2] = {MPI_INT, MPI_INT};

        MPI_Request rpcs[2];

        int send1 = n - 1;
        int recv1;
        void * params1[2] = {&send1, &new_max_rank};

        MPIX_Ioffload(params1,
                      types,
                      2,
                      &recv1,
                      MPI_INT,
                      "fibo_accl",
                     1,
                      &rpcs[0]);

        MPI_Request rpc2;
        int send2 = n - 2;
        int recv2;
        void * params2[2] = {&send2, &new_max_rank};
    
        MPIX_Ioffload(params2,
                      types,
                      2,
                      &recv2,
                      MPI_INT,
                      "fibo_accl",
                      2,
                      &rpcs[1]);
        MPI_Waitall(2, rpcs, MPI_STATUSES_IGNORE);

        return recv1 + recv2;
    }
    else
    {
        return fibo_accl(n - 1, max_rank ) + fibo_accl(n - 2,  max_rank );
    }
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
        printf("Fib(%d) = %d %d %d\n", i, fibo(i), fibo_accl(i, size), fibo_lin(i));
    }

    MPI_Barrier(MPI_COMM_WORLD);

    MPIX_Accelerator_finalize();
    MPI_Finalize();

    return 0;
}