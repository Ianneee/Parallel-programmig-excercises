/*
Create a program that calculates an approximation of
integral using the trapezoidal rule.
The program must estimate correctly the integral even
if comm_sz doesn't divide n (the number of processes does
not divide the number of trapezoids).
*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <mpi.h>

double Trap(double left_endpt, double right_endpt, int trap_count, double base_len);
double func(double);

int main(void){
    int my_rank, comm_size, n = 1024, local_n;
    double a = 0.0, b = 3.0, h, local_a, local_b;
    double local_int, total_int;
    int source;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    h = (b-a)/n;
    local_n = n/comm_size;
    int rest = n%comm_size;

    if (rest != 0 && my_rank < rest){
        local_a = a + (my_rank * local_n * h) + (my_rank * h);
        local_n += 1;
        local_b = local_a + local_n*h;
    } else if (rest != 0 && my_rank >= rest){
        local_a = a + (rest * h) + (my_rank * local_n * h);
        local_b = local_a + (local_n * h);
    } else {
        local_a = a + (my_rank * local_n * h);
        local_b = local_a + local_n*h;
    }

    local_int = Trap(local_a, local_b, local_n, h);

    if (my_rank != 0){
        MPI_Send(&local_int, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    } else {
        total_int = local_int;
        for (source = 1; source < comm_size; source++){
            MPI_Recv(&local_int, 1, MPI_DOUBLE, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            total_int += local_int;
        }
    }

    if (my_rank == 0){
        printf("With n = %d trapezoids over %d processes, our estimate\n", n, comm_size);
        printf("of the integral from %f to %f = %.15e\n", a, b, total_int);
    }
    MPI_Finalize();
    return 0;
}

double Trap(double left_endpt, double right_endpt, int trap_count, double base_len){
    double estimate, x;
    int i;
    estimate = (func(left_endpt) + func(right_endpt))/2.0;
    for (i = 1; i <= trap_count-1; i++){
        x = left_endpt + i*base_len;
        estimate += func(x);
    }
    estimate = estimate*base_len;

    return estimate;
}

double func(double point){
    // 12x^3+9x^2+2
    double val = (12 * pow(point, 3)) + (9 * pow(point, 2)) + 2;
    //printf("Valore calcolato per il punto %f: %f\n", point, val);
    return val;
}
