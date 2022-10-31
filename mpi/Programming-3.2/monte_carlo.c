#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define num_tosses 1000000

int toss();

/*
 * Generate a random double into range [-1, 1].
 */
double rand_double();

int main(){
    int local_numbers_in_circle;
    double pi_estimate;
    int my_rank, comm_sz;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    local_numbers_in_circle = toss();

    if (my_rank == 0){
        int in_circle_buff;
        int total_tosses = num_tosses;
        for (int i = 1; i < comm_sz; i++){
            MPI_Recv(&in_circle_buff, 1, MPI_INT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            local_numbers_in_circle += in_circle_buff;
            total_tosses += num_tosses;
        }
        pi_estimate = 4 * local_numbers_in_circle / (double)total_tosses;
        printf("Estimation of pi with %d process: %lf\n", comm_sz, pi_estimate);
    } else {
        MPI_Send(&local_numbers_in_circle, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    MPI_Finalize();
    return 0;
}

int toss(){
    double x, y, distance_squared;
    int numbers_in_circle = 0;
    for (int toss = 0; toss < num_tosses; toss++){
        x = rand_double();
        y = rand_double();
        distance_squared = x*x + y*y;
        if (distance_squared <= 1){
            numbers_in_circle++;
        }
    }
    return numbers_in_circle;
}

double rand_double(){
    double n = (double)rand() / (double)RAND_MAX;
    return (n * 2) -1;
}
