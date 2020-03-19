#include "mpi_accelerator.h"

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ffi.h>
#include <dlfcn.h>
#include <malloc.h>


MPI_Comm accelerator_comm;

static inline MPIX_Acc_type MPIX_Acc_type_from_mpi( MPI_Datatype type )
{
    if( type == MPI_DATATYPE_NULL )
        return MPIX_ACC_NULL;

    if( type == MPI_INT )
    return MPIX_ACC_INT;

    if( type == MPI_FLOAT )
        return MPIX_ACC_FLOAT;

    if( type == MPI_DOUBLE )
        return MPIX_ACC_DOUBLE;

    fprintf(stderr, "TYPE NOT SUPPORTED YET\n");
    abort();
}

/* TODO need more types */

static inline  MPI_Datatype MPIX_type_from_mpi_acc( MPIX_Acc_type type )
{
    switch(type)
    {
        case MPIX_ACC_NULL:
            return MPI_DATATYPE_NULL;
        case MPIX_ACC_INT:
            return MPI_INT;
        case MPIX_ACC_FLOAT:
            return MPI_FLOAT;
        case MPIX_ACC_DOUBLE:
            return MPI_DOUBLE;
    }

    fprintf(stderr, "TYPE NOT SUPPORTED YET\n");
    abort();
}

static inline size_t MPIX_Acc_type_extent( MPIX_Acc_type type )
{
    switch(type)
    {
        case MPIX_ACC_NULL:
            return 0;
        case MPIX_ACC_INT:
            return sizeof(int);
        case MPIX_ACC_FLOAT:
            return sizeof(float);
        case MPIX_ACC_DOUBLE:
            return sizeof(double);
    }

    fprintf(stderr, "TYPE NOT SUPPORTED YET\n");
    abort();
}


static inline char * MPIX_Acc_type_str( MPIX_Acc_type type )
{
    switch(type)
    {
        case MPIX_ACC_NULL:
            return "NULL";
        case MPIX_ACC_INT:
            return "int";
        case MPIX_ACC_FLOAT:
            return "float";
        case MPIX_ACC_DOUBLE:
            return "double";
    }

    fprintf(stderr, "TYPE NOT SUPPORTED YET\n");
    abort();
}


static inline ffi_type * ffi_type_from_MPIX_Acc_type( MPIX_Acc_type type )
{
    switch(type)
    {
        case MPIX_ACC_NULL:
            return &ffi_type_void;
        case MPIX_ACC_INT:
            return &ffi_type_sint;
        case MPIX_ACC_FLOAT:
            return &ffi_type_float;
        case MPIX_ACC_DOUBLE:
            return &ffi_type_double;
    }

    fprintf(stderr, "TYPE NOT SUPPORTED YET\n");
    abort();
}




struct accelerator_rpc_desc
{
    int source;
    int response_tag;
    char end_of_rpc;
    char function[MPI_ACC_MAX_FN_NAME_LEN];
    int send_count;
    MPIX_Acc_type send_types[MPI_ACC_MAX_SEND_COUNT];
    MPIX_Acc_type recv_type;
    char data[0];
};

void accelerator_rpc_desc_print(struct accelerator_rpc_desc * rpcd)
{
    printf("==========================\n");
    printf("Calling '%s'\n", rpcd->function);
    printf("In parameters (%d) :\n", rpcd->send_count);
    int i;
    for ( i = 0; i < rpcd->send_count; i++)
    {
        printf("\t%s\n", MPIX_Acc_type_str(rpcd->send_types[i]));
    }
    printf("Recv type: %s\n", MPIX_Acc_type_str(rpcd->recv_type));
    printf("From %d:%d\n", rpcd->source, rpcd->response_tag);
    printf("End of RPC flag %hd\n", (short)rpcd->end_of_rpc);
    printf("==========================\n");
}

struct accelerator_rpc_desc * accelerator_rpc_desc_eof()
{
    static struct accelerator_rpc_desc eof;

    memset(&eof, 0, sizeof(struct accelerator_rpc_desc));

    eof.end_of_rpc = 1;

    return &eof;
}

#define MPIX_ACC_STATIC_COMM_BUFF 512

static inline struct accelerator_rpc_desc * accelerator_rpc_desc_init(char * function,
                                                                      MPIX_Acc_type * send_types,
                                                                      void ** buffers,
                                                                      int send_count,
                                                                      MPIX_Acc_type recv_type,
                                                                      int end_of_rpc,
                                                                      size_t * out_overall_size,
                                                                      int * to_free)
                {
    /* 0 is reserved for RPC notification
       alt the rest is increasing tags */
    static int expected_response_tag = 1;
    static pthread_mutex_t tag_lock = PTHREAD_MUTEX_INITIALIZER;

    static __thread char tmp_buffer[MPIX_ACC_STATIC_COMM_BUFF];


    /* Compute overall size */
    size_t overall_size = sizeof(struct accelerator_rpc_desc);

    int i;

    for(i = 0 ; i < send_count; i++)
    {
        overall_size += MPIX_Acc_type_extent(send_types[i]);
    }

    struct accelerator_rpc_desc * rpcd = NULL;

    if( overall_size < MPIX_ACC_STATIC_COMM_BUFF)
    {
        rpcd = (struct accelerator_rpc_desc *)tmp_buffer;
        *to_free = 0;
    }
    else
    {
        rpcd = (struct accelerator_rpc_desc *)valloc(overall_size * sizeof(char));
        *to_free = 1;
    }

    *out_overall_size = overall_size;

    if(!rpcd)
    {
        perror("valloc");
        abort();
    }

    rpcd->response_tag = expected_response_tag;

    pthread_mutex_lock(&tag_lock);
    /* TODO can be exhausted */
    /* Do not use the 0 tag ! */
    expected_response_tag = (expected_response_tag + 1) % 50000 + 1;
    pthread_mutex_unlock(&tag_lock);

    MPI_Comm_rank(accelerator_comm, &rpcd->source);

    rpcd->end_of_rpc = end_of_rpc;

    if( strlen(function) >= MPI_ACC_MAX_FN_NAME_LEN)
    {
        fprintf(stderr, "Function name too long\n");
        abort();
    }

    snprintf(rpcd->function, MPI_ACC_MAX_FN_NAME_LEN, "%s", function);
    rpcd->send_count = send_count;

    if( send_count >= MPI_ACC_MAX_SEND_COUNT)
    {
        fprintf(stderr, "Maximum parameter count is %d\n", MPI_ACC_MAX_SEND_COUNT);
        abort();
    }

    memcpy(rpcd->send_types, send_types, send_count * sizeof(MPIX_Acc_type));
    rpcd->recv_type = recv_type;

    /* Is is now time to pack */

    size_t offset = 0;

    for(i = 0 ; i < send_count; i++)
    {
        size_t ext = MPIX_Acc_type_extent(send_types[i]);
        memcpy(rpcd->data + offset, buffers[i], ext);
        offset += ext;
    }

    return rpcd;
}


#define MPIX_ACC_MAX_PAYLOAD (2 * 1024 * 1024)
#define MPIX_ACC_MAX_RESP 1024

static inline void * _resolve_fname(char * fname)
{
    static __thread char * pfname = NULL;
    static __thread void * pfn = NULL;

    if(pfname)
    {
        if(!strcmp(pfname, fname))
        {
            return pfn;
        }
    }

    free(pfname);
    pfname = strdup(fname);
    pfn = dlsym(NULL, fname);

    return pfn;
}



static inline int accelerator_process_rpc(struct accelerator_rpc_desc * rpcd, void * result_buffer )
{
    ffi_cif cif;
    void * arg_ffi_v[MPI_ACC_MAX_SEND_COUNT];

    {
        //TODO: cache CIFs
        ffi_type * arg_ffi[MPI_ACC_MAX_SEND_COUNT];

        int i;

        size_t offset = 0;

        for( i = 0 ; i < rpcd->send_count; i++)
        {
            arg_ffi[i] = ffi_type_from_MPIX_Acc_type(rpcd->send_types[i]);
            arg_ffi_v[i] = rpcd->data + offset;
            offset += MPIX_Acc_type_extent(rpcd->send_types[i]);
        }

        ffi_type * ret_ffi = ffi_type_from_MPIX_Acc_type(rpcd->recv_type);

        ffi_status ret = ffi_prep_cif (&cif,
                                    FFI_DEFAULT_ABI,
                                    rpcd->send_count,
                                    ret_ffi,
                                    arg_ffi);

        if(ret != FFI_OK)
        {
            return -1;
        }
    }

    void * fn = NULL;

    {
        //TODO: cache fn
        fn = _resolve_fname(rpcd->function);
        //fn = dlsym(NULL, rpcd->function);

        if(!fn)
        {
            fprintf(stderr, "Could not resolve function %s\n", rpcd->function);
            abort();
        }
    }

    ffi_call (&cif, fn, result_buffer, arg_ffi_v);

    return 0;
}


void * accelerator_progress_loop(void * dummy)
{
    MPI_Request req;
    MPI_Status st;

    void * tmp_buffer = valloc( MPIX_ACC_MAX_PAYLOAD * sizeof(char));
    char result_buffer[MPIX_ACC_MAX_RESP];

    char * debug = getenv("MPIX_ACC_DEBUG");

    while(1)
    {

        int msg_size = 0;

        MPI_Recv(tmp_buffer, MPIX_ACC_MAX_PAYLOAD, MPI_BYTE, MPI_ANY_SOURCE, 0, accelerator_comm, &st);

        MPI_Get_count(&st, MPI_BYTE, &msg_size);

        if( MPIX_ACC_MAX_PAYLOAD <= msg_size)
        {
            fprintf(stderr, "Message payload is too large (%d / %d)\n", msg_size, MPIX_ACC_MAX_PAYLOAD);
            abort();
        }

        struct accelerator_rpc_desc * rpcd = (struct accelerator_rpc_desc *)tmp_buffer;

        if(debug)
            accelerator_rpc_desc_print(rpcd);

        if( rpcd->end_of_rpc )
        {
            break;
        }

        if( MPIX_ACC_MAX_RESP <= MPIX_Acc_type_extent(rpcd->recv_type))
        {
            fprintf(stderr, "Message response is too large (%ld / %d)\n", MPIX_Acc_type_extent(rpcd->recv_type), MPIX_ACC_MAX_RESP);
            abort();
        }

        accelerator_process_rpc(rpcd, &result_buffer);

        /* No need to answer if response is NULL */
        if(rpcd->recv_type != MPIX_ACC_NULL)
        {
            MPI_Send(result_buffer,
                     MPIX_Acc_type_extent(rpcd->recv_type),
                     MPI_BYTE, rpcd->source,
                     rpcd->response_tag,
                     accelerator_comm);
        }
    }

    free(tmp_buffer);
}

pthread_t acc_progress_thread;

int MPIX_Accelerator_init()
{
    MPI_Comm_dup(MPI_COMM_WORLD, &accelerator_comm);
    pthread_create(&acc_progress_thread, NULL, accelerator_progress_loop, NULL);
    MPI_Barrier(accelerator_comm);
}


int MPIX_Accelerator_finalize()
{
    MPI_Barrier(accelerator_comm);

    int myself;
    MPI_Comm_rank(accelerator_comm, &myself);

    struct accelerator_rpc_desc * eof = accelerator_rpc_desc_eof();
    MPI_Send(eof, sizeof(struct accelerator_rpc_desc), MPI_BYTE, myself, 0, accelerator_comm);

    pthread_join(acc_progress_thread, NULL);

    MPI_Barrier(accelerator_comm);

    MPI_Comm_free(&accelerator_comm);
}

int MPIX_Ioffload(void ** buffers,
                  MPI_Datatype * datatypes,
                  int sendcount,
                  void * recvbuf,
                  MPI_Datatype recvtype,
                  char * fname,
                  int dest,
                  MPI_Request * req)
{
    MPIX_Acc_type acc_send_types[MPI_ACC_MAX_SEND_COUNT];

    int i;

    for( i = 0 ; i < sendcount; i++)
    {
        acc_send_types[i] = MPIX_Acc_type_from_mpi(datatypes[i]);
    }

    MPIX_Acc_type acc_recv_type = MPIX_Acc_type_from_mpi(recvtype);


    size_t overall_size = 0;
    int to_free = 0;

    struct accelerator_rpc_desc * rpcd = accelerator_rpc_desc_init(fname,
                                                                   acc_send_types,
                                                                   buffers,
                                                                   sendcount,
                                                                   acc_recv_type,
                                                                   0,
                                                                   &overall_size,
                                                                   &to_free);

    MPI_Send(rpcd, overall_size, MPI_BYTE, dest, 0, accelerator_comm);

    /* We only expect a response if we have a return type */
    if(rpcd->recv_type != MPIX_ACC_NULL)
    {
        MPI_Irecv(recvbuf, MPIX_Acc_type_extent(acc_recv_type), MPI_BYTE, dest, rpcd->response_tag, accelerator_comm, req);
    }
    else
    {
        *req = MPI_REQUEST_NULL;
    }

    if(to_free)
        free(rpcd);

    return MPI_SUCCESS;
}


int MPIX_Offload(void ** buffers,
                 MPI_Datatype * datatypes,
                 int sendcount,
                 void * recvbuf,
                 MPI_Datatype recvtype,
                 char * fname,
                 int dest)
{
    MPI_Request req;

    MPIX_Ioffload(buffers,
                  datatypes,
                  sendcount,
                  recvbuf,
                  recvtype,
                  fname,
                  dest,
                  &req);
    return MPI_Wait(&req, MPI_STATUS_IGNORE);
}

