#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

#define N 100000
#define M 10
#define K 16

void bs_parallel(int arr[], int n);
void insert_in_bucket(int arr[], int start, int end, int b[][N], int counts[], int range);
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
        bs_parallel(arr, N);
        end = clock();

        double cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_cpu_time_used += cpu_time_used;

        printf("It %d= Time = %f sec\n", iter+1, cpu_time_used);
    }

    printf("Avg= %f sec\n", total_cpu_time_used / 10);

    return 0;
}

void bs_parallel(int arr[], int n) {
    int b[M][N] = {0}; 
    int counts[M] = {0}; 
    int range = 100 / M; 

    #pragma omp parallel num_threads(K)
    {
        int tid = omp_get_thread_num();
        int items_per_thread = n / K;
        int start = tid * items_per_thread;
        int end = start + items_per_thread - 1;
        if (tid == K - 1) end = n - 1; 
        insert_in_bucket(arr, start, end, b, counts, range);
    }

    #pragma omp parallel num_threads(K)
    {
        int tid = omp_get_thread_num();
        int buckets_per_thread = M / K;
        int start = tid * buckets_per_thread;
        int end = start + buckets_per_thread - 1;
        if (tid == K - 1) end = M - 1; 
        sort_buckets(b, counts, start, end);
    }

    int index = 0;
    for (int i = 0; i < M; i++) {
        for (int j = 0; j < counts[i]; j++) {
            arr[index++] = b[i][j];
        }
    }
}

void insert_in_bucket(int arr[], int start, int end, int b[][N], int counts[], int range) {
    for (int i = start; i <= end; i++) {
        int bucket_index = arr[i] / range;
        #pragma omp critical
        {
            b[bucket_index][counts[bucket_index]++] = arr[i];
        }
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
