#include <iostream>
#include <cstdlib>
#include <time.h>
#include <chrono>
#include <mpi.h>

using namespace std::chrono;
using namespace std;

void randomVector(int vector[], int size)
{
    for (int i = 0; i < size; i++)
    {
        vector[i] = rand() % 100;
    }
}

int main(int argc, char **argv)
{

    unsigned long size = 100000000;

    srand(time(0));

    int *v1, *v1_root, *v2, *v2_root, *v3, *v3_root;
    int numtasks, rank;

    // int subSum = 0;
    // int globalSum;

    // Divide data length for each processor
    int len = size / numtasks;

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of tasks/process
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    // Get the rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    // Allocate memory in the heap for sub vector
    // Avoid overwriting memory in heap
    v1 = (int *)malloc(size / numtasks * sizeof(int *));
    v2 = (int *)malloc(size / numtasks * sizeof(int *));
    v3 = (int *)malloc(size / numtasks * sizeof(int *));

    if (rank == 0)
    {
        // Allocate memory in the heap for the main vector
        // The process is done with the master node
        v1_root = (int *)malloc(size * sizeof(int *));
        v2_root = (int *)malloc(size * sizeof(int *));
        v3_root = (int *)malloc(size * sizeof(int *));

        // Generate random integer
        randomVector(v1_root, size);
        randomVector(v2_root, size);
    }

    auto start = high_resolution_clock::now();

    // Evenly scatter data from root to processor
    MPI_Scatter(v1_root, len, MPI_INT, v1, len, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(v2_root, len, MPI_INT, v2, len, MPI_INT, 0, MPI_COMM_WORLD);

    // Calculate v3 in each processor
    for (int i = 0; i < len; i++)
    {
        v3[i] = v1[i] + v2[i];
        // subSum += v3[i];
    }

    // Report v3 back to master.
    MPI_Gather(v3, len, MPI_INT, v3_root, len, MPI_INT, 0, MPI_COMM_WORLD);

    // MPI_Reduce(&subSum, &globalSum, 1, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);

    if (rank == 0)
    {
        auto stop = high_resolution_clock::now();

        auto duration = duration_cast<microseconds>(stop - start);
        cout << "Time taken by function: "
             << duration.count() << " microseconds" << endl;
        // cout << "Total sum is: "
        //      << globalSum << endl;
    }

    // Finalize the MPI environment
    MPI_Finalize();

    return 0;
}