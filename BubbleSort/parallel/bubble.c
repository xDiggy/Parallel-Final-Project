//#include "/Users/diggy/Downloads/openMPI_testing/openmpi-5.0.2/ompi/include/mpi.h"
#include <mpi.h>
// #include <cstddef>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include "random_128.c"
#define SIZE 128
int* array = random_128;

extern void bubbleCuda(int* array, int size);

int main (int argc, char *argv[]) {
    int rank, numProcesses;

    char* filename = "128.txt";
    struct stat st;
    stat(filename, &st);

    //printf("size of %s is %lld bytes\n", filename, (long long)st.st_size);

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    int* sortedish = calloc(SIZE, sizeof(int));

    if(rank == 0){ MPI_Wtime(); }

    // number of elements per rank
    unsigned int elementsPerRank = SIZE / numProcesses;

    int* world = calloc(elementsPerRank, sizeof(int));

    MPI_Scatter(array, elementsPerRank, MPI_INT, world, elementsPerRank, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);


    // send to cuda to be bubble sorted
    bubbleCuda(world, elementsPerRank);

    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Gather(world, elementsPerRank, MPI_INT, sortedish, elementsPerRank, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);
    if (rank == 0){
        int* sorted = calloc(SIZE, sizeof(int));
        int ptrs[numProcesses];
        for(int i = 0; i < numProcesses; i++){
                ptrs[i] = i * elementsPerRank;
        }

        for(int i = 0; i < SIZE; i++){
                int least = 200;
                int index = -1;
                for(int j = 0; j < numProcesses; j++){
                        if(ptrs[j] >= SIZE || sortedish[ptrs[j]] == -1){ continue; }
                        if(sortedish[ptrs[j]] < least){
                                least = sortedish[ptrs[j]];
                                index = j;
                        }
                }
                sorted[i] = sortedish[ptrs[index]];
                sortedish[ptrs[index]] = -1;
                ptrs[index] = ptrs[index] + 1;
        }
        int valid = 1;
        for(int i = 1; i < SIZE; i++){
                //printf("%-2d ", sorted[i]);
                if (sorted[i-1] > sorted[i]){
                        valid = 0;
                        //break;
                }
                //if(i%16 == 0){ printf("\n"); }
        }


        if(valid == 0){
                printf("INCORRECT SOLUTION, TRY AGAIN\n");
                return 1;
        } else {
                double execTime = MPI_Wtime();
                printf("VALID SOLUTION\n");
                printf("Execution time: %fs\n", execTime);
        }
    } else {}

    MPI_Finalize();

    return EXIT_SUCCESS;
}