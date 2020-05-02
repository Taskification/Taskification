#include <mpi_accelerator.h>
#include <stdio.h>

int pure()
{
	return 5;
}

int ft_pure () 
{
	int a = 23;
	int b = 1;
	return a + b ;
}

int MAX(int X, int Y)
{
	if (X>Y) return X;
	else return Y;
}


int main(int argc, char ** argv)
{
	int sup;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &sup);
	MPIX_Accelerator_init();

	int rank, size;

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

	printf("%d/%d\n", rank, size);

	if( rank == 0 )
	{

		int i;


		for(i=1; i < size; i++)
		{

			int ret;

			// appel la fonction MAX

			MPI_Datatype param[2] = {MPI_INT, MPI_INT};
                void* buffers[2] = {&i, &size};		

			MPIX_Offload(buffers,
				     param,
				     2,
				     &ret,
				     MPI_INT,
			         "MAX",
				     i);

			printf("Call MAX between %d and %d on %d returned %d\n",i, size, i, ret);

			// appel la fonction pure

			MPIX_Offload(NULL,
				     NULL,
				     0,
				     &ret,
				     MPI_INT,
			         "pure",
				     i);

			printf("Call pure on %d returned %d\n", i, ret);

			//appel la fonction ft_pure

			MPIX_Offload(NULL,
				     NULL,
				     0,
				     &ret,
				     MPI_INT,
			         "ft_pure",
				     i);



			printf("Call ft_pure on %d returned %d\n", i, ret);
		}
	}



	MPIX_Accelerator_finalize();	
	MPI_Finalize();

	return 0;
}
