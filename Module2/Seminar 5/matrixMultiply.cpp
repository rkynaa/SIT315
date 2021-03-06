#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>

using namespace std::chrono;
using namespace std;

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

int main()
{
    // size = 10
    // unsigned long size = 10;

    // size = 100
    // unsigned long size = 100;

    // size = 1000
    unsigned long size = 2000;

    srand(time(0));

    int *v1, *v2, *v3;

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

    //ToDo: Returns a time point representing the starting point in time.
    auto start = high_resolution_clock::now();

    randomArr(x, size);
    randomArr(y, size);

    //ToDo: calculating the matrix multiplication
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