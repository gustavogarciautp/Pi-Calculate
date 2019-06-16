#include <iostream>
#include <random>
#include <chrono>
#include <time.h>
#include <omp.h>

using namespace std;

int main(int argc, char *argv[]){
    int no_of_points = atoi(argv[1]);
    float inside, total_inside=0.0;
    int taskid,         /* task ID - also used as seed number */
    numtasks,       /* number of tasks */
    cant,
    i=0;

    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0.0, 1.0);
    float x,y,z=0.0;

    auto start = std::chrono::system_clock::now();

    #pragma omp parallel private(x,y,z,taskid, cant, inside,i) shared(numtasks, no_of_points, total_inside) 
    {

    numtasks = omp_get_num_threads();
    //printf("tasks %d\n", numtasks);
    taskid = omp_get_thread_num();
    //printf("taskid  %d\n", taskid);

    cant= no_of_points /numtasks;

    if (taskid == numtasks -1 )
        cant = no_of_points - taskid*cant;

    //printf("cant %d\n", cant);
        
    for(i=0; i<cant; i++){
        x= dis(gen);
        //printf("%f\n", x);
        y= dis(gen);

        //Calculate the radius
        z= x*x +y*y;

        if (z <= 1){
            ++inside;
        }
    }
    //printf("i %d\n", i);
    //printf("inside %f\n", inside);
    #pragma omp atomic update
    total_inside+= inside;
    }

    //printf("total_inside %f\n", total_inside);
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<float,std::ratio<1>> duration = end - start;

    std::cout << duration.count() << std::endl;

    //Calculate the estimated value of Pi
    float Pi = 4.0 * total_inside / no_of_points;
    printf("The value of Pi is estimated to be %.11f using %d points\n", Pi, no_of_points);

}