#include <iostream>
#include <random>
#include <math.h>
#include <chrono>
#include <omp.h>

using namespace std;

#define PI 3.14159265

bool check_intersection(float middle, int d,int l, float theta){
	//printf("middle_f %.4f\n", middle);
	//printf("theta_f %.4f \n", theta);
	if((middle + (l/2.0) * sin(theta)) >= d)
		return true;
	else{
		if ((middle - (l/2.0)*sin(theta) <= 0))
			return true;
		else
			return false;
		}
}

int main(int argc, char *argv[]){
    float num_overlaps, total_overlaps=0.0;
	int num_needles= (argc > 1) ? atoi(argv[1]) : 2500; //Numero de agujas
	int d =  (argc > 2) ? atoi(argv[2]) : 30; //Distancia entre las lineas
	int l =  (argc > 3) ? atoi(argv[3]) : 10; //Longitud de la aguja
    float middle, theta;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0,1.0);

    int  taskid,         /* task ID - also used as seed number */
    numtasks,       /* number of tasks */
    j,
    cant;

    auto start = std::chrono::system_clock::now();
	
    #pragma omp parallel private(num_overlaps, middle, cant, theta, taskid, j) shared(num_needles, numtasks, d,l, total_overlaps) 
    {
        numtasks = omp_get_num_threads();
        //printf("tasks %d\n", numtasks);
        taskid = omp_get_thread_num();
        //printf("taskid  %d\n", taskid);

        cant= num_needles/numtasks;

        if (taskid == numtasks -1 )
            cant = num_needles - taskid*cant;
    
    printf("cant %d\n", cant);
      for(j=0; j< cant; j++){
        middle = d*dis(gen);
        //printf("middle %f\n", middle);
        theta = PI*dis(gen);
        //printf("theta %f\n", theta);
        if (check_intersection(middle,d,l, theta)){
          num_overlaps+=1.0;
        }
      }
      //printf("process %d %d\n",taskid, j);
      //printf("num_overlaps %f\n", num_overlaps);
      #pragma omp atomic update
      total_overlaps+=num_overlaps;
    }

	//printf("total_overlaps %f\n", total_overlaps);
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<float,std::ratio<1>> duration = end - start;
	std::cout << duration.count() << std::endl;
	float result = (2*num_needles*l)/(d*total_overlaps);
	printf("The value of Pi is estimated to be %.11f using %d needles\n", result, num_needles);  

}