// andrewnashhw1.cpp : Defines the entry point for the console application.
//

#include <mpi.h>
#include <iostream>

#define ARRAY_SIZE 640

int arrayA[ARRAY_SIZE][ARRAY_SIZE];
int arrayB[ARRAY_SIZE][ARRAY_SIZE];
int arrayC[ARRAY_SIZE][ARRAY_SIZE];


int main(int argc, char* argv[])
{
	int size, rank, source, dest, rows, offset, i, j, k;

	MPI_Init(&argc, &argv);      /* starts MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);        /* get current process id */
	MPI_Comm_size(MPI_COMM_WORLD, &size);        /* get number of processes */

	
	for (int k = 0; k < ARRAY_SIZE; i++)
	{
		for (int l = 0; l < ARRAY_SIZE; j++)
		{
			arrayA[k][l] = 1;
			arrayB[k][l] = 2;
			arrayC[k][l] = 0;
		}
	}

	if (rank == 0) {

		rows = ARRAY_SIZE / size;
		offset = 0;

		for (dest = 1; dest <= size-1; dest++)
		{
			MPI_Send(&offset, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
			MPI_Send(&rows, 1, MPI_INT, dest, 1, MPI_COMM_WORLD);
			MPI_Send(&arrayA[offset][0], rows*ARRAY_SIZE, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
			MPI_Send(&arrayB, ARRAY_SIZE*ARRAY_SIZE, MPI_DOUBLE, dest, 1, MPI_COMM_WORLD);
			offset = offset + rows;
		}

		//get data back
		for (i = 1; i <= size-1; i++)
		{
			source = i;
			MPI_Recv(&offset, 1, MPI_INT, source, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&rows, 1, MPI_INT, source, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			MPI_Recv(&arrayC[offset][0], rows*ARRAY_SIZE, MPI_DOUBLE, source, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}

		cout << size;
	}

	//slave
	else {
		source = 0;
		MPI_Recv(&offset, 1, MPI_INT, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&rows, 1, MPI_INT, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&arrayA, rows*ARRAY_SIZE, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&arrayB, ARRAY_SIZE*ARRAY_SIZE, MPI_DOUBLE, source, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

		/* Matrix multiplication */
		for (k = 0; k<ARRAY_SIZE; k++)
			for (i = 0; i<rows; i++) {
				arrayC[i][k] = 0;
				for (j = 0; j<ARRAY_SIZE; j++)
					arrayC[i][k] = arrayC [i][k] + arrayA[i][j] * arrayB[j][k];
			}


		MPI_Send(&offset, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&rows, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
		MPI_Send(&arrayC, rows*ARRAY_SIZE, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}

