#include<stdio.h>
#include<cuda.h>
#include<cuda_runtime.h>

extern "C" 
{
    void mergeCuda(int* array, int startindex, int endindex, int threadCount);
}



__global__ void mergeKernel(int* subarray, int len, int sizetosort){
    unsigned int index = blockIdx.x *blockDim.x + threadIdx.x;
    int left = index*sizetosort;
    int right = left+sizetosort-1;
    while (left < len){
        // sizetosort is how many indices each thread is responsible for
        // size of left/right
        int lsize = sizetosort/2;
        int rsize = lsize;

        
        // overspill correction
        if (right >= len){
            right = len-1;
            int sublen = right-left+1;
            rsize = sublen-lsize;
            if (rsize < 0){
                rsize = 0;
            }
        }

        if (rsize != 0){
            printf("Sorting [%d,%d]\n",left,right);
            // sort
            int l = 0;
            int r = 0;
            while(l < lsize && r < rsize ){
                int lindex = l+left;
                int rindex = r+right;
                int leftV = subarray[lindex];
                int rightV = subarray[rindex];
                if (rightV < leftV){
                    subarray[lindex] = rightV;
                    subarray[rindex] = leftV;
                    r++;
                }
                else if(leftV < rightV){
                    l++;
                }
            }
        }
        index+=blockDim.x;
        left = index*sizetosort;
        right = left+sizetosort-1;
    }
    
}


void mergeCuda(int* array, int startindex, int endindex, int threadCount){
    // create the subarray
    int len = (endindex - startindex) + 1;
    int * subarray;
    cudaMallocManaged(&subarray, len*sizeof(int));

    
    for (int i = 0; i < len; i++){
        subarray[i] = array[startindex+i];
    }

    printf("Original array: [");
    for (int i = 0; i < len-1; i++){
        printf("%d ",subarray[i]);
    }
    printf("%d]\n",subarray[len-1]);

    // kernel launch with this subarray    
    // sort every 2
    // sort every 4
    // sort every 8
    // ...

    int iteration = 1;
    int sizetosort = 0;
    while (sizetosort < len){
        sizetosort = 1 << iteration;
        int adjustedthreads = len/sizetosort;
        if (len%sizetosort > 0){
            adjustedthreads++;
        }
        if (adjustedthreads > threadCount){
            adjustedthreads = threadCount;
        }
        printf("Using %d threads\n", adjustedthreads);
        mergeKernel<<<1,adjustedthreads>>>(subarray, len, sizetosort);
        // finish this subsize, then loop
        cudaDeviceSynchronize();
        printf("Resulting array: [");
        for (int i = 0; i < len-1; i++){
            printf("%d ",subarray[i]);
        }
        printf("%d]\n",subarray[len-1]);
        iteration++;
    }
    cudaFree(subarray);
}