#include <iostream>
#include <time.h>
#include <cstdlib>
#include <chrono>
#include <mpi.h>
#include <CL/cl.h>
#include <unistd.h>

using namespace std::chrono;
using namespace std;

#define FROM_MASTER 1 /* setting a message type */
#define FROM_WORKER 2 /* setting a message type */

cl_mem bufV;

cl_device_id device_id;
cl_context context;
cl_program program;
cl_kernel kernel;
cl_command_queue queue;
cl_event event = NULL;

int err;

cl_device_id create_device();
void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname);
cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename);
void setup_kernel_memory();
void copy_kernel_args();
void free_memory();

void init(int *&A, int size);
void print(int *A, int size);

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

void setup_kernel_memory()
{
    //ToDo: Add comment (what is the purpose of clCreateBuffer function? What are its arguments?)
    //The second parameter of the clCreateBuffer is cl_mem_flags flags. Check the OpenCL documention to find out what is it's purpose and read the List of supported memory flag values
    bufV = clCreateBuffer(context, CL_MEM_READ_WRITE, SZ * sizeof(int), NULL, NULL);

    // Copy matrices to the GPU
    clEnqueueWriteBuffer(queue, bufV, CL_TRUE, 0, SZ * sizeof(int), &v[0], 0, NULL, NULL);
}

void free_memory()
{
    clReleaseMemObject(bufV);

    clReleaseKernel(kernel);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);

    free(v);
}

void copy_kernel_args()
{
    clSetKernelArg(kernel, 0, sizeof(int), (void *)&SZ);
    clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&bufV);

    if (err < 0)
    {
        perror("Couldn't create a kernel argument");
        printf("error = %d", err);
        exit(1);
    }
}

void setup_openCL_device_context_queue_kernel(char *filename, char *kernelname)
{
    device_id = create_device();
    cl_int err;

    context = clCreateContext(NULL, 1, &device_id, NULL, NULL, &err);
    if (err < 0)
    {
        perror("Couldn't create a context");
        exit(1);
    }

    program = build_program(context, device_id, filename);

    queue = clCreateCommandQueueWithProperties(context, device_id, 0, &err);
    if (err < 0)
    {
        perror("Couldn't create a command queue");
        exit(1);
    };

    kernel = clCreateKernel(program, kernelname, &err);
    if (err < 0)
    {
        perror("Couldn't create a kernel");
        printf("error =%d", err);
        exit(1);
    };
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char *filename)
{

    cl_program program;
    FILE *program_handle;
    char *program_buffer, *program_log;
    size_t program_size, log_size;

    /* Read program file and place content into buffer */
    program_handle = fopen(filename, "r");
    if (program_handle == NULL)
    {
        perror("Couldn't find the program file");
        exit(1);
    }
    fseek(program_handle, 0, SEEK_END);
    program_size = ftell(program_handle);
    rewind(program_handle);
    program_buffer = (char *)malloc(program_size + 1);
    program_buffer[program_size] = '\0';
    fread(program_buffer, sizeof(char), program_size, program_handle);
    fclose(program_handle);

    //ToDo: Add comment (what is the purpose of clCreateProgramWithSource function? What are its arguments?)
    program = clCreateProgramWithSource(ctx, 1,
                                        (const char **)&program_buffer, &program_size, &err);
    if (err < 0)
    {
        perror("Couldn't create the program");
        exit(1);
    }
    free(program_buffer);

    /* Build program 
    The fourth parameter accepts options that configure the compilation. 
    These are similar to the flags used by gcc. For example, you can 
    define a macro with the option -DMACRO=VALUE and turn off optimization 
    with -cl-opt-disable.
    */
    err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
    if (err < 0)
    {

        /* Find size of log and print to std output */
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                              0, NULL, &log_size);
        program_log = (char *)malloc(log_size + 1);
        program_log[log_size] = '\0';
        clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG,
                              log_size + 1, program_log, NULL);
        printf("%s\n", program_log);
        free(program_log);
        exit(1);
    }

    return program;
}

cl_device_id create_device()
{

    cl_platform_id platform;
    cl_device_id dev;
    int err;

    /* Identify a platform */
    err = clGetPlatformIDs(1, &platform, NULL);
    if (err < 0)
    {
        perror("Couldn't identify a platform");
        exit(1);
    }

    // Access a device
    // GPU
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
    if (err == CL_DEVICE_NOT_FOUND)
    {
        // CPU
        printf("GPU not found\n");
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
    }
    if (err < 0)
    {
        perror("Couldn't access any devices");
        exit(1);
    }

    return dev;
}