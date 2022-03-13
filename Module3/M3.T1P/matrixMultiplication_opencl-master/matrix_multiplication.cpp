#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <CL/cl.h>
#define N 40
#define MEMORY_SIZE (4)
#define MAXIMUM_SIZE (0x100000)
#define BLOCK_SIZE 1
 
int main()
{
  int width = N;
  float * A = (float *)malloc(sizeof(float)*width*width);
  float * B = (float *)malloc(sizeof(float)*width*width);
  float * C = (float *)malloc(sizeof(float)*width*width);
  float * Res = (float *)malloc(sizeof(float)*width*width);
  float * D= (float *)malloc(sizeof(float)*width*width);
  printf("\nMatrix 1 : \n");
  for(int i = 0;i < width; i++)
  {
        for(int j=0;j<width;j++)       {
            *(A+i*width+j)=((j*rand())+1)%10000;
            printf("%f ",*(A+i*width+j));
        }
   }
  printf("\nMatrix 2 : \n");
    for(int i = 0;i < width; i++)
    {
        for(int j=0; j<width;j++)     {
            *((B+i*width+j))=((j*rand())+1)%10000;
            printf("%f ",*(B+i*width+j));
        }
    }
  
  cl_event prof_event;
  cl_command_queue comm;

   
  cl_device_id device_id = NULL;
  cl_context context = NULL;
  cl_command_queue command_queue = NULL;
  cl_mem memobjA = NULL;
  cl_mem memobjB = NULL;
  cl_mem memobjC = NULL;
  cl_mem rowA = NULL;
  cl_mem colC = NULL;
  cl_program program = NULL;
  cl_kernel kernel = NULL;
  cl_platform_id platform_id = NULL;
  cl_uint ret_num_devices;
  cl_uint ret_num_platforms;
  cl_int ret;
 
 
  FILE *file_ptr;
  char filename[] = "./matrix_multiplication.cl";
  char *source_str;
  size_t source_size;
  int row = width;
  int col = width;
  /* Load the source code containing the kernel*/
  
  file_ptr = fopen(filename, "r");
  source_str = (char*)malloc(MAXIMUM_SIZE);
  source_size = fread( source_str, 1, MAXIMUM_SIZE, file_ptr);
  fclose( file_ptr );
 
  /* Get Platform and Device Info */
  ret = clGetPlatformIDs(1, &platform_id, &ret_num_platforms);
  ret = clGetDeviceIDs( platform_id, CL_DEVICE_TYPE_GPU, 1, &device_id, &ret_num_devices);
 
  /* Create OpenCL context */
  context = clCreateContext( NULL, 1, &device_id, NULL, NULL, &ret);
 
  /* Create Command Queue */
  command_queue = clCreateCommandQueue(context, device_id, CL_QUEUE_PROFILING_ENABLE, &ret);
 
  /* Create Memory Buffer */
  memobjA = clCreateBuffer(context, CL_MEM_READ_WRITE, width * width * sizeof(float), NULL, &ret);
  memobjB = clCreateBuffer(context, CL_MEM_READ_WRITE, width * width * sizeof(float), NULL, &ret);
  memobjC = clCreateBuffer(context, CL_MEM_READ_WRITE, width * width * sizeof(float), NULL, &ret);
  rowA = clCreateBuffer(context, CL_MEM_READ_WRITE,  sizeof(int), NULL, &ret);
  colC = clCreateBuffer(context, CL_MEM_READ_WRITE,  sizeof(int), NULL, &ret);
 
  // Copy the lists A and B to their respective memory buffers
    ret = clEnqueueWriteBuffer(command_queue,memobjA, CL_TRUE, 0,
           width * width * sizeof(int), A, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, memobjB, CL_TRUE, 0,
            width * width * sizeof(int), B, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, rowA, CL_TRUE, 0, sizeof(int), &row, 0, NULL, NULL);
    ret = clEnqueueWriteBuffer(command_queue, colC, CL_TRUE, 0, sizeof(int), &col, 0, NULL, NULL);
 
  /* Create Kernel Program from the source */
  program = clCreateProgramWithSource(context, 1, (const char **)&source_str,
                                      (const size_t *)&source_size, &ret);
 
  /* Build Kernel */
  ret = clBuildProgram(program, 1, &device_id, NULL, NULL, NULL);
 
  /* Create the OpenCL Kernel */
  kernel = clCreateKernel(program, "matrixMultiplication", &ret);
 
  /* Set Kernel Args */
  ret = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&memobjA);
  ret = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&memobjB);
  ret = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&memobjC);
  ret = clSetKernelArg(kernel, 3, sizeof(int), (void *)&row);
  ret = clSetKernelArg(kernel, 4, sizeof(int), (void *)&col);
  /* Execute Kernel */
  size_t global[2] = {width, width};
  size_t local[2] = {BLOCK_SIZE,BLOCK_SIZE};


  int err;
 
  err = clEnqueueNDRangeKernel(command_queue, kernel, 2, NULL, global, local, 0, 0, &prof_event);

  clFinish(command_queue);
  err = clWaitForEvents(1,&prof_event );

  cl_ulong start_time, end_time;
  size_t return_bytes;
  double run_time;


  err = clGetEventProfilingInfo( prof_event, CL_PROFILING_COMMAND_START, sizeof(cl_ulong), &start_time, &return_bytes);
  err = clGetEventProfilingInfo( prof_event, CL_PROFILING_COMMAND_END, sizeof(cl_ulong), &end_time, &return_bytes); 

  run_time = (double) (end_time - start_time);
 
  printf("\nOpenCl Execution time is: %0.6f milliseconds \n",run_time/1000000);

  ret = clEnqueueReadBuffer(command_queue, memobjC, CL_TRUE, 0,
                            width * width * sizeof(float),Res, 0, NULL, NULL);
 
  printf("\nThe Matrix Result : \n");
    for(int i = 0;i < width; i++)
    {
        for(int j=0;j < width; j++)
        {
 
            printf("%f ",*(Res+i*width+j));
 
        }
    }
 
  ret = clFlush(command_queue);
  ret = clFinish(command_queue);
  ret = clReleaseKernel(kernel);
  ret = clReleaseProgram(program);
  ret = clReleaseMemObject(memobjA);
  ret = clReleaseMemObject(memobjB);
  ret = clReleaseMemObject(memobjC);
  ret = clReleaseCommandQueue(command_queue);
  ret = clReleaseContext(context);
 
  free(source_str);
 
  float sum=0.0;
 
  for(int i = 0;i < width; i++)
    {
        for(int j = 0; j < width; j++)
        {
            sum = 0;
            for(int k = 0; k < width; k++)
            {
                sum += A[i*col+k] * B[k*row+j];
            }
        D[i*width+j] = sum;
        }
 
    }
  printf("\nResults : \n");
    for(int i = 0;i < width; i++)
    {
        for(int j=0;j < width; j++)
        {
            printf("%f ",*(D+i*width+j));
 
        }
    }

 printf("\nOpenCl Execution time is: %0.6f milliseconds \n",run_time/1000000);
  return 0;
}
