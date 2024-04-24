Antonio Martinez III
Ilias Soumayah
Josh Mozes
Reza Malik

## newgen.c
Generates a new c file with an array filled with random ints

gcc newgen.c -o newgen

./newgen 100


## bubble sort testing
salloc -N 1 --partition=el8-rpi --gres=gpu:1 -t 30

module load xl_r spectrum-mpi cuda/11.2

clear

rm output.txt

nvcc -g -G bubble.cu -c -o bubblecu


mpicc bubble.c bubblecu -o bubbleout  -L/usr/local/cuda-11.2/lib64/ -lcudadevrt -lcudart -lstdc++

mpiexec -np $1 bubbleout > outfile.txt

## merge sort testing
salloc -N 1 --partition=el8-rpi --gres=gpu:1 -t 30

module load xl_r spectrum-mpi cuda/11.2

nvcc -g -G merge.cu -c -o mergecu

mpicc merge.c mergecu -o mergeout  -L/usr/local/cuda-11.2/lib64/ -lcudadevrt -lcudart -lstdc++

mpiexec mergeout randlist.txt 100
