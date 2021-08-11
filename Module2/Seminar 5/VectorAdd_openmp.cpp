#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <omp.h>
#define THREAD_NUM 3

using namespace std::chrono;
using namespace std;

void randomVector(int vector[], int size)
{
#pragma omp parallel for default(none) shared(size, vector)
    for (int i = 0; i < size; i++)
    {
        //ToDo: Adding random elements to the vector between 1 and 99
        vector[i] = rand() % 100;
    }
}

int main()
{

    unsigned long size = 100000000;

    srand(time(0));

    int *v1, *v2, *v3;

    //ToDo: Add Comment
    auto start = high_resolution_clock::now();

    omp_set_num_threads(5);

    //ToDo: Add Comment

    v1 = (int *)malloc(size * sizeof(int *));
    v2 = (int *)malloc(size * sizeof(int *));
    v3 = (int *)malloc(size * sizeof(int *));

    randomVector(v1, size);

    randomVector(v2, size);

// For static scheduling, chunk 4
// #pragma omp parallel for default(none) shared(size, v3, v1, v2) schedule(static, 4)

// For static scheduling, chunk 8
// #pragma omp parallel for default(none) shared(size, v3, v1, v2) schedule(static, 8)

// For guided scheduling, chunk 4
// #pragma omp parallel for default(none) shared(size, v3, v1, v2) schedule(guided, 4)

// For guided scheduling, chunk 8
// #pragma omp parallel for default(none) shared(size, v3, v1, v2) schedule(guided, 8)

// For dymanic scheduling, chunk 4
// #pragma omp parallel for default(none) shared(size, v3, v1, v2) schedule(dynamic, 4)

// For dymanic scheduling, chunk 8
// #pragma omp parallel for default(none) shared(size, v3, v1, v2) schedule(dynamic, 8)

// For dymanic scheduling, chunk 12
#pragma omp parallel for default(none) shared(size, v3, v1, v2) schedule(dynamic, 12)
    //ToDo: Add Comment
    for (int i = 0; i < size; i++)
    {
        v3[i] = v1[i] + v2[i];
    }

    int test = 0;
#pragma omp parallel private(test)
    {
#pragma omp critical
        test++;
    }
    printf("Using critical: %d\n", &test);

    int total = 0;
#pragma omp parallel for reduction(+ \
                                   : total)
    for (int i = 0; i < size; i++)
    {
#pragma omp atomic update
        total += v3[i];
    }
    printf("Using atomic:   %d\n", &total);

    auto stop = high_resolution_clock::now();

    //ToDo: Add Comment
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}