#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <ctype.h>
#include <time.h>
//#include <clockcycle.h>

extern void mergeCuda(int* array, int len, int threadCount, int* result);

// salloc -N 1 --partition=el8-rpi --gres=gpu:4 -t 30
// module load xl_r spectrum-mpi cuda/11.2
// nvcc -g -G merge.cu -c -o mergecu
// mpicc merge.c mergecu -o mergeout  -L/usr/local/cuda-11.2/lib64/ -lcudadevrt -lcudart -lstdc++
// mpiexec mergeout randlist.txt 1000
//

// uint64_t clock_now(void)
// {
//   unsigned int tbl, tbu0, tbu1;
  
//   do {
//     __asm__ __volatile__ ("mftbu %0" : "=r"(tbu0));
//     __asm__ __volatile__ ("mftb %0" : "=r"(tbl));
//     __asm__ __volatile__ ("mftbu %0" : "=r"(tbu1));
//   } while (tbu0 != tbu1);
//   return (((uint64_t)tbu0) << 32) | tbl;
// }

void generate_random_array(const char* filename, int size) {
    // Open the file for writing
    FILE* file = fopen(filename, "w");
    if (file == NULL) {
        printf("Error opening file!\n");
        return;
    }

    // Seed the random number generator
    srand(time(NULL));

    // Fill the array with random integers
    for (int i = 0; i < size; i++) {
        int x = rand() % 100;// Adjust the range of random numbers as needed
        if (x > 9){
            fprintf(file, "%d", x); 
        }
        else{
            fprintf(file, " %d", x); 
        }
        
    }

    // Close the file
    fclose(file);
}


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
    
    if (argc < 3) {
        printf("Wrong Number of Arguments Provided. Usage: %s filename arraysize\n", argv[0]);
        MPI_Finalize();
        return 1;
    }
    


    MPI_Init(&argc, &argv);
    MPI_Wtime();

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
    
    const char* fname = argv[1];
    int len = atoi(argv[2]);
    if (rank == 0){
        generate_random_array(fname, len);

        FILE *file = fopen("randlist.txt", "r");
        if (file == NULL) {
            fprintf(stderr, "Error opening file.\n");
            return 1;
        }

        // printf("Original List: [");
        // int num;
        // while (fscanf(file, "%2d", &num) != EOF) {
        //     printf("%d ", num);
        // }
        // printf("]\n");

        fclose(file);
    }

    

    // how much of the array does each rank deal with?
    int subLength = 1;
    int subCount = 1;
    for (int i = cores; i > 0; i--){
        if (len % i == 0 && i%2 == 0 && len/i > 1){
            subCount = i;
            subLength = len / i;
            i = 0;
        }
    }

    // COMPUTATIOn IS HERE:
    MPI_Barrier(MPI_COMM_WORLD);
    int* subsorted = malloc(subLength*sizeof(int));

    MPI_File mfile;
    MPI_File_open(MPI_COMM_WORLD, fname, MPI_MODE_RDONLY, MPI_INFO_NULL, &mfile);

    int* parsed = calloc(subLength, sizeof(int));
    int num = 0;
    double ttl_read_time = 0;
    if (rank < subCount){
        // make the subarray for this mpi rank
        MPI_Offset offset = rank*(subLength*2)*sizeof(char);
        char buffer[201];
        //u_int64_t before_read = clock_now();
        double before_read = MPI_Wtime();
        MPI_File_read_at(mfile, offset, buffer, (subLength*sizeof(char))*2, MPI_CHAR, MPI_STATUS_IGNORE);
        double after_read = MPI_Wtime();
        ttl_read_time += after_read-before_read;

        for (int i = 0; i < subLength*2; i+=2) {
            int y;
            if (isdigit(buffer[i])){
                char a[3];
                a[0] = buffer[i];
                a[1] = buffer[i+1];
                a[2] = '\0';
                y = atoi(a);
            }
            else{
                char a[2];
                a[0] = buffer[i+1];
                a[1] = '\0';
                y=atoi(a);
            }
            parsed[num] = y;
            num++;
        }


    
        // pass this to the device to do merge sort in parallel
        int threadCount = 64;
        
        mergeCuda(parsed, subLength, threadCount, subsorted);
        

        /*
        printf("Subsorted %d: [",rank);
        for (int i = 0; i < subLength-1; i++){
            printf("%d, ",subsorted[i]);   
        }
        printf("%d]\n",subsorted[subLength-1]);   
        */
        
        

    }

    MPI_File_close(&mfile);

    MPI_Barrier(MPI_COMM_WORLD);

    // after completing merge sort for this subarray, we combine the results for the final sorted array
    // ensure all cores have finished computing their subarray

    // helper variable to keep track of extra items in rank 0
    int adjuster = 0;
    while(subCount > 1 && rank < subCount){
        if (rank >= subCount/2){
            // sender
            if (rank == subCount-1 && subCount%2 == 1){
                // case for an odd amount of MPI Processes, the last rank is sent to rank 0 to account for this
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



    // Rank 0 collects the result

    // if (rank == 0){
    //     printf("Sorted List: [");
    //     for (int i = 0; i < subLength+adjuster; i++){
    //         printf("%d ",subsorted[i]);
    //     }
    //     printf("]\n");
    // }


    // Finalize the MPI environment.
    if (rank == 0){
        double end_time = MPI_Wtime();
        printf("End time %f\n",end_time);
        printf("read time: %f\n", ttl_read_time);
        
    }
    free(subsorted);
    MPI_Finalize();

    return 0;

}