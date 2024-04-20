#include<stdio.h>
#include<cuda.h>
#include<cuda_runtime.h>

extern "C" 
{
    void mergeCuda(int* array, int startindex, int endindex, int threadCount);
}



__global__ void mergeKernel(int* subarray,int* subarray2, int len, int sizetosort){
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
            int added = 0;
            //printf("Sorting [%d,%d]\n",left,right);
            // sort
            int l = 0;
            int r = 0;
            while(l < lsize && r < rsize ){
                int lindex = l+left;
                int rindex = left+lsize+r;
                int leftV = subarray[lindex];
                int rightV = subarray[rindex];
                if (rightV == leftV){
                    subarray2[left+l+r]=rightV;
                    r++;
                    added++;
                    subarray2[left+l+r]=leftV;
                    l++;
                    added++;
                }
                if (rightV < leftV){
                    subarray2[left+l+r]=rightV;
                    //printf("1 placing %d at %d\n",rightV, left+l+r);
                    r++;
                    added++;
                }
                else if(leftV < rightV){
                    subarray2[left+l+r] = leftV;
                    //printf("2 placing %d at %d\n",leftV, left+l+r);
                    added++;
                    l++;
                }
            }
            if (l < lsize && r >= rsize){
                // add the rest of L to subarray2
                while (l < lsize){
                    int idex = left+added;
                    int lindex = l+left;
                    int leftV = subarray[lindex];
                    subarray2[idex] = leftV;
                    //printf("3 placing %d at %d\n",leftV, idex);
                    l++;
                    added++;
                } 
            }
            if(r < rsize && l >= lsize){
                // add the rest of R to subarray2
                while (r < rsize){
                    int idex = left+added;
                    int rindex = left+lsize+r;
                    int rightV = subarray[rindex];
                    subarray2[idex] = rightV;
                   // printf("4 placing %d at %d\n",rightV, idex);
                    r++;
                    added++;
                } 
            }
            // case for when things are equal!
        }
        else if (lsize > 0 && rsize == 0){
           // printf("Sorting [%d,%d]\n",left,right);
            for (int i = 0; i < lsize; i++){
                //printf("5 placing %d at %d\n",subarray[left+i], left+i);
                subarray2[left+i] = subarray[left+i];
            }
        }
        // there shouldn't be a case where left is 0 and right > 0 since we fill the left side first 
        index+=blockDim.x;
        left = index*sizetosort;
        right = left+sizetosort-1;
    }
    
}


void mergeCuda(int* array, int startindex, int endindex, int threadCount){
    // create the subarray
    int len = (endindex - startindex) + 1;
    int * subarray;
    int * subarray2;
    cudaMallocManaged(&subarray, len*sizeof(int));
    cudaMallocManaged(&subarray2, len*sizeof(int));

    
    for (int i = 0; i < len; i++){
        subarray[i] = array[startindex+i];
        subarray2[i] = array[startindex+i];
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
        mergeKernel<<<1,adjustedthreads>>>(subarray,subarray2, len, sizetosort);
        cudaDeviceSynchronize();

        // finish this subsize, copy, then loop
        cudaMemcpy(subarray, subarray2, len * sizeof(int), cudaMemcpyDeviceToDevice);

        for (int i = 0; i < len; i++){
            subarray2[i] = -1;
        }
        iteration++;        
    }
    printf("Resulting array: [");
    for (int i = 0; i < len-1; i++){
        printf("%d ",subarray[i]);
    }
    printf("%d]\n",subarray[len-1]);
    
    cudaFree(subarray);
    cudaFree(subarray2);
}