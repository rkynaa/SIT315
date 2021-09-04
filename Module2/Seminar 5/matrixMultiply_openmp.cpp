#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <omp.h>
#define THREAD_NUM 20

using namespace std::chrono;
using namespace std;

void randomArr(int **arr, int size)
{
#pragma omp parallel for default(none) shared(size, arr)
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            arr[i][j] = rand() % 100;
        }
    }
    //arr[0] = 1;
}

int main()
{
    // size
    unsigned long size = 1000;

    int total = 0;

    srand(time(0));

    //ToDo: Returns a time point representing the starting point in time.
    auto start = high_resolution_clock::now();

    omp_set_num_threads(5);

    //ToDo: Initiating arrays
    int **x = new int *[size];
    int **y = new int *[size];
    int **z = new int *[size];
    for (int i = 0; i < size; i++)
    {
        x[i] = new int[size];
        y[i] = new int[size];
        z[i] = new int[size];
    }

    randomArr(x, size);
    randomArr(y, size);

    //ToDo: calculating the matrix multiplication
#pragma omp parallel for reduction(* \
                                   : total)
    for (int i = 0; i < size; i++)
    {
        for (int j = 0; j < size; j++)
        {
            for (int k = 0; k < size; k++)
            {
                total = x[i][k] * y[k][j];
                z[i][j] += total;
            }
        }
    }

    auto stop = high_resolution_clock::now();

    //ToDo: Convert the duration time between starting point and stopping point to ToDuration type
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    for (int i = 0; i < size; i++)
    {
        delete x[i];
        delete y[i];
        delete z[i];
    }
    delete[] x;
    delete[] y;
    delete[] z;

    return 0;
}