/* 
 * Write an MPI program that computes a tree-structured global sum. 
 * Write your program for the special case in which comm_sz is a power of two. 
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define array_size 100

void create_data(int *array);
void send_data(int comm_sz, int *array);
void starting_sum(int *result, int *array, int n);

int  main(){
    int my_rank, comm_sz;
    int k = 1, j = 2, sum = 0;
    int *local_data;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    int n = array_size / comm_sz;
    int rest = array_size % comm_sz;

    if (my_rank == 0){
        int data[array_size];
        create_data(data);
        send_data(comm_sz, data);
        n = n+rest;
        local_data = calloc(n, sizeof(int));
        for (int i = 0; i < n; i++){
            local_data[i] = data[i];
        }
    } else {
        local_data = calloc(n, sizeof(int));
        MPI_Recv(local_data, n, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    }

    starting_sum(&sum, local_data, n);

    int other_sum = 0;
    while (j <= comm_sz){
        if (my_rank % j == 0 || my_rank == 0){
            MPI_Recv(&other_sum, 1, MPI_INT, my_rank+k, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            sum += other_sum;
        } else {
            MPI_Send(&sum, 1, MPI_INT, my_rank-k, 0, MPI_COMM_WORLD);
            break;
        }
        k *= 2;
        j *= 2;
    }

    if (my_rank == 0){
        printf("Total sum calcuated with %d processes: %d\n", comm_sz, sum);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}


void create_data(int *array){
    for (int i = 1; i <= array_size; i++){
        array[i-1] = i;
    }
}

void send_data(int comm_sz, int *array){
    int n = array_size / comm_sz;
    int rest = array_size % comm_sz;
    int data_array[n];

    for (int i = 1; i < comm_sz; i++){
        for (int k = 0; k < n; k++){
            data_array[k] = array[n * i + rest + k];
        }
        MPI_Send(data_array, n, MPI_INT, i, 0, MPI_COMM_WORLD);
    }
}

void starting_sum(int *result, int *array, int n){
    for (int i = 0; i < n; i++){
        *result += array[i];
    }
}
