#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

extern void mergeCuda(int* array, int startindex, int endindex, int threadCount, int* result);

// salloc -N 1 --partition=el8-rpi --gres=gpu:4 -t 30
// module load xl_r spectrum-mpi cuda/11.2
// nvcc -g -G merge.cu -c -o mergecu
// mpicc temp.c mergecu -o mergeout  -L/usr/local/cuda-11.2/lib64/ -lcudadevrt -lcudart -lstdc++
// mpiexec mergeout


void mergeSort(int* array, int lsize, int rsize, int* result){
    //printf("Size of left: %d, size of right %d\n",lsize,rsize);
    int l = 0;
    int r = 0;
    int added = 0;

    while (l < lsize && r < rsize){
        int lindex = l;
        int lval = array[lindex];
        int rindex = lsize+r;
        int rval = array[rindex];

        if (lval == rval){
            result[added] = lval;
            added++;
            result[added] = rval;
            added++;
            r++;
            l++;

           // printf("1 %d == %d\n",lval, rval);
        }
        else if (lval > rval){
            result[added] = rval;
            r++;
            added++;
           // printf("2 %d > %d\n",lval,rval);
        }
        else if (rval > lval){
            result[added] = lval;
            l++;
            added++;
            //printf("3 %d < %d\n",lval,rval);
        }

    }
    while (l < lsize && r >= rsize){
        int lindex = l;
        int lval = array[lindex];
        result[added] = lval;
        added++;
        l++;
        //rintf("4 %d finished right\n",lval);
    }
    while (r < rsize && l >= lsize){
        int rindex = lsize+r;
        int rval = array[rindex];
        result[added] = rval;
        added++;
        r++;
        //printf("5 %d finished left\n",rval);
    }

}

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


    if (rank == 0){
        printf("Original List: [");
        for (int i = 0; i < 100; i++){
            printf("%d ",original[i]);
        }
        printf("]\n");
    }

    

    // how much of the array does each rank deal with?
    int subLength = 1;
    int subCount = 1;
    for (int i = cores; i > 0; i--){
        if (len % i == 0 && i%2 == 0){
            subCount = i;
            subLength = len / i;
            i = 0;
        }
    }



    // COMPUTATIOn IS HERE:
    MPI_Barrier(MPI_COMM_WORLD);
    int* subsorted = malloc(subLength*sizeof(int));

    if (rank < subCount){
        // make the subarray for this mpi rank
        int startindex;
        int endindex;
        startindex =  rank*subLength;
        endindex = (rank*subLength)+(subLength-1);

        // pass this to the device to do merge sort in parallel
        int threadCount = 64;
        
        mergeCuda(original, startindex, endindex , threadCount, subsorted);
        

        /*
        printf("Subsorted %d: [",rank);
        for (int i = 0; i < subLength-1; i++){
            printf("%d, ",subsorted[i]);   
        }
        printf("%d]\n",subsorted[subLength-1]);   
        */
        
        

    }

    MPI_Barrier(MPI_COMM_WORLD);

    // after completing merge sort for this subarray, we combine the results for the final sorted array
    // ensure all cores have finished computing their subarray

    // helper variable to keep track of extra items in rank 0
    int adjuster = 0;
    while(subCount > 1 && rank < subCount){
        if (rank >= subCount/2){
            // sender
            if (rank == subCount-1 && subCount%2 == 1){
                MPI_Send(subsorted, subLength, MPI_INT, 0, 0, MPI_COMM_WORLD);
            }
            else{
                int dest = rank - (subCount/2);
                MPI_Send(subsorted, subLength, MPI_INT, dest, 0, MPI_COMM_WORLD);
            }
        }
        else{
            // reciever
            int src = rank+(subCount/2);
            if (subLength > 0){
                subsorted = realloc(subsorted, (subLength*2+adjuster)*sizeof(int));
                MPI_Recv(subsorted+subLength+adjuster, subLength, MPI_INT, src, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                int* result = calloc((subLength*2+adjuster), sizeof(int));
                mergeSort(subsorted, subLength+adjuster, subLength, result);
                free (subsorted);
                subsorted = result;
            }
            

            
            if (rank == 0 && subLength*2+adjuster == 100){
                printf("Sorted List: [");
                for (int i = 0; i < subLength*2+adjuster; i++){
                    printf("%d ",subsorted[i]);
                }
                printf("]\n");
            }

            if (rank == 0 && subCount%2 == 1 && subCount){
                int newsize = subLength*3+adjuster;
                subsorted = realloc(subsorted, (newsize)*sizeof(int));
                MPI_Recv(subsorted+(subLength*2)+adjuster, subLength, MPI_INT, (subCount-1), 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                int* result = calloc((newsize), sizeof(int));
                mergeSort(subsorted, subLength*2+adjuster, subLength, result);
                free (subsorted);
                subsorted = result;


                adjuster+=subLength;
            }
            
        }
        subLength = subLength*2;
        subCount = subCount/2;
    }


    // Finalize the MPI environment.
    free(subsorted);
    MPI_Finalize();

    return 0;

}