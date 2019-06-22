#include "mpi.h"
#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

using namespace std;

float compute(int cant){
  random_device rd;
  mt19937 gen(rd());
  uniform_real_distribution<> dis(0.0, 1.0);
  float x, y,z, inside=0.0; 
  int i=0;

  /* "throw darts at board" */
  for (i = 0; i < cant; i++)  {
    /* generate random numbers for x and y coordinates */
    x = dis(gen);
    y = dis(gen);
    //printf("x %f\n", x);
    //printf("y %f\n", y);
    z= x*x + y*y;
    /* if dart lands in circle, increment inside */
    if (z <= 1.0)
         inside=inside+1.0;
    }
    return inside;
} 

#define MASTER 0        /* task ID of master task */

int main (int argc, char *argv[]){
char *fname;
  int	no_of_points = atoi(argv[1]);        /* average pi value for all iterations */
  fname = (argc > 2) ? argv[2]: NULL;
  float inside, total_inside=0.0;
  int	taskid,	        /* task ID - also used as seed number */
  	numtasks,       /* number of tasks */
  	rc;             /* return code */

  MPI_Status status;
   double t1, t2,tf;
 // auto start = std::chrono::system_clock::now();
   t1=MPI_Wtime();
  /* Obtain number of tasks and task ID */
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
 // printf ("MPI task %d has started...\n", taskid);

  int cant = no_of_points/numtasks;
  if (taskid == numtasks-1)
    cant = no_of_points- taskid*cant;
  
  //printf("taskid %d cant %d\n", taskid, cant);
  inside = compute(cant);

  //printf("Local inside for process %d: %f\n", taskid, inside);

  rc = MPI_Reduce(&inside, &total_inside, 1, MPI_FLOAT, MPI_SUM,
                     MASTER, MPI_COMM_WORLD);

  MPI_Finalize();  
  if (taskid== MASTER){
    //printf("total %f\n", total_inside);
   // auto end = std::chrono::system_clock::now();
    t2=MPI_Wtime();
    tf=t2-t1;  
 // std::chrono::duration<float,std::ratio<1>> duration = end - start;
   // std::cout << duration.count() << std::endl;
    //printf("Total inside for all process %f\n", total_inside);
    float Pi = 4.0 * total_inside/ no_of_points;
   // printf("The value of Pi is estimated to be %.11f using %d points\n", Pi, no_of_points);  
   FILE* fp=fopen(fname, "a");
   fprintf(fp, "%g %g\n", tf,Pi);
   fclose(fp);
}
  return 0;
}

