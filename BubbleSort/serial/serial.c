#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "random_array_100.c"
#include "random_array_1000.c"
#include "random_array_5000.c"
#include "random_array_10000.c"
#include "random_array_50000.c"
#include "random_array_100000.c"

#include <stdbool.h>
#include <stdio.h>

void swap(int* x, int* y);
void bubble(int array[], int length);
void merge(int arr[], int l, int m, int r);
void mergeSort(int arr[], int l, int r);
void printArray(int array[], int length);
void timerAnalysis(struct timeval start, struct timeval end, char* customMessage);


int main (int argc, char *argv[]) {

    // BUBBLESORT -------------------------------------------------------------

    // 100
    struct timeval bubble100Start, bubble100After;
    gettimeofday(&bubble100Start, NULL);
    bubble(random_100, 100);
    gettimeofday(&bubble100After, NULL);
    timerAnalysis(bubble100Start, bubble100After, "Bubble sort 100");

    // 1000
    struct timeval bubble1000Start, bubble1000After;
    gettimeofday(&bubble1000Start, NULL);
    bubble(random_1000, 1000);
    gettimeofday(&bubble1000After, NULL);
    timerAnalysis(bubble1000Start, bubble1000After, "Bubble sort 1000");

    // 5000
    struct timeval bubble5000Start, bubble5000After;
    gettimeofday(&bubble5000Start, NULL);
    bubble(random_5000, 5000);
    gettimeofday(&bubble5000After, NULL);
    timerAnalysis(bubble5000Start, bubble5000After, "Bubble sort 5000");

    // 10.000
    struct timeval bubble10000Start, bubble10000After;
    gettimeofday(&bubble10000Start, NULL);
    bubble(random_10000, 10000);
    gettimeofday(&bubble10000After, NULL);
    timerAnalysis(bubble10000Start, bubble10000After, "Bubble sort 10000");

    // 50.000
    struct timeval bubble50000Start, bubble50000After;
    gettimeofday(&bubble50000Start, NULL);
    bubble(random_50000, 50000);
    gettimeofday(&bubble50000After, NULL);
    timerAnalysis(bubble50000Start, bubble50000After, "Bubble sort 50000");

    // 100.000
    struct timeval bubble100000Start, bubble100000After;
    gettimeofday(&bubble100000Start, NULL);
    bubble(random_100000, 100000);
    gettimeofday(&bubble100000After, NULL);
    timerAnalysis(bubble100000Start, bubble100000After, "Bubble sort 100000");

    // MERGESORT -------------------------------------------------------------

    // 100
    struct timeval merge100Start, merge100After;
    gettimeofday(&merge100Start, NULL);
    mergeSort(random_100, 0, 99);
    gettimeofday(&merge100After, NULL);
    timerAnalysis(merge100Start, merge100After, "Merge sort 100");

    // 1000
    struct timeval merge1000Start, merge1000After;
    gettimeofday(&merge1000Start, NULL);
    mergeSort(random_1000, 0, 999);
    gettimeofday(&merge1000After, NULL);
    timerAnalysis(merge1000Start, merge1000After, "Merge sort 1000");

    // 5000
    struct timeval merge5000Start, merge5000After;
    gettimeofday(&merge5000Start, NULL);
    mergeSort(random_5000, 0, 4999);
    gettimeofday(&merge5000After, NULL);
    timerAnalysis(merge5000Start, merge5000After, "Merge sort 5000");

    // 10.000
    struct timeval merge10000Start, merge10000After;
    gettimeofday(&merge10000Start, NULL);
    mergeSort(random_10000, 0, 9999);
    gettimeofday(&merge10000After, NULL);
    timerAnalysis(merge10000Start, merge10000After, "Merge sort 10000");

    // 50.000
    struct timeval merge50000Start, merge50000After;
    gettimeofday(&merge50000Start, NULL);
    mergeSort(random_50000, 0, 49999);
    gettimeofday(&merge50000After, NULL);
    timerAnalysis(merge50000Start, merge50000After, "Merge sort 50000");

    // 100.000
    struct timeval merge100000Start, merge100000After;
    gettimeofday(&merge100000Start, NULL);
    mergeSort(random_100000, 0, 99999);
    gettimeofday(&merge100000After, NULL);
    timerAnalysis(merge100000Start, merge100000After, "Merge sort 100000");
    return EXIT_SUCCESS;
}

void swap(int* x, int* y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

void bubble(int array[], int length)
{
    bool flag;
    for (int x = 0; x < length - 1; x++) {
        flag = false;
        for (int y = 0; y < length - x - 1; y++) {
            if (array[y] > array[y + 1]) {
                swap(&array[y], &array[y + 1]);
                flag = true;
            }
        }

        if (!flag){
            break;
        }
    }
}

// Merges two subarrays of arr[]. 
// First subarray is arr[l..m] 
// Second subarray is arr[m+1..r] 
void merge(int arr[], int l, int m, int r) 
{ 
    int i, j, k; 
    int n1 = m - l + 1; 
    int n2 = r - m; 
  
    // Create temp arrays 
    int L[n1], R[n2]; 
  
    // Copy data to temp arrays 
    // L[] and R[] 
    for (i = 0; i < n1; i++) 
        L[i] = arr[l + i]; 
    for (j = 0; j < n2; j++) 
        R[j] = arr[m + 1 + j]; 
  
    // Merge the temp arrays back 
    // into arr[l..r] 
    // Initial index of first subarray 
    i = 0; 
  
    // Initial index of second subarray 
    j = 0; 
  
    // Initial index of merged subarray 
    k = l; 
    while (i < n1 && j < n2) { 
        if (L[i] <= R[j]) { 
            arr[k] = L[i]; 
            i++; 
        } 
        else { 
            arr[k] = R[j]; 
            j++; 
        } 
        k++; 
    } 
  
    // Copy the remaining elements 
    // of L[], if there are any 
    while (i < n1) { 
        arr[k] = L[i]; 
        i++; 
        k++; 
    } 
  
    // Copy the remaining elements of 
    // R[], if there are any 
    while (j < n2) { 
        arr[k] = R[j]; 
        j++; 
        k++; 
    } 
} 
  
// l is for left index and r is 
// right index of the sub-array 
// of arr to be sorted 
void mergeSort(int arr[], int l, int r) 
{ 
    if (l < r) { 
        // Same as (l+r)/2, but avoids 
        // overflow for large l and r 
        int m = l + (r - l) / 2; 
  
        // Sort first and second halves 
        mergeSort(arr, l, m); 
        mergeSort(arr, m + 1, r); 
  
        merge(arr, l, m, r); 
    } 
} 
void printArray(int array[], int length)
{
    for (int x = 0; x < length; x++){
        printf("%-3d ", array[x]);

        if((x+1)%10 == 0 && x != 0){
            printf("\n");
        }
    }
}

void timerAnalysis(struct timeval start, struct timeval end, char* customMessage)
{
    double totalTime = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Time taken: %f\n", totalTime);
    printf("\t%s\n", customMessage);
}