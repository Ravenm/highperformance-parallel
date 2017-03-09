// AndrewNash
//Serial matrix multiplication

#include <stdio.h>
//size of the arrays
#define ARRAY_SIZE 640

//init the arrays
int arrayA[ARRAY_SIZE][ARRAY_SIZE];
int arrayB[ARRAY_SIZE][ARRAY_SIZE];
int arrayC[ARRAY_SIZE][ARRAY_SIZE];

int main()
{
	//file io junk
	FILE *f = fopen("AndrewNashSerialH2.txt", "w");
	if (f == NULL)
	{
		printf("Error opening file!\n");
		return 0;
	}

	//fill arrays A and B with values and zero C
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		for (int j = 0; j < ARRAY_SIZE; j++)
		{
			arrayA[i][j] = 1;
			arrayB[i][j] = 2;
			arrayC[i][j] = 0;
		}
	}

	//loop through array A and B
	for (int i = 0; i<ARRAY_SIZE; i++) {
		for (int j = 0; j<ARRAY_SIZE; j++) {		
			for (int k = 0; k<ARRAY_SIZE; k++) {
				//i=a row j=a column k=traversal of row or column
				arrayC[i][j] += arrayA[i][k] * arrayB[k][j];
			}
		}
	}

	//print to file
	for (int i = 0; i < ARRAY_SIZE; i++)
	{
		for (int j = 0; j < ARRAY_SIZE; j++)
		{
			fprintf(f, " %d ", arrayC[i][j]);
		}
		fprintf(f, "\n");
	}

	fclose(f);
    return 0;
}

