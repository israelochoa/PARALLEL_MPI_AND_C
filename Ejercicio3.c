
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

//tamaño de los datos
static const int number_of_elements = 20000;
static const int elementsRow=20;
static const int linMin=number_of_elements/elementsRow;
//Functionpara cambiar dos numeros
void swap(int* arr, int i, int j)
{
	int t = arr[i];
	arr[i] = arr[j];
	arr[j] = t;
}

// Funcion quicksort
void quicksort(int* arr, int start, int end)
{
	int pivot, index;

	// Base Case
	if (end <= 1)
		return;

	// Selecciona un pivote y lo intercambia con el primet elemento
	
	pivot = arr[start + end / 2];
	swap(arr, start, start + end / 2);

	
	index = start;

	// Iterate over the range [start, end]
	for (int i = start + 1; i < start + end; i++) {

		// Swap if the element is less
		// than the pivot element
		if (arr[i] < pivot) {
			index++;
			swap(arr, i, index);
		}
	}

	// Swap the pivot into place
	swap(arr, start, index);

	// Recursive Call for sorting
	// of quick sort function
	quicksort(arr, start, index - start);
	quicksort(arr, index + 1, start + end - index - 1);
}

// Funcion que mezcla dos arrays
int* merge(int* arr1, int n1, int* arr2, int n2)
{
	int* result = (int*)malloc((n1 + n2) * sizeof(int));
	int i = 0;
	int j = 0;
	int k;

	for (k = 0; k < n1 + n2; k++) {
		if (i >= n1) {
			result[k] = arr2[j];
			j++;
		}
		else if (j >= n2) {
			result[k] = arr1[i];
			i++;
		}

		
		else if (arr1[i] < arr2[j]) {
			result[k] = arr1[i];
			i++;
		}

		// v2[j] <= v1[i]
		else {
			result[k] = arr2[j];
			j++;
		}
	}
	return result;
}
int sumPartial[1];



// Driver Code
int main(int argc, char* argv[])
{
    
	
	int* data = NULL;
	int chunk_size, own_chunk_size;
	int* chunk;
	FILE* file = NULL;
	double time_taken;
	MPI_Status status;

	
	int number_of_process, rank_of_process;
	int rc = MPI_Init(&argc, &argv);

	if (rc != MPI_SUCCESS) {
		printf("Error in creating MPI "
			"program.\n "
			"Terminating......\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
	}
MPI_File fh;
	MPI_Comm_size(MPI_COMM_WORLD, &number_of_process);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank_of_process);
chunk_size
			= (number_of_elements % number_of_process == 0)
				? (number_of_elements / number_of_process)
				: (number_of_elements / number_of_process
					- 1);

	if (rank_of_process == 0) {
		// Opening the file
        data= (int*)malloc(number_of_elements*sizeof(int));
        printf("SIZE OF DATA %ld", sizeof(data));
				for (int i = 0; i < number_of_elements; i++)
                {
                    /* code */
                    
int num = rand() % number_of_elements + 1;
                    data[i]=num;
                    
                }
                			
      


		// Printing the array read from file
		printf("Elements in the array is : \n");
		for (int i = 0; i < number_of_elements; i++) {
			printf("%d ", data[i]);
		}
 
		printf("\n");

		
		
	}

	// Blocks all process until reach this point
	MPI_Barrier(MPI_COMM_WORLD);

	// Starts Timer
	time_taken -= MPI_Wtime();

	// BroadCast the Size to all the
	// process from root process
	

	// Computing chunk size
	chunk_size
		= (number_of_elements % number_of_process == 0)
			? (number_of_elements / number_of_process)
			: number_of_elements
					/ (number_of_process - 1);

	// Calculating total size of chunk
	// according to bits
	chunk = (int*)malloc(chunk_size * sizeof(int));

	//Distribuye la data a todos los procesos
	MPI_Scatter(data, chunk_size, MPI_INT, chunk,
				chunk_size, MPI_INT, 0, MPI_COMM_WORLD);
	free(data);
    int smp;
    for (int i = 0; i < chunk_size; i++)
{
    /* code */

    //printf("SUMPARTIALSUM()==>rank %d ==>sumPartial = data[%d]: %d + sumPartial[%d]: %d === %d\n",rank_of_process,i,chunk[i],i,smp,smp+chunk[i]);       
    smp+=chunk[i];
    
}
    
    
data = NULL;

int* data2=NULL;
if (rank_of_process==0)
{
    data2=(int *)malloc(number_of_process*sizeof(int));
}
     MPI_Gather(&smp, 1, MPI_INT, data2, 1, MPI_INT, 0,
           MPI_COMM_WORLD);  

	

	// Compute size of own chunk and
	// then sort them
	// using quick sort

	own_chunk_size = (number_of_elements
					>= chunk_size * (rank_of_process + 1))
						? chunk_size
						: (number_of_elements
							- chunk_size * rank_of_process);

	// Sorting array with quick sort for every
	// chunk as called by 
    
	quicksort(chunk, 0, own_chunk_size);
    
    
    
    
MPI_Barrier(MPI_COMM_WORLD);
	for (int step = 1; step < number_of_process;
		step = 2 * step) {
		if (rank_of_process % (2 * step) != 0) {
			MPI_Send(chunk, own_chunk_size, MPI_INT,
					rank_of_process - step, 0,
					MPI_COMM_WORLD);
			break;
		}

		if (rank_of_process + step < number_of_process) {
			int received_chunk_size
				= (number_of_elements
				>= chunk_size
						* (rank_of_process + 2 * step))
					? (chunk_size * step)
					: (number_of_elements
						- chunk_size
							* (rank_of_process + step));
			int* chunk_received;
			chunk_received = (int*)malloc(
				received_chunk_size * sizeof(int));
			MPI_Recv(chunk_received, received_chunk_size,
					MPI_INT, rank_of_process + step, 0,
					MPI_COMM_WORLD, &status);

			data = merge(chunk, own_chunk_size,
						chunk_received,
						received_chunk_size);

			free(chunk);
			free(chunk_received);
			chunk = data;
			own_chunk_size
				= own_chunk_size + received_chunk_size;
		}
	}

	// Stop the timer
	time_taken += MPI_Wtime();

	// Opening the other file as taken form input
	// and writing it to the file and giving it
	// as the output
	if (rank_of_process == 0) {
		// Opening the for
        int total=0;
        for (int i = 0; i < number_of_process ;i++)
        {
            /* code */
          //  printf("SUMPARTIALSUM()==>rank %d ==>total = sumPartial %d + totalInit: %d\n",rank_of_process,data2[i],total);    
            total+=data2[i];
            
            printf("TOTAL: %d\n",total)   ;
            printf("====================FIN TOTAL=====================================\n");
        }
        MPI_File_open(MPI_COMM_SELF, "DocEjer3.txt",MPI_MODE_CREATE | MPI_MODE_WRONLY,MPI_INFO_NULL,&fh);
        char buf[42];
        int bufint[42];
        snprintf(buf,42,"TOTAL DE LA SUMA DE ELEMENTOS: %d\n",total);
        MPI_File_write(fh,buf,strlen(buf), MPI_CHAR,&status);
        snprintf(buf,42,"QUICK SORT");
        MPI_File_write(fh,buf,strlen(buf), MPI_CHAR,&status);
  for (int i=0; i < linMin; i++){
                
                snprintf(buf,42,"%s","\n");
                MPI_File_write(fh,buf,strlen(buf), MPI_CHAR,&status);
            //fprintf(f,"%d \n",i);
            for(int j=0; j<elementsRow; j++){
                
                snprintf(buf,42,"%d \t",data[(i+1)*j]);
                //printf("This is buf: %s",buf);
                //sMPI_File_write(MPI_File fh, const void *buf,
                                //int count, MPI_Datatype datatype,
                                //MPI_Status *status)
                MPI_File_write(fh,buf,strlen(buf), MPI_CHAR,&status);
            }
            
            
        }
        //        fclose(f);
        MPI_File_close(&fh);

		printf("QUICK SORT: \n");

		for (int i = 0; i < number_of_elements; i++) {
			printf("%d ", chunk[i]);
		}


		printf("\n\n\n\nRESULTADO ALMACENADO EN DocEjer3.txt\n");

		// For Printing in the terminal
		printf("Total number of Elements ingresados : "
			"%d\n",
			number_of_elements);
            printf("======== Total sum of elements: %d ============== \n",total);
		
		printf(
			"\nTime of Algoritmh Quicksort: %f secs, with %d elements and %d processors\n",time_taken,number_of_elements, number_of_process);
	}

	MPI_Finalize();
	return 0;
}
