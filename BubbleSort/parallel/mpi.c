#include "/Users/diggy/Downloads/openMPI_testing/openmpi-5.0.2/ompi/include/mpi.h"
// #include <mpi.h>
//  #include <cstddef>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// int random_100[100];
#include "random_array_100.c"
#define SIZE 100

int* array = random_100;

int main (int argc, char *argv[]) {

    int rank, numProcesses;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    unsigned int elementsPerRank = SIZE / numProcesses;

    int* world = calloc(elementsPerRank, sizeof(int));

    if (rank == 0){} else {}

    MPI_Scatter(array, elementsPerRank, MPI_INT, world, elementsPerRank, MPI_INT, 0, MPI_COMM_WORLD);
    MPI_Barrier(MPI_COMM_WORLD);

    // CUDA BUBBLE SORT
    // GATHER CUDA
    // SEND TO RANK 0
    // MERGE SORT

    MPI_Finalize();

    return EXIT_SUCCESS;
}