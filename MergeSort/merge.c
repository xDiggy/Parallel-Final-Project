#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

extern void mergeCuda(int* array, int startindex, int endindex, int threadCount);

// salloc -N 1 --partition=el8-rpi --gres=gpu:4 -t 30
// module load xl_r spectrum-mpi cuda/11.2
// nvcc -g -G merge.cu -c -o mergecu
// mpicc merge.c mergecu -o mergeout  -L/usr/local/cuda-11.2/lib64/ -lcudadevrt -lcudart -lstdc++
// mpiexec -np 25 mergeout

int main(int argc, char** argv) {

    int original[] = {7, 49, 73, 58, 30, 72, 44, 78, 23, 9,
                40, 65, 92, 42, 87, 3, 27, 29, 40, 12,
                3, 69, 9, 57, 60, 33, 99, 78, 16, 35, 97,
                26, 12, 67, 10, 33, 79, 49, 79, 21, 67,
                72, 93, 36, 85, 45, 28, 91, 94, 57, 1,
                53, 8, 44, 68, 90, 24, 96, 30, 3, 22,
                66, 49, 24, 1, 53, 77, 8, 28, 33, 98,
                81, 35, 13, 65, 14, 63, 36, 25, 69, 15,
                94, 29, 1, 17, 95, 5, 4, 51, 98, 88, 23,
                5, 82, 52, 66, 16, 37, 38, 44};

    int len = 100;
    

    MPI_Init(&argc, &argv);

    /*
    MPI_Status status;
    MPI_Request send_request;
    MPI_Request recv_request;
    */

    // Get the number of processes
    int cores;
    MPI_Comm_size(MPI_COMM_WORLD, &cores);

    // Get the rank of the process
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);

    

    // how much of the array does each rank deal with?
    int subLength;
    int subCount;
    for (int i = cores; i > 0; i--){
        if (len % i == 0){
            subCount = i;
            subLength = len / i;
            i = 0;
        }
    }


    // COMPUTATIOn IS HERE:
    
    //if (rank < subCount){
    if (rank == 3){

        // make the subarray for this mpi rank
        int startindex;
        int endindex;
        startindex =  rank*subLength;
        endindex = (rank*subLength)+(subLength-1);

        // pass this to the device to do merge sort in parallel
        int threadCount = 16;
        mergeCuda(original, startindex, endindex , threadCount);

        // after completing merge sort for this subarray, we combine the results for the final sorted array
        // tbd

    }

    // Finalize the MPI environment.
    MPI_Finalize();

    return EXIT_SUCCESS;

}