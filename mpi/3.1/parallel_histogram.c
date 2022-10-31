/*
 * Parallel program to generate histogram from floating point data
 * taken from input.
 */

#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define bin_count 5
#define max_data 20

void take_inputs(double *data_array);
void calc_min_max(double *array, int len, double *min, double *max);
int find_bin(double data, double *bin_maxes, int data_count);

int main(){
    double bin_maxes[bin_count], *local_data;
    int local_bin_counts[bin_count] = {0}, data_count;
    int my_rank, comm_sz;
    double min_meas, max_meas;
    int local_n, rest;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    data_count = max_data;
    local_n = data_count / comm_sz;
    rest = data_count % comm_sz;

    if (my_rank == 0){
        double data[] = { 1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9 }; //tot=20
        /*
        double data[20];
        take_inputs(data);
        */

        calc_min_max(data, data_count, &min_meas, &max_meas);
        double min_max_meas[2] = { min_meas, max_meas };
        double data_shared[local_n];

        for (int i = 1; i < comm_sz; i++){
            for (int k = 0; k < local_n; k++){
                data_shared[k] = data[rest + (i * local_n) + k];
            }
            MPI_Send(data_shared, local_n, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
            MPI_Send(min_max_meas, 2, MPI_DOUBLE, i, 0, MPI_COMM_WORLD);
        }
        local_n = local_n + rest;
        local_data = calloc(local_n, sizeof(double));
        for (int i = 0; i < local_n; i++){
            local_data[i] = data[i];
        }
    } else {
        local_data = calloc(local_n, sizeof(double));
        double min_max_meas[2];

        MPI_Recv(local_data, local_n, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        MPI_Recv(min_max_meas, 2, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        min_meas = min_max_meas[0];
        max_meas = min_max_meas[1];
    }


    double bin_width = (max_meas - min_meas)/bin_count;
    for (int b = 0; b < bin_count; b++){
        bin_maxes[b] = min_meas + bin_width*(b+1);
    }

    for (int i = 0; i < local_n; i++){
        int bin = find_bin(local_data[i], bin_maxes, max_data);
        if (bin == -1){
            return -1;
        }
        local_bin_counts[bin]++;
    }

    if (my_rank == 0){
        int bin_buff[bin_count];
        for (int i = 1; i < comm_sz; i++){
            MPI_Recv(bin_buff, bin_count, MPI_DOUBLE, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            for (int k = 0; k < bin_count; k++){
                local_bin_counts[k] += bin_buff[k];
            }
        }
        for (int i = 0; i < bin_count; i++){
            int counts = local_bin_counts[i];
            printf("[%d|", i);
            for (int k = 0; k < counts; k++){
                printf("*");
            }
            printf("\n");
        }
    } else {
        MPI_Send(local_bin_counts, bin_count, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
    }

    free(local_data);
    MPI_Finalize();
    return 0;
}

void take_inputs(double *data_array){
    int count = 0;
    printf("Insert %d floating numbers\n", max_data);
    while (count < max_data) {
        scanf("%lf", &data_array[count]);
        count++;
    }
}

void calc_min_max(double *array, int len, double *min, double *max){
    *min = array[0];
    *max = array[0];
    for (int i = 1; i < len; i++){
        if (array[i] < *min){
            *min = array[i];
        } else if (array[i] > *max){
            *max = array[i];
        }
    }
}

int find_bin(double data, double *bin_maxes, int data_count){
    /* Can implement this function with binary search for if use a
       big number of buckets. */
    for (int i = 0; i < data_count; i++){
        if (data <= bin_maxes[i]){
            return i;
        }
    }
    return -1;
}
