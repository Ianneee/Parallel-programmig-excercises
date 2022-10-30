/*
 * Print a line from every process in rank order:
 * process 0 output first, then process 1 and so on.
 */

#include <stdio.h>
#include <mpi.h>

int main(){
    int my_rank, comm_sz, mesg;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0){
        printf("Process rank %d\n", my_rank);
        MPI_Send(&my_rank, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
    } else if (my_rank < comm_sz-1){
        MPI_Recv(&mesg, 1, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process rank %d\n", my_rank);
        MPI_Send(&my_rank, 1, MPI_INT, my_rank+1, 0, MPI_COMM_WORLD);
    } else {
        MPI_Recv(&mesg, 1, MPI_INT, my_rank-1, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        printf("Process rank %d\n", my_rank);
    }

    MPI_Finalize();
    return 0;
}
