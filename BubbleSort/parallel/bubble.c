//#include "/Users/diggy/Downloads/openMPI_testing/openmpi-5.0.2/ompi/include/mpi.h"
#include <mpi.h>
// #include <cstddef>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
//#include "random_128.c"

#define SIZE 5000
#define FILENAME "random_array_5000.txt"

extern void bubbleCuda(int* array, int size);

int main (int argc, char *argv[]) {
    int rank, numProcesses;


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &numProcesses);

    //FILE* outfile;
    //char* outfileName = "output.txt";

    //outfile = fopen(outfileName, "w");

    int* sortedish = calloc(SIZE, sizeof(int));
    double IO_time = 0;

    if(rank == 0){ MPI_Wtime(); }
    //////////
    int totalElements = SIZE;
    MPI_File fh;
    MPI_Offset offset = rank * (totalElements / numProcesses) * 3;
    int count = 3 * (totalElements / numProcesses);
    char data[count];
    MPI_Status status;
    double beforeOpen = MPI_Wtime();
    MPI_File_open(MPI_COMM_WORLD, FILENAME, MPI_MODE_RDONLY, MPI_INFO_NULL, &fh);
    MPI_File_read_at(fh, offset, data, count, MPI_CHAR, &status);
    MPI_File_close(&fh);
    double afterClose = MPI_Wtime();
    IO_time += afterClose - beforeOpen;

    //MPI_File x;
    //MPI_Offset o;
    //MPI_File_open(MPI_COMM_WORLD, "mpi_output.out", MPI_MODE_WRONLY | MPI_MODE_CREATE, MPI_INFO_NULL, &x);
    //o = rank * sizeof(double);
    //MPI_File_write_at(x, o, &IO_Time, 1, MPI_DOUBLE, MPI_STATUS_IGNORE);
    //MPI_File_close(&x);
    //fprintf(outfile, "Rank %d IO Time: %fs\n", rank, IO_Time);
    //printf("Rank %d IO Time: %fs\n", rank, IO_Time);

    int* mini = calloc(totalElements / numProcesses, sizeof(int));
    int tracker = 0;
    int n = 0;
    //char* buffer = calloc(2, sizeof(char));
    //int flag = 0;
    int c = 0;
    for(int i = 0; i < count; i++){
        if(data[i] == ' ' && n == 0){ continue; }
        if(data[i] == ' ' && n != 0){
            mini[tracker++] = n;
            n = 0;
            continue;
        }
        n = (n * 10) + (data[i] - '0');
    }


    //MPI_Finalize();
    //return 0;

    //////////
    // number of elements per rank
    unsigned int elementsPerRank = SIZE / numProcesses;

    //int* world = calloc(elementsPerRank, sizeof(int));
    //MPI_Scatter(array, elementsPerRank, MPI_INT, world, elementsPerRank, MPI_INT, 0, MPI_COMM_WORLD);
    //MPI_Barrier(MPI_COMM_WORLD);


    // send to cuda to be bubble sorted
    //bubbleCuda(world, elementsPerRank);
    bubbleCuda(mini, elementsPerRank);
    MPI_Barrier(MPI_COMM_WORLD);

    MPI_Gather(mini, elementsPerRank, MPI_INT, sortedish, elementsPerRank, MPI_INT, 0, MPI_COMM_WORLD);
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
                        printf("i-1: %d\n", sorted[i-1]);
                        printf("i, %d: %d\n", i, sorted[i]);
                        break;
                }
                //if(i%16 == 0){ printf("\n"); }
        }

        if(valid == 0){
                printf("INCORRECT SOLUTION, TRY AGAIN\n");
                return 1;
        } else {
                double execTime = MPI_Wtime();
                printf("VALID SOLUTION\n");
                //fprintf(outfile, "VALID SOLUTION\n");
                printf("Execution time: %fs\n", execTime);
                //fprintf(outfile, "Total Execution time: %fs\n", execTime);
                printf("IO time: %f", IO_time);
        }
        free(sorted);
    } else {}

    free(mini);
    free(sortedish);

    MPI_Finalize();

    return EXIT_SUCCESS;
}