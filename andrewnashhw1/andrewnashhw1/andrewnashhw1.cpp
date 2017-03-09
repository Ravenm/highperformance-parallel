// andrewnashhw1.c
//This is a parallel example of matrix multiplication
//this does not use collective communication

#include <mpi.h>
#include <stdio.h>
#include <filesystem>
#define ARRAY_SIZE 640
#define ROW_SIZE 40

//init the needed arrays note that arrayC is used for sumation only
int arrayA[ARRAY_SIZE][ARRAY_SIZE];
int arrayB[ARRAY_SIZE][ARRAY_SIZE];
int arrayC[ROW_SIZE][ARRAY_SIZE];
int sumarray[ARRAY_SIZE][ARRAY_SIZE];


int main(int argc, char* argv[])
{
	int size, rank, source, dest, offset, i, j, k;

	MPI_Init(&argc, &argv);      /* starts MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);        /* get current process id */
	MPI_Comm_size(MPI_COMM_WORLD, &size);        /* get number of processes */



	for (int k = 0; k < ARRAY_SIZE; k++)
	{
		for (int l = 0; l < ARRAY_SIZE; l++)
		{
			arrayA[k][l] = 1;
			arrayB[k][l] = 2;
			arrayC[k][l] = 0;
		}
	}

	if (rank == 0) {
		FILE *f = fopen("file.txt", "w");
		if (f == NULL)
		{
			printf("Error opening file!\n");
			return 0;
		}
		int temp = 0;
		//loop through array A and B
		for (int i = 0; i < ROW_SIZE; i++) {
			for (int j = 0; j < ARRAY_SIZE; j++) {
				for (int k = 0; k<ARRAY_SIZE; k++) {
					temp += arrayA[i][k] * arrayB[k][j];
				}
				sumarray[i][j] = temp;
				temp = 0;
			}
		}

		//get data back
		for (i = 1; i <= size-1; i++)
		{
			source = i;
			MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&sumarray[offset][0], ROW_SIZE*ARRAY_SIZE, MPI_INT, source, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		//print to file
		for (int i = 0; i < ARRAY_SIZE; i++)
		{
			for (int j = 0; j < ARRAY_SIZE; j++)
			{
				fprintf(f, " %d ", sumarray[i][j]);
			}
			fprintf(f, "\n");
		}

	}

	//slave
	else {
		offset = rank * ROW_SIZE;
		int rowcount = 0;
		//loop through array A and B
		for (int i = offset; i< offset + ROW_SIZE; i++) {
			for (int j = 0; j< ARRAY_SIZE; j++) {
				for (int k = 0; k<ARRAY_SIZE; k++) {
					arrayC[rowcount][j] += arrayA[i][k] * arrayB[k][j];
				}
				
			}
			rowcount++;
		}
		
		MPI_Send(&offset, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&arrayC[0][0], ROW_SIZE*ARRAY_SIZE, MPI_INT, 0, 2, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}

