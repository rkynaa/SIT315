#include <iostream>
#include <time.h>
#include <cstdlib>
#include <chrono>
#include <pthread.h>
#include <math.h>
#include <malloc/malloc.h>
#include <omp.h>

#include "omp_kmeans.h"

using namespace std;

#define MAX_ITERATIONS 200

#define THRESHOLD 1e-4

float *centroids_global;
float *cluster_points_global;
float delta_global = THRESHOLD + 1;
double current_itr_delta_global;

double Euclidean(float *pointA, float *pointB)
{
    // Euclidean Distance Formula = 
    // ((Xa + Xb)^2 + (Ya + Yb)^2 + (Za + Zb)^2)^0.5
    double var1 = pow(((double)(*(pointB + 0)) - (double)(*(pointA + 0))), 2)
    double var2 = pow(((double)(*(pointB + 1)) - (double)(*(pointA + 1))), 2)
    double var3 = pow(((double)(*(pointB + 2)) - (double)(*(pointA + 2))), 2)
    return sqrt(var1 + var2 + var3);
}

void threadedClustering(int ThreadID, int NumPoints, int NumCluster, int NumThreads, float *DataPoints, float **ClusterPoints, float **centroids, int *iteration)
{
    printf("Inside threadedClustering %d\n", ThreadID);
    int length_per_thread = NumPoints / NumThreads;
    int start = ThreadID * length_per_thread;
    int end = start + length_per_thread;

    if (end > NumPoints)
    {
        end = NumPoints;
        length_per_thread = start - end;
    }

    double min_distance, current_distance;
    int *current_cluster_id = (int *)malloc(sizeof(int) * length_per_thread); // The cluster ID to which the data point belong to after each iteration

    int iteration_count = 0;

    while ((delta_global > THRESHOLD) && (iteration_count < MAX_ITERATIONS))
    {
        float *current_centroid = (float *)calloc(NumCluster * 3, sizeof(float)); // Coordinates of the centroids which are calculated at the end of each iteration
        int *cluster_count = (int *)calloc(NumCluster, sizeof(int));              // No. of data points which belongs to each cluster at the end of each iteration. Initialised to zero
        for (int i = start; i < end; i++)
        {
        min_distance = __DBL_MAX__; // min_distance is assigned the largest possible double value

        for (int j = 0; j < NumCluster; j++)
        {
            current_distance = Euclidean((DataPoints + (i * 3)), (centroids_global + (iteration_count * NumCluster + j) * 3));
            if (current_distance < min_distance)
            {
            min_distance = current_distance;
            current_cluster_id[i - start] = j;
            }
        }

        cluster_count[current_cluster_id[i - start]]++;

        current_centroid[current_cluster_id[i - start] * 3] += DataPoints[(i * 3)];
        current_centroid[current_cluster_id[i - start] * 3 + 1] += DataPoints[(i * 3) + 1];
        current_centroid[current_cluster_id[i - start] * 3 + 2] += DataPoints[(i * 3) + 2];
        }

    #pragma omp critical
        {
        for (int i = 0; i < K; i++)
        {
            if (cluster_count[i] == 0)
            {
            // printf("Cluster %d has no data points in it\n", i);
            continue;
            }

            // Update the centroids
            centroids_global[((iteration_count + 1) * NumCluster + i) * 3] = current_centroid[(i * 3)] / (float)cluster_count[i];
            centroids_global[((iteration_count + 1) * NumCluster + i) * 3 + 1] = current_centroid[(i * 3) + 1] / (float)cluster_count[i];
            centroids_global[((iteration_count + 1) * NumCluster + i) * 3 + 2] = current_centroid[(i * 3) + 2] / (float)cluster_count[i];
        }
        }

        // Find delta value after each iteration in all the threads
        double current_delta = 0.0;
        for (int i = 0; i < NumCluster; i++)
        {
        current_delta += Euclidean((centroids_global + (iteration_count * NumCluster + i) * 3), (centroids_global + ((iteration_count - 1) * NumCluster + i) * 3));
        }

        // Store the largest delta value among all delta values in all the threads
    #pragma omp barrier
        {
        if (current_delta > current_itr_delta_global)
            current_itr_delta_global = current_delta;
        }

    #pragma omp barrier
        iteration_count++;

        // Set the global delta value and increment the number of iterations
    #pragma omp master
        {
        delta_global = current_itr_delta_global;
        current_itr_delta_global = 0.0;
        (*iteration)++;
        }
    }

    // Update the ClusterPoints
    for (int i = start; i < end; i++)
    {
        cluster_points_global[i * 4] = DataPoints[i * 3];
        cluster_points_global[i * 4 + 1] = DataPoints[i * 3 + 1];
        cluster_points_global[i * 4 + 2] = DataPoints[i * 3 + 2];
        cluster_points_global[i * 4 + 3] = (float)current_cluster_id[i - start];
    }

    // printf("After updating the cluster points %d\n", ThreadID);
}

void KMeansParallel(int NumPoints, int NumCluster, int NumThreads, float *DataPoints, float **ClusterPoints, float **centroids, int *iteration)
{
  // 7 arguments
  // NumCluster              - Number of clusters to be created (read only)
  // NumThreads    - No. of threads using which the algorithm should be executed (read only)
  // DataPoints    - 3 dimensional data points (read only)
  // ClusterPoints - 3 dimensional data points along with the ID of the cluster to which they belong (write)
  // centroids      - 3 dimensional coordinate values of the centroids of the K cluster (write)
  // iteration - Number of iterations taken to complete the algorithm (write)

  *ClusterPoints = (float *)malloc(sizeof(float) * NumPoints * 4);
  cluster_points_global = *ClusterPoints;

  // calloc intitalizes the values to zero
  centroids_global = (float *)calloc(MAX_ITERATIONS * NumCluster * 3, sizeof(float));

  // Assigning the first K data points to be the centroids of the K clusters
  for (int i = 0; i < NumCluster; i++)
  {
    centroids_global[(i * 3)] = DataPoints[(i * 3)];
    centroids_global[(i * 3) + 1] = DataPoints[(i * 3) + 1];
    centroids_global[(i * 3) + 2] = DataPoints[(i * 3) + 2];
  }

  omp_set_num_threads(NumThreads);

#pragma omp parallel
  {
    int ThreadID = omp_get_thread_num();
    printf("Thread no. %d created\n", ThreadID);
    threadedClustering(ThreadID, NumPoints, NumCluster, NumThreads, DataPoints, ClusterPoints, centroids, iteration);
  }

  int centroids_size = (*iteration + 1) * NumCluster * 3;
  *centroids = (float *)calloc(centroids_size, sizeof(float));
  for (int i = 0; i < centroids_size; i++)
  {
    (*centroids)[i] = centroids_global[i];
  }

  printf("Process Completed\n");
  printf("Number of iterations: %d\n", *iteration);
  for (int i = 0; i < NumCluster; i++)
  {
    printf("Final Centroids:\t(%f, %f, %f)\n", *(*centroids + ((*iteration) * NumCluster) + (i * 3)), *(*centroids + ((*iteration) * NumCluster) + (i * 3) + 1), *(*centroids + ((*iteration) * NumCluster) + (i * 3) + 2));
  }
}

void readDataset(const char *filename, int *N, float **data_points)
{
	FILE *fptr = fopen(filename, "r");

	fscanf(fptr, "%d", N);

	printf("No. of data points in the dataset: %d\n", *N);

	// Each data point is of 3 dimension
	*data_points = (float *)malloc(((*N) * 3) * sizeof(float));

	for (int i = 0; i < (*N) * 3; i++)
	{
		int temp;
		fscanf(fptr, "%d", &temp);
		*(*data_points + i) = temp;
	}

	fclose(fptr);
}

void writeClusters(const char *filename, int N, float *cluster_points)
{
	FILE *fptr = fopen(filename, "w");

	for (int i = 0; i < N; i++)
	{
		fprintf(fptr, "%f %f %f %f\n", *(cluster_points + (i * 4)), *(cluster_points + (i * 4) + 1), *(cluster_points + (i * 4) + 2), *(cluster_points + (i * 4) + 3));
	}

	fclose(fptr);
}

void writeCentroids(const char *filename, int K, int num_iterations, float *centroids)
{
	FILE *fptr = fopen(filename, "w");

	for (int i = 0; i < num_iterations; i++)
	{
		for (int j = 0; j < K; j++)
		{
			fprintf(fptr, "%f %f %f, ", *(centroids + (i * K) + (j * 3)), *(centroids + (i * K) + (j * 3) + 1), *(centroids + (i * K) + (j * 3) + 2));
		}
		fprintf(fptr, "\n");
	}

	fclose(fptr);
}

int main(int argc, char const *argv[])
{
	/* Correct structure of command line arguments
			- ./a.out
	 		- dataset filename, 
			- number of clusters,
			- number of threads,
			- filename to write the clustered data points,
			- filename to write the coordinated of the centroid
	 */

	if (argc < 6)
	{
		printf("Less no. of command line arguments\n\n");
		return 0;
	}
	else if (argc > 6)
	{
		printf("Too many command line arguments\n\n");
		return 0;
	}

	// Correct no. of command line arguments

	const char *dataset_filename = argv[1];
	const int K = atoi(argv[2]);
	const int num_threads = atoi(argv[3]);
	const char *data_points_output_filename = argv[4];
	const char *centroids_output_filename = argv[5];

	int N;								 // Total no. of data_points in the file
	float *data_points;		 // 2D array of 3 dimensional data points
	float *cluster_points; // 2D array of data points along with the ID of the cluster they belong to
	float *centroids;			 // 2D array of the coordinates of the centroids of clusters
	int num_iterations = 0;

	readDataset(dataset_filename, &N, &data_points);

	double start_time = omp_get_wtime();
	printf("start time: %lf\n", start_time);
	kmeansClusteringOmp(N, K, num_threads, data_points, &cluster_points, &centroids, &num_iterations);
	double end_time = omp_get_wtime();

	printf("Time Taken: %lfs\n", end_time - start_time);

	writeClusters(data_points_output_filename, N, cluster_points);
	writeCentroids(centroids_output_filename, K, num_iterations, centroids);

	return 0;
}