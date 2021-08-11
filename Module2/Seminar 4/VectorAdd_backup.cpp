#include <iostream>
#include <cstdlib>
#include <time.h>
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

//ToDo: Allocating memories for each vectors
int *allocate_memory(unsigned long size)
{
    int *vector;
    vector = (int *)malloc(size * sizeof(int *));
    return vector;
}

//void addElement(int v1[], int v2[], int v3[], unsigned long size)
//{
//    for (int i = 0; i < size; i++)
//    {
//        v3[i] = v1[i] + v2[i];
//    }
//}

int main()
{

    unsigned long size = 100000000;

    srand(time(0));

    // ToDo: Calcluate
    //calculate_time(size);
    //ToDo: Returns a time point representing the starting point in time.
    auto start = high_resolution_clock::now();

    int *v1, *v2, *v3, *v4;

    //v1 = (int *)malloc(size * sizeof(int *));
    //v2 = (int *)malloc(size * sizeof(int *));
    //v3 = (int *)malloc(size * sizeof(int *));

    cout << "Starting initialization. . . . \n";
    v1 = allocate_memory(size);
    cout << "Vector 1 initialization complete\n";
    v2 = allocate_memory(size);
    cout << "Vector 2 initialization complete\n";
    v3 = allocate_memory(size);
    cout << "Vector 3 initialization complete\n";
    //v4 = allocate_memory(size);
    //cout << "Vector 4 initialization complete\n";

    randomVector(v1, size);

    randomVector(v2, size);

    //ToDo: Addding element to the third vector based on first and second vector elements addition
    //addElement(v1, v2, v3, size);
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