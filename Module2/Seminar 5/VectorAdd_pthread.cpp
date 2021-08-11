#include <iostream>
#include <cstdlib>
#include <time.h>
#include <pthread.h>
#include <chrono>

using namespace std::chrono;
using namespace std;

struct struct_vector
{
    int *vector;
    unsigned long size;
};

void randomVector(int vector[])
{
    for (int i = 0; i < sizeof(vector) / sizeof(vector[0]); i++)
    {
        //ToDo: Adding random elements to the vector between 1 and 99
        vector[i] = rand() % 100;
    }
}

//ToDo: Allocating memories for each vectors
int *allocateMemory(unsigned long size)
{
    int *vector;
    vector = (int *)malloc(size * sizeof(int *));
    return vector;
}

//ToDo: Allocating memories for each vectors
void *allocate_memory_pthread(void *arg)
{
    // detach the current thread
    // from the calling thread
    pthread_detach(pthread_self());

    struct struct_vector *vect = (struct struct_vector *)arg;
    vect->vector = allocateMemory(vect->size);

    //printf("Inside allocate memory thread\n");

    // exit the current thread
    pthread_exit(NULL);
}

void *randomVector_pthread(void *arg)
{
    // detach the current thread
    // from the calling thread
    pthread_detach(pthread_self());

    struct struct_vector *vect = (struct struct_vector *)arg;
    randomVector(vect->vector);

    //printf("Inside random vector thread\n");

    // exit the current thread
    pthread_exit(NULL);
}

int main()
{

    unsigned long size = 100000000;

    srand(time(0));

    struct struct_vector struct_v1;
    struct struct_vector struct_v2;
    struct struct_vector struct_v3;

    struct_v1.size = size;
    struct_v2.size = size;
    struct_v3.size = size;

    //int *v1, *v2, *v3;

    //ToDo: Returns a time point representing the starting point in time.
    auto start = high_resolution_clock::now();

    pthread_t allocate_v1, allocate_v2, allocate_v3;

    pthread_create(&allocate_v1, NULL, &allocate_memory_pthread, (void *)&struct_v1);
    pthread_create(&allocate_v2, NULL, &allocate_memory_pthread, (void *)&struct_v2);
    pthread_create(&allocate_v3, NULL, &allocate_memory_pthread, (void *)&struct_v3);

    pthread_join(allocate_v1, NULL);
    pthread_join(allocate_v2, NULL);
    pthread_join(allocate_v3, NULL);

    pthread_t randVect1, randVect2;

    pthread_create(&randVect1, NULL, &randomVector_pthread, (void *)&struct_v1);
    pthread_create(&randVect2, NULL, &randomVector_pthread, (void *)&struct_v2);

    pthread_join(randVect1, NULL);
    pthread_join(randVect2, NULL);

    //ToDo: Addding element to the third vector based on first and second vector elements addition
    for (int i = 0; i < size; i++)
    {
        struct_v3.vector[i] = struct_v1.vector[i] + struct_v2.vector[i];
    }

    auto stop = high_resolution_clock::now();

    //ToDo: Convert the duration time between starting point and stopping point to ToDuration type
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "Time taken by function: "
         << duration.count() << " microseconds" << endl;

    return 0;
}