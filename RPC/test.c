#include <mpi.h>
#include <mpi_accelerator.h>
#include <stdio.h>
#include <stdlib.h>


/* /!\ we need rdynamic or dlsym does not work */
int test_func(int a, int b)
{
    int myrank;
    MPI_Comm_rank(MPI_COMM_WORLD, &myrank);

    printf("\t Rank %d doing %d + %d\n", myrank, a , b);

    return a + b;
}


int main(int argc, char ** argv )
{
    int t_level;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &t_level);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    printf("Hello from %d / %d\n", rank, size);

    MPIX_Accelerator_init();


    if(rank == 0)
    {
        int * response_buffer = malloc(size * sizeof(int));
        MPI_Request * work_requests = malloc(size * sizeof(MPI_Request));

        int i;

        for( i = 0 ; i < size ; i++ )
        {
            MPI_Datatype param[2] = {MPI_INT, MPI_INT};
            void* buffers[2] = {&i, &i};

            MPIX_Ioffload(buffers,
                         param,
                         2,
                         &response_buffer[i],
                         MPI_INT,
                         "test_func",
                         i,
                         &work_requests[i]);
        }

        MPI_Waitall(size, work_requests, MPI_STATUSES_IGNORE);

        /* Now check values */
        int had_bad = 0;
        for( i = 0 ; i  < size ; i++)
        {
            if(response_buffer[i] != (i+i) )
            {
                fprintf(stderr, "Bad value on %d\n", i);
                had_bad |= 1;
            }
        }

        if(!had_bad)
        {
            printf("ALL OK\n");
        }

        free(response_buffer);
        free(work_requests);

    }


    MPIX_Accelerator_finalize();


    MPI_Finalize();


    return 0;
}