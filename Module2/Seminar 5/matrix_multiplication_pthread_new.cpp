#include <iostream>
#include <time.h>
#include <cstdlib>
#include <chrono>
#include <pthread.h>

using namespace std;
using namespace std::chrono;

#define NUM_THREADS 5
#define MAX_SIZE 2000

// initialisation of matrice 
int size, rows, cols;
int i = 0;

int MatA[MAX_SIZE][MAX_SIZE];
int MatB[MAX_SIZE][MAX_SIZE];
int MatC[MAX_SIZE][MAX_SIZE];


// Multiply MatA and MatB -> MatC
void* mul_matrix(void* arg)
{
	int thread_num = ++i;
	for (int j = 0; j < size; ++j)
	{
		MatC[i][j] = 0;
		for (int k = 0; k < size; ++k)
		{
			MatC[i][j] += MatA[i][k] * MatB[k][j];
		}
	}
}

int main(){
	// seed "real" random value
	srand(time(0));
	
	// read size
	cout << "Enter a number N (N rows x N cols): ";
	cin >> size;


    // loop all elements in the matrix.
	for (int i = 0; i < size; ++i)
	{
		for (int j = 0; j < size; ++j)
		{
			// Generate random number from 0-9 and store it in matrix
			MatA[i][j] = rand() % 100;
			MatB[i][j] = rand() % 100;

		}
	}

	// declaring threads
	pthread_t threads[NUM_THREADS];
	
	// start time
	auto start = high_resolution_clock::now();
	
	// create threads to perform multiplication separately
	for (int i = 0; i < NUM_THREADS; i++) {
		pthread_create(&threads[i], NULL, mul_matrix, (void*)(i));
	}

	// join all threads
	for (int i = 0; i < NUM_THREADS; i++){
		pthread_join(threads[i], NULL);
	}
	

	// stop time
	auto stop = high_resolution_clock::now();

	// execution time 
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "\nTime taken by function with pthread: "
         << duration.count() 
         << " microseconds" 
         << endl;

    return 0;
}	

