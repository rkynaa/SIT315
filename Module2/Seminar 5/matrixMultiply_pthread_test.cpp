#include <iostream>
#include <cstdlib>
#include <time.h>
#include <pthread.h>
#include <chrono>
#define NUM_THREADS 10

using namespace std::chrono;
using namespace std;

struct struct_matrix
{
    int **matrix;
    unsigned long size;
};

struct struct_matrices
{
    struct_matrix mat_x;
    struct_matrix mat_y;
    struct_matrix mat_z;
};

void randomArr(int **arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            arr[i][j] = rand() % 100;
        }
    }
    //arr[0] = 1;
}

int **allocate_memory(unsigned long size)
{
    int **matrix = new int *[size];
    for (int i = 0; i < size; i++)
    {
        matrix[i] = new int[size];
    }
    return matrix;
}

void matMultiply(int **x, int **y, int **z, unsigned long size)
{
    int thread_id = pthread_self();
    unsigned long split_size = size / NUM_THREADS;
    for (int w == thread_id * split_size; w < (thread_id + 1) * split_size; w++)
    {

        for (int i = 0; i < size; i++)
        {
            for (int j = 0; j < size; j++)
            {
                for (int k = 0; k < size; k++)
                {
                    z[i][j] += x[i][k] * y[k][j];
                }
            }
        }
    }
}

void *allocate_memory_pthread(void *arg)
{
    // detach the current thread
    // from the calling thread
    pthread_detach(pthread_self());

    struct struct_matrix *matr = (struct struct_matrix *)arg;
    matr->matrix = allocate_memory(matr->size);

    // exit the current thread
    pthread_exit(NULL);
}

void *randomArr_pthread(void *arg)
{
    // detach the current thread
    // from the calling thread
    pthread_detach(pthread_self());

    struct struct_matrix *matr = (struct struct_matrix *)arg;
    randomArr(matr->matrix, matr->size);

    // exit the current thread
    pthread_exit(NULL);
}

void *matMultiply_pthread(void *arg)
{
    // detach the current thread
    // from the calling thread
    //pthread_detach(pthread_self());
    printf("%d", pthread_self());

    struct struct_matrices *matrices = (struct struct_matrices *)arg;
    matMultiply(matrices->mat_x.matrix, matrices->mat_y.matrix, matrices->mat_z.matrix, matrices->mat_x.size);

    // exit the current thread
    pthread_exit(NULL);
}

int main()
{
    // size = 10
    // unsigned long size = 10;

    // size = 100
    // unsigned long size = 100;

    // size = 1000
    unsigned long size = 100;

    srand(time(0));

    struct struct_matrices main_mat;

    struct struct_matrix struct_x;
    struct struct_matrix struct_y;
    struct struct_matrix struct_z;

    struct_x.size = size;
    struct_y.size = size;
    struct_z.size = size;

    //ToDo: Returns a time point representing the starting point in time.
    auto start = high_resolution_clock::now();

    pthread_t threads[NUM_THREADS];

    pthread_t allocate_x,
        allocate_y, allocate_z;

    pthread_create(&allocate_x, NULL, &allocate_memory_pthread, (void *)&struct_x);
    pthread_create(&allocate_y, NULL, &allocate_memory_pthread, (void *)&struct_y);
    pthread_create(&allocate_z, NULL, &allocate_memory_pthread, (void *)&struct_z);

    // for (int i = 0; i < NUM_THREADS; i++)
    // {
    //     pthread_join(threads[i], NULL);
    // }

    pthread_join(allocate_x, NULL);
    pthread_join(allocate_y, NULL);
    pthread_join(allocate_z, NULL);

    pthread_t randMatX, randMatY;

    pthread_create(&randMatX, NULL, &randomArr_pthread, (void *)&struct_x);
    pthread_create(&randMatY, NULL, &randomArr_pthread, (void *)&struct_y);

    pthread_join(randMatX, NULL);
    pthread_join(randMatY, NULL);

    main_mat.mat_x = struct_x;
    main_mat.mat_y = struct_y;
    main_mat.mat_z = struct_z;

    pthread_t matMultThread;

    //ToDo: calculating the matrix multiplication
    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_create(&threads[i], NULL, &matMultiply_pthread, (void *)&main_mat);
    }

    for (int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }

    // pthread_create(&matMultThread, NULL, &matMultiply_pthread, (void *)&main_mat);
    // pthread_join(matMultThread, NULL);

    //pthread_join(matMultThread, NULL);

    auto stop = high_resolution_clock::now();

    //ToDo: Convert the duration time between starting point and stopping point to ToDuration type
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}