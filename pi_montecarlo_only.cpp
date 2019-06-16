#include "mpi.h"
#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <chrono>

using namespace std;

#define PI 3.14159265
#define MASTER 0        /* task ID of master task */

bool check_intersection(float middle, int d,int l, float theta){
  if((middle + (l/2.0) * sin(theta)) >= d)
    return true;
  else{
    if ((middle - (l/2.0)*sin(theta) <= 0))
      return true;
    else
      return false;
    }
} 

int main (int argc, char *argv[]){
  float num_overlaps, total_overlaps=0.0;
  int num_needles= (argc > 1) ? atoi(argv[1]) : 2500; //Numero de agujas
  int d =  (argc > 2) ? atoi(argv[2]) : 30; //Distancia entre las lineas
  int l =  (argc > 3) ? atoi(argv[3]) : 10; //Longitud de la aguja
  float middle, theta;

  random_device rd;
  mt19937 gen(rd());
  uniform_real_distribution<> dis(0.0,1.0);        /* average pi value for all iterations */
  
  int	taskid,	        /* task ID - also used as seed number */
  	numtasks,       /* number of tasks */
  	rc,             /* return code */
    j;
  MPI_Status status;

  auto start = std::chrono::system_clock::now();

  /* Obtain number of tasks and task ID */
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numtasks);
  MPI_Comm_rank(MPI_COMM_WORLD,&taskid);
  printf ("MPI task %d has started...\n", taskid);

  int cant = num_needles/numtasks;
  
  if (taskid == numtasks-1)
    cant = num_needles- taskid*cant;
  
  //printf("cant %d\n", cant);
  for(j=0; j< cant; j++){
    middle = d*dis(gen);
    //printf("middle %f\n", middle);
    theta = PI*dis(gen);
    //printf("theta %f\n", theta);
    if (check_intersection(middle,d,l, theta)){
      num_overlaps+=1.0;
    }
  }

  rc = MPI_Reduce(&num_overlaps, &total_overlaps, 1, MPI_FLOAT, MPI_SUM,
                     MASTER, MPI_COMM_WORLD);

  MPI_Finalize(); 

  if (taskid== MASTER){
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float,std::ratio<1>> duration = end - start;
    std::cout << duration.count() << std::endl;

    float result = (2*num_needles*l)/(d*total_overlaps);
    printf("The value of Pi is estimated to be %.11f using %d needles\n", result, num_needles);  
  }
  return 0;
}
