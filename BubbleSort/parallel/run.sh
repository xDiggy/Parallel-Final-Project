#!/bin/sh
#salloc -N 1 --partition=el8-rpi --gres=gpu:4 -t 30
#module load xl_r spectrum-mpi cuda/11.2
clear
rm output.txt
nvcc -g -G bubble.cu -c -o bubblecu
mpicc bubble.c bubblecu -o bubbleout  -L/usr/local/cuda-11.2/lib64/ -lcudadevrt -lcudart -lstdc++
# mpiexec bubbleout
mpiexec -np $1 bubbleout > outfile.txt