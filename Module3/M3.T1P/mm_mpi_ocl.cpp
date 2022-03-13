#include <iostream>
#include <time.h>
#include <cstdlib>
#include <chrono>
#include <mpi.h>
#include <unistd.h>

using namespace std::chrono;
using namespace std;

#define FROM_MASTER 1 /* setting a message type */
#define FROM_WORKER 2 /* setting a message type */

int main(int argc, char **argv)
{
    int numtasks, /* number of tasks in partition */
        matrixSize,
        srand(time(0)),
        processID,             /* a task identifier */
        numworkers,            /* number of worker tasks */
        source,                /* task id of message source */
        dest,                  /* task id of message destination */
        mtype,                 /* message type */
        rows,                  /* rows of matrix A sent to each worker */
        avgRow, reRow, offset, /* used to determine rows sent to each worker */
        i, j, k, rc;           /* misc */

    matrixSize = atoi(argv[1]);

    double matrixA[matrixSize][matrixSize], /* matrix A to be multiplied */
        matrixB[matrixSize][matrixSize],    /* matrix B to be multiplied */
        matrixC[matrixSize][matrixSize];    /* result matrix C */

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &processID);
    MPI_Comm_size(MPI_COMM_WORLD, &numtasks);
    MPI_Status status;
    if (numtasks < 2)
    {
        printf("Need at least two MPI tasks. Quitting...\n");
        MPI_Abort(MPI_COMM_WORLD, rc);
        exit(1);
    }
    numworkers = numtasks - 1;

    /**************************** master task ************************************/
    if (processID == 0)
    {
        // read matrixSize
        //cout << "Enter a number N (N rows x N cols): ";
        //cin >> matrixSize;

        printf("The MPI has started with %d tasks.\n", numtasks);
        printf("Initializing arrays...\n");
        // loop all elements in the matrix.
        for (int i = 0; i < matrixSize; ++i)
        {
            for (int j = 0; j < matrixSize; ++j)
            {
                // Generate random number from 0-9 and store it in matrix
                matrixA[i][j] = rand() % 10;
                matrixB[i][j] = rand() % 10;
            }
        }
        // start time
        auto start = high_resolution_clock::now();

        /* Send matrix data to the worker tasks */
        avgRow = matrixSize / numworkers;
        reRow = matrixSize % numworkers;
        offset = 0;
        mtype = FROM_MASTER;
        for (dest = 1; dest <= numworkers; dest++)
        {
            // Check to see if the remaining row is 0. If yes, row = avgRow, else row= avgRow + 1
            rows = (dest <= reRow) ? avgRow + 1 : avgRow;
            printf("Sending %d rows to task %d offset=%d\n", rows, dest, offset);
            MPI_Send(&offset, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            // Send rows to the workers
            MPI_Send(&rows, 1, MPI_INT, dest, mtype, MPI_COMM_WORLD);
            // Send matrix A. Its value depended on offset
            MPI_Send(&matrixA[offset][0], rows * matrixSize, MPI_DOUBLE, dest, mtype,
                     MPI_COMM_WORLD);
            MPI_Send(&matrixB, matrixSize * matrixSize, MPI_DOUBLE, dest, mtype, MPI_COMM_WORLD);
            offset = offset + rows;
        }

        /* Receive results from worker tasks */
        mtype = FROM_WORKER;
        for (i = 1; i <= numworkers; i++)
        {
            source = i;
            MPI_Recv(&offset, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&rows, 1, MPI_INT, source, mtype, MPI_COMM_WORLD, &status);
            MPI_Recv(&matrixC[offset][0], rows * matrixSize, MPI_DOUBLE, source, mtype,
                     MPI_COMM_WORLD, &status);
            printf("Received results from task %d\n", source);
        }

        /* Print results */

        //printf("******************************************************\n");
        //printf("Result Matrix:\n");
        //for (i=0; i<matrixSize; i++)
        //{
        //   printf("\n");
        //   for (j=0; j<matrixSize; j++)
        //      printf("%6.2f   ", matrixC[i][j]);
        //}
        //printf("\n******************************************************\n");

        // stop time
        auto stop = high_resolution_clock::now();

        // execution time
        auto duration = duration_cast<microseconds>(stop - start);
        cout << "\nTime taken by function: "
             << duration.count()
             << " microseconds"
             << endl;
    }

    /**************************** worker task ************************************/
    if (processID > 0)
    {
        mtype = FROM_MASTER;
        MPI_Recv(&offset, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&rows, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&matrixA, rows * matrixSize, MPI_DOUBLE, 0, mtype, MPI_COMM_WORLD, &status);
        MPI_Recv(&matrixB, matrixSize * matrixSize, MPI_DOUBLE, 0, mtype, MPI_COMM_WORLD, &status);

        //   for (k = 0; k < matrixSize; k++)
        //      for (i = 0; i < matrixSize; i++)
        //      {
        //         matrixC[i][k] = 0.0;
        //         for (j = 0; j < matrixSize; j++)
        //            matrixC[i][k] = matrixC[i][k] + matrixA[i][j] * matrixB[j][k];
        //      }
        mtype = FROM_WORKER;
        MPI_Send(&offset, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD);
        MPI_Send(&rows, 1, MPI_INT, 0, mtype, MPI_COMM_WORLD);
        MPI_Send(&matrixC, rows * matrixSize, MPI_DOUBLE, 0, mtype, MPI_COMM_WORLD);
    }
    MPI_Finalize();
}