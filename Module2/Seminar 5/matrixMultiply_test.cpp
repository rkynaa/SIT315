#include <iostream>
#include <cstdlib>
#include <time.h>
#include <pthread.h>
#include <chrono>
#define NUM_THREADS 10

#define MAX_NUMBERS 100

using namespace std::chrono;
using namespace std;

int matrix1[MAX_NUMBERS][MAX_NUMBERS];
int matrix2[MAX_NUMBERS][MAX_NUMBERS];
int matrix3[MAX_NUMBERS][MAX_NUMBERS];
int step_mult = 0;

void randomArr(int arr[][MAX_NUMBERS], int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            arr[i][j] = rand() % 100;
        }
    }
}

void matMultiply(void *arg)
{
    int i = step_mult++;
    for (int j = 0; j < MAX_NUMBERS; j++)
    {
        for (int k = 0; k < MAX_NUMBERS; k++)
        {
            matrix3[i][j] += matrix1[i][k] * matrix2[k][j];
        }
    }
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

    //ToDo: Returns a time point representing the starting point in time.
    auto start = high_resolution_clock::now();

    randomArr(matrix1, MAX_NUMBERS);
    // randomArr(matrix2, MAX_NUMBERS);

    // pthread_t threads[NUM_THREADS];

    // for (int i = 0; i < NUM_THREADS; i++)
    // {
    //     int *point;
    //     pthread_create(&threads[i], NULL, matMultiply, (void *)(point));
    // }

    // for (int i = 0; i < NUM_THREADS; i++)
    // {
    //     pthread_join(threads[i], NULL);
    // }

    //pthread_join(matMultThread, NULL);

    auto stop = high_resolution_clock::now();

    //ToDo: Convert the duration time between starting point and stopping point to ToDuration type
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}