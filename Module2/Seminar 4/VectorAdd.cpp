#include <iostream>
#include <cstdlib>
#include <time.h>
#include <pthread.h>
#include <chrono>

using namespace std::chrono;
using namespace std;

void randomVector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
    {
        //ToDo: Adding random elements to the vector between 1 and 99
        vector[i] = rand() % 100;
    }
}

int main()
{

    unsigned long size = 100000;

    srand(time(0));

    int *v1, *v2, *v3;

    //ToDo: Returns a time point representing the starting point in time.
    auto start = high_resolution_clock::now();

    //ToDo: Allocating memories for each vectors
    v1 = (int *)malloc(size * sizeof(int *));
    v2 = (int *)malloc(size * sizeof(int *));
    v3 = (int *)malloc(size * sizeof(int *));

    randomVector(v1, size);

    randomVector(v2, size);

    //ToDo: Addding element to the third vector based on first and second vector elements addition
    for (int i = 0; i < size; i++)
    {
        v3[i] = v1[i] + v2[i];
    }

    auto stop = high_resolution_clock::now();

    //ToDo: Convert the duration time between starting point and stopping point to ToDuration type
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}