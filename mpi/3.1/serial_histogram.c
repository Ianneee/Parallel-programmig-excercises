/*
 * Serial program to generate histogram from floating point data
 * stored into an array.
 */

#include <stdio.h>

#define bin_count 5

void calc_min_max(double *array, int len, double *min, double *max);
int find_bin(double data, double *bin_maxes);

int main(){
    double data[] = { 1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9 };
    int data_count = sizeof(data)/sizeof(data[0]);
    double bin_maxes[bin_count];
    int bin_counts[bin_count] = {0};

    double min_meas, max_meas;
    calc_min_max(data, data_count, &min_meas, &max_meas);

    double bin_width = (max_meas - min_meas)/bin_count;
    for (int b = 0; b < bin_count; b++){
        bin_maxes[b] = min_meas + bin_width*(b+1);
    }

    for (int i = 0; i < data_count; i++){
        int bin = find_bin(data[i], bin_maxes);
        if (bin == -1){
            return -1;
        }
        bin_counts[bin]++;
    }

    for (int i = 0; i < bin_count; i++){
        int counts = bin_counts[i];
        printf("[%d|", i);
        for (int k = 0; k < counts; k++){
            printf("*");
        }
        printf("\n");
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

int find_bin(double data, double *bin_maxes){
    /* Can implement this function with binary search for if use a
       big number of buckets. */
    for (int i = 0; i < bin_count; i++){
        if (data <= bin_maxes[i]){
            return i;
        }
    }
    return -1;
}
