#include <bits/stdc++.h>
#include <time.h>
#include <chrono>

using namespace std;
using namespace std::chrono;

// Function to swap elements
void swap_elements(int* a, int* b)
{
    int t = *a;
    *a = *b;
    *b = t;
}
 
// Function to perform the partitionin of array
int partition(int array[], int low, int high)
{
    // Declaring pivot point and index of smaller element
    int pivot = array[high];
    int i = (low - 1);
 
    // Rearranging the array
    for (int j = low; j <= high - 1; j++) {
        if (array[j] < pivot) {
            i++;
            swap_elements(&array[i], &array[j]);
        }
    }
    swap_elements(&array[i + 1], &array[high]);
 
    // Returning the respective index
    return (i + 1);
}
 
// Function to perform QuickSort Algorithm
void quicksort(int array[], int low, int high)
{
    // Declaration
    int index;
 
    if (low < high) 
    {
        // Getting the index of pivot by partitioning
        index = partition(array, low, high);
 
        // Evaluating the left half
        quicksort(array, low, index - 1);
        // Evaluating the right half
        quicksort(array, index + 1, high);
    }
}
 
// Driver Code
int main()
{
    // Declaring number of elements
    int num_elements = 100000;
 
    // Declaring array
    int array[num_elements];

    for (int i = 0; i < num_elements; i++)
    {
        array[i] = rand() % 100;
    }
 
    srand(time(0));

    // Returns a time point representing the starting point in time.
    auto start = high_resolution_clock::now();

    // Calling quicksort function
    quicksort(array, 0, num_elements - 1);
 
    // // Printing the sorted array
    // cout << "Array after Sorting is: \n";
 
    // for (int i = 0; i < num_elements; i++) {
    //     cout << array[i] << " ";
    // }

    cout << "Array sorted!";
    
    auto stop = high_resolution_clock::now();

    //Convert the duration time between starting point and stopping point to ToDuration type
    auto duration = duration_cast<microseconds>(stop - start);

    cout << "\nTime taken by function: "
         << duration.count() << " microseconds" << endl;
 
    return 0;
}