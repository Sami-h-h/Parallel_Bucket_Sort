#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

#define N 100000
#define M 10
#define K 16
#define RUNS 10 

int arr[N];
int b[M][N] = {0};
int counts[M] = {0};
int range = 100 / M;
pthread_mutex_t lock[M];

typedef struct {
    int start;
    int end;
    int id;
} thread_data;

void* insert(void* arg);
void* sort(void* arg);

int main() {
    double total_time = 0.0; 

    for (int run = 0; run < RUNS; run++) { 
        srand(42 + run); 
        for (int i = 0; i < N; i++) {
            arr[i] = rand() % 100;
        }

        
        for (int i = 0; i < M; i++) {
            counts[i] = 0;
        }

        pthread_t threads[K];
        thread_data t_data[K];
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        clock_t start, end;

        start = clock();

        
        for (int i = 0; i < M; i++) {
            pthread_mutex_init(&lock[i], NULL);
        }

        
        for (int i = 0; i < K; i++) {
            t_data[i].start = i * (N / K);
            t_data[i].end = (i + 1) * (N / K) - 1;
            if (i == K - 1) t_data[i].end = N - 1; 
            t_data[i].id = i;

            pthread_create(&threads[i], &attr, insert, (void*)&t_data[i]);
        }

        
        for (int i = 0; i < K; i++) {
            pthread_join(threads[i], NULL);
        }

        
        for (int i = 0; i < K; i++) {
            t_data[i].start = i * (M / K);
            t_data[i].end = (i + 1) * (M / K) - 1;
            if (i == K - 1) t_data[i].end = M - 1; 

            pthread_create(&threads[i], &attr, sort, (void*)&t_data[i]);
        }

        
        for (int i = 0; i < K; i++) {
            pthread_join(threads[i], NULL);
        }

        
        int index = 0;
        for (int i = 0; i < M; i++) {
            for (int j = 0; j < counts[i]; j++) {
                arr[index++] = b[i][j];
            }
        }

        end = clock();

        double time = ((double) (end - start)) / CLOCKS_PER_SEC;
        total_time += time;

        
        for (int i = 0; i < M; i++) {
            pthread_mutex_destroy(&lock[i]);
        }
    }
 
    double avg_time = total_time / RUNS;
    printf("Average time taken: %f seconds\n", avg_time);

    return 0;
}

void* insert(void* arg) {
    thread_data* t_data = (thread_data*)arg;
    int start = t_data->start;
    int end = t_data->end;

    for (int i = start; i <= end; i++) {
        int index = arr[i] / range;
        pthread_mutex_lock(&lock[index]);
        b[index][counts[index]++] = arr[i];
        pthread_mutex_unlock(&lock[index]);
    }

    pthread_exit(NULL);
}

void* sort(void* arg) {
    thread_data* t_data = (thread_data*)arg;
    int start = t_data->start;
    int end = t_data->end;

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

    pthread_exit(NULL);
}
