#include <iostream>
#include <cstdlib>
#include <time.h>
#include <pthread.h>
#include <chrono>

// maximum size of matrix
#define MAX 100

// maximum number of threads
#define MAX_THREAD 5

using namespace std::chrono;
using namespace std;

int matA[MAX][MAX];
int matB[MAX][MAX];
int matC[MAX][MAX];
int step_i = 0;

void randomArr(int arr[][MAX], int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            arr[i][j] = rand() % 100;
        }
    }
}

void *multi(void *arg)
{
    int i = step_i++; //i denotes row number of resultant matC

    for (int j = 0; j < MAX; j++)
        for (int k = 0; k < MAX; k++)
            matC[i][j] += matA[i][k] * matB[k][j];
}

int main()
{

    srand(time(0));

    //ToDo: Returns a time point representing the starting point in time.
    auto start = high_resolution_clock::now();

    randomArr(matA, MAX);
    randomArr(matB, MAX);

    // declaring four threads
    pthread_t threads[MAX_THREAD];

    // Creating four threads, each evaluating its own part
    for (int i = 0; i < MAX_THREAD; i++)
    {
        int *p;
        pthread_create(&threads[i], NULL, multi, (void *)(p));
    }

    // joining and waiting for all threads to complete
    for (int i = 0; i < MAX_THREAD; i++)
        pthread_join(threads[i], NULL);

    for (int i = 0; i < MAX; i++)
    {
        for (int j = 0; i < MAX; i++)
        {
            // if (j != 5)
            // {
            printf("%d ", matC[i][j]);
            // }
        }
        if (i != 5)
        {
            printf("\n");
        }
    }

    //pthread_join(matMultThread, NULL);

    auto stop = high_resolution_clock::now();

    //ToDo: Convert the duration time between starting point and stopping point to ToDuration type
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}