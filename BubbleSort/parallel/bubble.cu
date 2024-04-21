#include<stdio.h>
#include<cuda.h>
#include<cuda_runtime.h>

extern "C"
{
    void bubbleCuda(int* array, int len);
}

__global__ void bubbleKernel(int* array, int len){

    int threadID = threadIdx.x;

    for(int i = 0; i < len; i++){

        int offset = i % 2;
        int index = threadID * 2 + offset;

        if(index < len - 1){
            if(array[index] > array[index + 1]){
                int temp = array[index];
                array[index] = array[index + 1];
                array[index + 1] = temp;
            }
        }
        __syncthreads();
    }

}

void bubbleCuda(int* array, int len){

    int *d_array;
    cudaMalloc((void**)&d_array, len * sizeof(int));
    cudaMemcpy(d_array, array, len * sizeof(int), cudaMemcpyHostToDevice);

    bubbleKernel<<<1, len/2>>>(d_array, len);
    cudaDeviceSynchronize();

    cudaMemcpy(array, d_array, len * sizeof(int), cudaMemcpyDeviceToHost);
    cudaFree(d_array);

}