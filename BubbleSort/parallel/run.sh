#!/bin/sh

clear

$HOME/opt/usr/local/bin/mpicc mpi.c -o mpi
#./executable <pattern number> <grid size> <number of generations>
$HOME/opt/usr/local/bin/mpirun -np $1 ./mpi