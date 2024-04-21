#!/bin/sh

clear

$HOME/opt/usr/local/bin/mpicc bubble.c -o bubble
#./executable <pattern number> <grid size> <number of generations>
$HOME/opt/usr/local/bin/mpirun -np $1 ./bubble