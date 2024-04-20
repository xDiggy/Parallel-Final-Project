#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "random_array_100.c"
#include "random_array_10000.c"
#include "random_array_100000.c"

#include <stdbool.h>
#include <stdio.h>

void swap(int* x, int* y);
void bubble(int array[], int length);
void printArray(int array[], int length);
void timerAnalysis(struct timeval start, struct timeval end, char* customMessage);


int main (int argc, char *argv[]) {

    // 100
    struct timeval bubble100Start, bubble100After;
    gettimeofday(&bubble100Start, NULL);
    bubble(random_100, 100);
    gettimeofday(&bubble100After, NULL);
    timerAnalysis(bubble100Start, bubble100After, "Bubble sort 100");

    // 10.000
    struct timeval bubble10000Start, bubble10000After;
    gettimeofday(&bubble10000Start, NULL);
    bubble(random_10000, 10000);
    gettimeofday(&bubble10000After, NULL);
    timerAnalysis(bubble10000Start, bubble10000After, "Bubble sort 10000");

    // 100.000
    struct timeval bubble100000Start, bubble100000After;
    gettimeofday(&bubble100000Start, NULL);
    bubble(random_100000, 100000);
    gettimeofday(&bubble100000After, NULL);
    timerAnalysis(bubble100000Start, bubble100000After, "Bubble sort 100000");

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