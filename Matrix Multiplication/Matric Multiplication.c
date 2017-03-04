/*

	In this code 
	- i begin with taking input from process 0 from file or console 
	- i take thw first matrix and put it in 1D array 
	  (if i have 1 2
	  			 3 4 
	   it will be {1 , 2 , 3 ,4})
	- i send the whole second matrix to all processes from the root process 0 
	- i determine the number of rows that will be sent to each process and make them 1D array as before 
	  (if matrix has four rows and there is 2 processes, each process will take 2 rows but in `D array)
	- i multiply this 1D array to the second matrix and take care of rows and the result will be 1D array that carries the multiplication of the second matrix with the rows in each process 
	  (if the first matrix is   1  2  3
								4  5  6
								7  8  9
								10 11 12
				and the second one is   1 2
										3 4 
										5 6
		and i have 2 processes 
				 first process  --> 1 2 3 - 4 5 6
				 second process --> 7 8 9 - 10 11 12 )

	- i use gather to collect the result from all processes 
	- i handle the remainder by send and recieve 

*/





#include <stdio.h>
#include <stdlib.h>
#include "mpi.h"

int main(int argc , char**argv){	
	int rank , numberOfProcesses;
	int  rows1 , columns1 , rows2 , columns2 , choice , i , j;
	int** matrix2 ;
	int** matrix1 ;
	int *tempArr ;
	
	MPI_Init(&argc , &argv) ;
	MPI_Comm_rank(MPI_COMM_WORLD , &rank) ;
	MPI_Comm_size(MPI_COMM_WORLD , &numberOfProcesses) ;
	
	if(rank == 0){	
		// taking input 
		printf("Input from:\n"
			"1-File\n" 
			"2-Console\n") ;
		scanf("%d" , &choice);
	
		if(choice == 1){
			printf("Enter file name: ") ;
			char path[20] ;
			scanf("%s" , path) ;
		        FILE *file = fopen(path , "r" ) ;

		        fscanf(file , "%d" , &rows1) ;
		        fscanf(file , "%d" , &columns1) ;
		        fscanf(file , "%d" , &rows2) ;
		        fscanf(file , "%d" , &columns2) ;

			if(rows2 != columns1){
				printf("sorry, you can't multiply these matrices!\n") ;
				return 0 ;	
			}

		        matrix1 = malloc(rows1 * sizeof(int*));
		        for(i = 0 ; i < rows1 ; i++){
		            matrix1[i] = malloc(columns1 * sizeof(int)) ;
		            for(j = 0; j < columns1 ; j++)
		                fscanf(file , "%d" , &matrix1[i][j]) ;
		        }

		        matrix2 = malloc(rows2 * sizeof(int*));
		        for(i = 0 ; i < rows2 ; i++){
		            matrix2[i] = malloc(columns2 * sizeof(int)) ;
		            for(j = 0; j < columns2 ; j++)
		                fscanf(file , "%d" , &matrix2[i][j]) ;
		        }
		        fclose(file);
		}
		else{
			printf("Please, Enter first matrix dimension: ") ;
			scanf("%d%d" , &rows1 , &columns1) ;
			printf("Please, Enter second matrix dimension: ") ;
			scanf("%d%d" , &rows2 , &columns2) ;

			if(rows2 != columns1){
				printf("sorry, you can't multiply these matrices!\n") ;
				return 0 ;	
			}
			
			printf("Please, Enter first matrix:\n") ;
			matrix1 = malloc(rows1 * sizeof(int*));
			for(i = 0 ; i < rows1 ; i++){
			    matrix1[i] = malloc(columns1 * sizeof(int)) ;
			    for(j = 0; j < columns1 ; j++)
			        scanf("%d" , &matrix1[i][j]) ;
			}

			printf("Please, Enter second matrix:\n") ;
			matrix2 = malloc(rows2 * sizeof(int*));
			for(i = 0 ; i < rows2 ; i++){
			    matrix2[i] = malloc(columns2 * sizeof(int)) ;
			    for(j = 0; j < columns2 ; j++)
			        scanf("%d" , &matrix2[i][j]) ;
			}	
		}

		// make the  first matrix in 1D array
		tempArr = malloc(rows1 * columns1 * sizeof(int)) ;
		int idx=0 ;
		for(i=0 ; i < rows1 ; i++)
			for(j=0 ; j < columns1 ; j++)
				tempArr[idx++] = matrix1[i][j] ;

	}

	// send the second matrix to all processes
	MPI_Bcast(&columns1,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&rows1,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&columns2,1,MPI_INT,0,MPI_COMM_WORLD);
	MPI_Bcast(&rows2,1,MPI_INT,0,MPI_COMM_WORLD);
	if(rank != 0){
		matrix2 = malloc(rows2 * sizeof(int*));
		for(i = 0 ; i < rows2 ; i++)
			matrix2[i] = malloc(columns2 * sizeof(int)) ;
	}
	for(i = 0 ; i < rows2 ; i++)
		MPI_Bcast(matrix2[i] , columns2 , MPI_INT , 0 , MPI_COMM_WORLD) ;
	

	// handeling the sent rows to each process 
	int sentRows , tempRows;
	if(rows1 > numberOfProcesses){
		sentRows = columns1 * (rows1 / numberOfProcesses) ;
		tempRows = sentRows / columns1 ;
	}
	
	else{
		sentRows = columns1 ;
		tempRows = sentRows / columns1 ;
	}

	// send each process to 
	int *reciever = malloc(sentRows * sizeof(int)) ;
	MPI_Scatter(tempArr , sentRows , MPI_INT , reciever , sentRows , MPI_INT , 0 , MPI_COMM_WORLD);

	int cnt = tempRows , index = 0 , sum=0 , idx=0;
	int* result = malloc(tempRows * columns2 * sizeof(int)) ;
	int k , tempIndex;

	// make multipliction in each process 
	// cnt here is number of rows in each process 
	for(k = 0 ; k < cnt ; k++){
		tempIndex = k * columns1 ;
		for(i=0 ; i < columns2 ; i++){
			index = tempIndex ;
			for(j=0 ; j < rows2 ; j++)
				sum += reciever[index++] * matrix2[j][i] ;
			result[idx++] = sum ;
			sum = 0 ;
		}
	}	

	//collect the resut
	int * finalResult = malloc(columns2 * tempRows * sizeof(int));
	MPI_Gather(result , columns2 * tempRows , MPI_INT , finalResult , columns2 * tempRows , MPI_INT , 0 ,MPI_COMM_WORLD);

	// print the result
	if(rank == 0){
		int tempNOP = 0 ;
		printf("Final result: \n");
		if(numberOfProcesses > rows1)
			tempNOP = rows1 ;
		else
			tempNOP = numberOfProcesses ;
		for(i=0 ; i < tempNOP * (columns2 * tempRows) ; i++){
		    printf("%d " , finalResult[i]);
		    if((i+1) % columns2 == 0)
		    	printf("\n");
		}
	}

	// handeling the remainder
	if(rows1 % numberOfProcesses != 0 && rows1 > numberOfProcesses){
		MPI_Status status;
		if(rank == 0)
			MPI_Send(tempArr + (rows1 * columns1) - (rows1 % numberOfProcesses) * columns1 , (rows1 % numberOfProcesses) * columns1 , MPI_INT , numberOfProcesses - 1 , 0 , MPI_COMM_WORLD );
		if(rank == numberOfProcesses - 1){
			int * arr = malloc((rows1 % numberOfProcesses) * columns1 * sizeof(int)) ;
			MPI_Recv(arr , (rows1 % numberOfProcesses) * columns1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD , &status) ;

			cnt = rows1 % numberOfProcesses ;
			int*result1 = malloc(cnt * columns2 * sizeof(int)) ;
			index = 0 , sum = 0 , idx=0 ;
			for(k = 0 ; k < cnt ; k++){
				tempIndex = k * columns1 ;
				for(i=0 ; i < columns2 ; i++){
					index = tempIndex ;
					for(j=0 ; j < rows2 ; j++)
						sum += arr[index++] * matrix2[j][i] ;
					result1[idx++] = sum ;
					sum = 0 ;
				}
			}	
			MPI_Send(&cnt , 1 , MPI_INT , 0 , 0 , MPI_COMM_WORLD );
			MPI_Send(result1 , cnt * columns2 , MPI_INT , 0 , 0 , MPI_COMM_WORLD );
			for(i=0 ; i < cnt * columns2 ; i++){
			    printf("%d " , result1[i]);
			    if((i+1) % columns2 == 0)
			    	printf("\n");
			}
		}
	}	
/*
	for(i=0 ; i < numberOfProcesses * (columns2 * tempRows) ; i++)
		printf("%d ", semiResult[i]);
	printf("\n");
	*/

	MPI_Finalize();
	return 0 ;
}
