#include <mpi.h>
#include <mpi_accelerator.h>
#include <stdio.h>
#include <stdlib.h>


/* /!\ we need rdynamic or dlsym does not work */
int test_func(int a, int b)
{
    return a + b;
}


int main(int argc, char ** argv )
{
    int t_level;
    MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &t_level);

    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    MPIX_Accelerator_init();


    if(rank == 0)
    {
        int * response_buffer = malloc(size * sizeof(int));
        MPI_Request * work_requests = malloc(size * sizeof(MPI_Request));

        int i, iter;

        double total_time = 0.0;
        long total_rpcs = 0;


        for(iter = 0 ; iter < 100000 ; iter++)
        {
            double start = MPI_Wtime();

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

            double end = MPI_Wtime();

            total_time += end - start;

            total_rpcs += size;
        }

        free(response_buffer);
        free(work_requests);



        printf("RPC per second %g an rpc takes %g usec\n", 
                (double)total_rpcs/(double)total_time,
                1e6*(double)total_time/(double)total_rpcs);

    }



    MPIX_Accelerator_finalize();


    MPI_Finalize();


    return 0;
}
