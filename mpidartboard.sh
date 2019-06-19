#!/bin/bash
for i in {1..10}
do
	for j in {300000, 500000, 1000000, 1500000, 2000000, 2500000, 3000000, 3500000, 4000000, 4500000, 5000000}
	do
		mpirun -hostfile hosts --prefix /home/andressone9/mpi -np 3 /home/andressone9/hpc2/Pi-Calculate/dartboardmpi $j dartmpi.csv
	done
done