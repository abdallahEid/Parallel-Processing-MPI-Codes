#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc , char* argv[]) {
	int rank , numberOfProcesses , i , blockSize , dest , source , tag=0 , maximum , tempMaximum , index , position;	
	MPI_Status status;
	int* arr;

	MPI_Init(&argc , &argv);
	MPI_Comm_rank(MPI_COMM_WORLD , &rank) ;
	MPI_Comm_size(MPI_COMM_WORLD , &numberOfProcesses) ;
	if(rank == 0){
		int arraySize ;
		printf("Hello from Master Process.\nNumber of Slave Processes is %d\nEnter Array Size: " , numberOfProcesses-1) ;
		if(scanf("%d\n" , &arraySize) != 1)
			printf("Expected 1 number!") ;
		int tempArraySize = arraySize ;
		arr = (int*) malloc(arraySize * sizeof(int)) ;
		for(i=0 ; i < arraySize ; i++)
			if(scanf("%d" , &arr[i]) != 1)
				printf("Expected 1 number!") ;
		// handeling size of array to be divisble by number of processes 
		if(arraySize % (numberOfProcesses-1) != 0)
			arraySize = arraySize - (arraySize % (numberOfProcesses-1)) + (numberOfProcesses-1) ;
		arr = (int*) realloc(arr , arraySize * sizeof(int)) ;
		for(i=tempArraySize ; i < arraySize ; i++)
			arr[i] = -999999999 ;
		
		blockSize = arraySize / (numberOfProcesses-1) ;
		for(dest=1 ; dest < numberOfProcesses ; dest++){
			MPI_Send(&blockSize , 1 , MPI_INT , dest , tag , MPI_COMM_WORLD) ;
			MPI_Send(arr + ((dest-1) * blockSize) , blockSize , MPI_INT , dest , tag , MPI_COMM_WORLD) ;		
		}	
		maximum = -999999999;
		for(source=1 ; source < numberOfProcesses ; source++){
			MPI_Recv(&tempMaximum , 1 , MPI_INT , source , tag , MPI_COMM_WORLD , &status);
			MPI_Recv(&position , 1 , MPI_INT , source , tag , MPI_COMM_WORLD , &status);
			if(maximum < tempMaximum){
				maximum = tempMaximum ;	
				index = blockSize * (source - 1) + position +1;
			}		
		}	
		printf("Master process announce the final max which is %d and its index is %d.\nThanks for using our program\n" , maximum , index) ;
		
	}
	else{	
		MPI_Recv(&blockSize , 1 , MPI_INT , 0 , tag , MPI_COMM_WORLD , &status);
		int* tempArr = (int*) malloc(blockSize * sizeof(int)) ;
		MPI_Recv(tempArr , blockSize , MPI_INT , 0 , tag , MPI_COMM_WORLD , &status);
		maximum = -999999999 ;
		for(i=0 ; i < blockSize ; i++){
			if(maximum < tempArr[i]){
				maximum = tempArr[i] ;
				index = i;
			}
		}
		if(maximum == -999999999){	
			maximum = 0 ;
			index = 0 ;
		}
		printf("Hello from Slave#%d max number in my partition is %d and index is %d .\n" , rank , maximum , index) ;
		MPI_Send(&maximum , 1 , MPI_INT , 0 , tag , MPI_COMM_WORLD);
		MPI_Send(&index , 1 , MPI_INT , 0 , tag , MPI_COMM_WORLD);	
	}
	MPI_Finalize() ;
	return 0 ;
}
