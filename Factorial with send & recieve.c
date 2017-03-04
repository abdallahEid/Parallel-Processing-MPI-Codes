
#include "mpi.h"

int main(int argc , char* argv[]){
	int rank , numberOfProcesses , number , source , i , dest , tag = 0 , subRange , factorial , subFactorial;
	MPI_Status status;

	MPI_Init(&argc , &argv);
	MPI_Comm_rank(MPI_COMM_WORLD , &rank) ;
	MPI_Comm_size(MPI_COMM_WORLD , &numberOfProcesses) ;
	if(rank == 0){
		if(scanf("%d" , &number) != 1)
			printf("Expected 1 number!") ;
		subRange = number / (numberOfProcesses-1) ;
		for(dest=1 ; dest < numberOfProcesses ; dest++){
			MPI_Send(&number , 1 , MPI_INT , dest , tag , MPI_COMM_WORLD) ;
			MPI_Send(&subRange , 1 , MPI_INT , dest , tag , MPI_COMM_WORLD) ;
		}
		factorial = 1 ;
		for(source = 1; source < numberOfProcesses; source++){
			MPI_Recv(&subFactorial , 1 , MPI_INT , source , tag , MPI_COMM_WORLD , &status);
			factorial *= subFactorial;
		}
		printf("%d\n", factorial);

	}
	else{
		source = 0 ;
		MPI_Recv(&number , 1 , MPI_INT , source , tag , MPI_COMM_WORLD , &status) ;
		MPI_Recv(&subRange , 1 , MPI_INT , source , tag , MPI_COMM_WORLD , &status) ;
		int subFactorial = 1 ;
		for(source= subRange*(rank-1)+1 ; source <= rank*subRange ; source++)
			subFactorial *= source ;
		MPI_Send(&subFactorial , 1 , MPI_INT , 0 , tag , MPI_COMM_WORLD);
	}
	MPI_Finalize();
}



