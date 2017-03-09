// andrewnashhw1.cpp : Defines the entry point for the console application.
//

#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
	const int summationSize = 10000, partySize = 100;
	int largeArray[summationSize];
	int rank, size;
	int sum = 0;
	int localArray[summationSize/partySize];
	int summationArray[partySize];

	MPI_Init(&argc, &argv);      /* starts MPI */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);        /* get current process id */
	MPI_Comm_size(MPI_COMM_WORLD, &size);        /* get number of processes */

	
	//many loops
	if( rank == 0 )
	{
		//make sure slot 0 is clean
		summationArray[0] = 0;
		//using the size of ints to be summed load large array
		for (int i = 0; i < summationSize; i++)
		{
			largeArray[i] = i+1;
		}
		//sum for process 0
		for (int i = 0; i < partySize; i++)
		{
			summationArray[0] += largeArray[i];
		}
		//ship array parts to processes
		for (int i = 1; i < size; i ++)
		{
			MPI_Send(&largeArray[i*partySize], partySize, MPI_INT, i, 1, MPI_COMM_WORLD);
		}
		//get sums back from processes
		for (int i = 1; i < size; i++)
		{
			MPI_Recv(&summationArray[i], 1, MPI_INT, i, 2, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		}
		//total sum and print out of each process
		for (int i = 0; i < partySize; i++)
		{
			printf("summation recieved from process %d is :: %d \n", i, summationArray[i]);
			sum += summationArray[i];
		}
		printf("The sum is: %d", sum);

	}
	else
	{
		//recieve array parts
		MPI_Recv(&localArray, partySize, MPI_INT, 0, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		//sum the recieved array
		for (int i = 0; i < partySize; i++)
		{
			sum += localArray[i];
		}
		//ship sum back to process 0
		MPI_Send(&sum, 1, MPI_INT, 0, 2, MPI_COMM_WORLD);
	}

	MPI_Finalize();
	return 0;
}

