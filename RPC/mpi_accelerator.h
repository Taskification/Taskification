#ifndef MPI_ACCELERATOR_H
#define MPI_ACCELERATOR_H

#include <mpi.h>

#define MPI_ACC_MAX_FN_NAME_LEN 32
#define MPI_ACC_MAX_SEND_COUNT 8

typedef enum
{
    MPIX_ACC_NULL = 0,
    MPIX_ACC_INT,
    MPIX_ACC_FLOAT,
    MPIX_ACC_DOUBLE
}MPIX_Acc_type;


int MPIX_Ioffload(void ** buffers,
                  MPI_Datatype * datatypes,
                  int sendcount,
                  void * recvbuf,
                  MPI_Datatype recvtype,
                  char * fname,
                  int dest,
                  MPI_Request * req);


int MPIX_Offload(void ** buffers,
                 MPI_Datatype * datatypes,
                 int sendcount,
                 void * recvbuf,
                 MPI_Datatype recvtype,
                 char * fname,
                 int dest);

int MPIX_Accelerator_init();

int MPIX_Accelerator_finalize();

#endif /* MPI_ACCELERATOR_H */
