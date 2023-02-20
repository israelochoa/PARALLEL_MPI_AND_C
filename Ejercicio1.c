#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// tamaño de los datos
int n = 10000000;

int main(int argc, char* argv[])
{
    int i = 0;
    double count = 0;
    double res = 0;
    double a = 0;
    double b = 0;
    int pid, np,
        elements_per_process,
        n_elements_recieved;
    // np -> no. of processes
    // pid -> process id 
  
    MPI_Status status;
  
    // Creation of parallel processes
    MPI_Init(&argc, &argv);
  
    // Encuentra el ID process 
   
    MPI_Comm_rank(MPI_COMM_WORLD, &pid);

//El número de procesadores 4>> mpirun -np 4 
    MPI_Comm_size(MPI_COMM_WORLD, &np);
  
    // master process
    if (pid == 0) {
        int index, i;
	//reparte la data entre el número de procesadores
        elements_per_process = n / np;;
        printf("elements_per_process= %d\n",elements_per_process);  
       
 // Verifica si esque hay más de un procesador en ejecución
        if (np > 1) {
            // distributes the tamano de los datos
            printf("np= %d\n",np);
            for (i = 1; i < np; i++) {  
                //MPI_Send(//void* data,
                    //int count,
                    //MPI_Datatype datatype,
                    //int destination,
                    //int tag,
                    // communicator)
                MPI_Send(&elements_per_process,
                         1, MPI_INT, i, 0,
                         MPI_COMM_WORLD);
               
            }
  
            
        }
        double tmp;//almacena la suma parcial de los nodos
        // Aqui suma la porción de datos el proceso maestro
        while(i < elements_per_process) {
            a =((double)rand() / RAND_MAX) * 100;
            b = ((double)rand() / RAND_MAX) * 100;
            if ((a * a) + (b * b) < 1) {
                tmp=count++;
            }
            i++;
        }
        printf("Processor %d response = %f\n",np,tmp);
        // se recolecta las sumas parciales de los otros procesadores
        
        for (i = 1; i < np; i++) {
            // MPI_Recv(// void* data,
                        //  int count,
                        //  MPI_Datatype datatype,
                        //  int source,
                        //  int tag,
           //  MPI_Comm communicator,
            // MPI_Status* status)
            MPI_Recv(&tmp, 1, MPI_DOUBLE,
                     MPI_ANY_SOURCE, 0,
                     MPI_COMM_WORLD,
                     &status);
            int sender = status.MPI_SOURCE;
            printf("Processor %d response = %f\n",i,tmp);
            tmp += count;
        }
        count=tmp;
        res = 4 * (count / n);
         printf("Solucion = %f\n",res);
              
    }else {
        MPI_Recv(&n_elements_recieved,
                 1, MPI_INT, 0, 0,
                 MPI_COMM_WORLD,
                 &status);     
  
        // Los procesadores esclavos calcula su suma parcial y envían las respuestas en el método send
        double partial_sum = 0;
        while(i < n_elements_recieved) {
            a =((double)rand() / RAND_MAX) * 100;
            b = ((double)rand() / RAND_MAX) * 100;
            if ((a * a) + (b * b) < 1) {
                partial_sum++;
            }
            i++;
        }

        // envían la suma parcial al proceso maestro
        MPI_Send(&partial_sum, 1, MPI_DOUBLE,
                 0, 0, MPI_COMM_WORLD);
    }
  
    // Limpia y sale
    MPI_Finalize();
  
    return 0;
}
