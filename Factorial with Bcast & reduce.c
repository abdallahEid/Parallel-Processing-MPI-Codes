#include "mpi.h"

int main(int argc , char* argv[])
{
	int rank , numberOfProcesses , number , subRange , i , subFactorial=1 , factorial=1;

	MPI_Init(&argc , &argv);
	MPI_Comm_rank(MPI_COMM_WORLD , &rank);
	MPI_Comm_size(MPI_COMM_WORLD , &numberOfProcesses);
	if(rank == 0){
		if(scanf("%d" , &number) != 1)
			printf("Expected 1 number!") ;
		subRange = number / (numberOfProcesses-1) ;
	}
	MPI_Bcast(&number , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
	MPI_Bcast(&subRange , 1 , MPI_INT , 0 , MPI_COMM_WORLD);
	if(rank){
		for(i=subRange*(rank-1)+1; i <= subRange*rank; i++)
			subFactorial *= i;
	}
	MPI_Reduce(&subFactorial , &factorial , 1 , MPI_INT , MPI_PROD , 0 , MPI_COMM_WORLD);
	if(rank == 0)
		printf("%d\n" , factorial);

	MPI_Finalize();

}
