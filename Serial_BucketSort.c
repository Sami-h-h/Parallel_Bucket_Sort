//3-19-2024
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 100000
#define M 10

void bs_serial(int arr[], int n);
void insert_into_bucket(int arr[], int start, int end, int b[][N], int counts[], int range);
void sort_buckets(int b[][N], int counts[], int start, int end);

int main() {
    int arr[N];
    clock_t start, end;
    double total_cpu_time_used = 0.0;
    for (int iter = 0; iter < 10; iter++) {
        srand(42);
        for (int i = 0; i < N; i++) {
            arr[i] = rand() % 100; 
        }

        start = clock();
        bs_serial(arr, N);
        end = clock();

        double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_cpu_time_used += cpu_time_used;

        printf("It %d= Time taken: %f sec\n", iter+1, cpu_time_used);
    }

    printf("Avg= %f sec\n", total_cpu_time_used / 10);

    return 0;
}

void bs_serial(int arr[], int n) {
    int b[M][N] = {0};
    int counts[M] = {0};
    int range = 100 / M;

    insert_into_bucket(arr, 0, n - 1, b, counts, range);

    sort_buckets(b, counts, 0, M - 1);

    int index = 0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < counts[i]; j++) {
            arr[index++] = b[i][j];
        }
    }
}

void insert_into_bucket(int arr[], int start, int end, int b[][N], int counts[], int range) {
    for (int i = start; i <= end; i++) {
        int bucket_index = arr[i] / range;
        b[bucket_index][counts[bucket_index]++] = arr[i];
    }
}

void sort_buckets(int b[][N], int counts[], int start, int end) {
    for (int i = start; i <= end; i++) {
        if (counts[i] > 0) {
            for (int j = 1; j < counts[i]; j++) {
                int key = b[i][j];
                int k = j - 1;
                while (k >= 0 && b[i][k] > key) {
                    b[i][k + 1] = b[i][k];
                    k = k - 1;
                }
                b[i][k + 1] = key;
            }
        }
    }
}
