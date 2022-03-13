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
        //ToDo: Add Comment
        vector[i] = rand() % 100;
    }
}

int main()
{

    unsigned long size = 100000000;

    srand(time(0));

    int *v1, *v1_root, *v2, *v2_root, *v3, *v3_root;
    int numtasks, rank;

    //ToDo: Add Comment
    auto start = high_resolution_clock::now();

    // Initialize the MPI environment
    MPI_Init(&argc, &argv);

    // Get the number of tasks/process
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);

    // Get the rank
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    //ToDo: Add Comment
    v1 = (int *)malloc(size / numtasks * sizeof(int *));
    v2 = (int *)malloc(size / numtasks * sizeof(int *));
    v3 = (int *)malloc(size / numtasks * sizeof(int *));

    if (rank == 0)
    {
        v1_root = (int *)malloc(size * sizeof(int *));
        v2_root = (int *)malloc(size * sizeof(int *));
        v3_root = (int *)malloc(size * sizeof(int *));

        randomVector(v1_root, size);

        randomVector(v2_root, size);
    }

    MPI_Scatter(v1_root, size / numtasks, MPI_INT, v1, size / numtasks, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Scatter(v2_root, size / numtasks, MPI_INT, v2, size / numtasks, MPI_INT, 0, MPI_COMM_WORLD);

    //ToDo: Add Comment
    for (int i = 0; i < size / numtasks; i++)
    {
        v3[i] = v1[i] + v2[i];
    }

    MPI_Gather(v3, size / numtasks, MPI_INT, v3_root, size / numtasks, MPI_INT, 0, MPI_COMM_WORLD);

    // Finalize the MPI environment
    MPI_Finalize();

    auto stop = high_resolution_clock::now();

    //ToDo: Add Comment
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}