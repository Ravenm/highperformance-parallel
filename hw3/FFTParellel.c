/*
	Austin Cullar
	Andrew Nash
	FFT
	HPC - Colmenares
	This program will use a Radix 2 Fast Fourier Transform to
	compute real and imaginary components for each N
	README:
		The algorithm is transcribed directly from lecture 19 on D2L.
		I used a constant approximation of pi since one didn't seem to exist in c already.

		The even and odd functions *could* be combined with the use of a boolean, but I
		thought leaving them seperate might more easily lend the program to parallelization.
		The algorithm works by sending the full array to both functions, allowing each to compute
		their own portion of the array. The result is then sent back in the form of a complex number.
		The last step involves adding the parts together to achieve the final result.
*/

#include<stdio.h>
#include<math.h>
#include<mpi.h>

#define N 1024 //number of samples expected 1024 or 16384
#define SUB 64//N div processes 16 or 32 that will be used expected 64,32 or 1024,512
#define pi 3.141593

/*
	struct to be used to represent complex numbers
*/
struct complex{
    double real;
    double imag;
};

/*
	This function was written in reference to Lecture 19
	The following is to represent the calculation of the even
	part of the FFT.
    param: complex array list of given samples, int k which output are we calculating
*/
struct complex evenFFT(struct complex x[N], int k){
    int i;
    struct complex result = {0, 0}, r;
    for(i=0; i<=(N/2)-1; i++){
        r.real = cos((2*pi*((2*i)+1)*k)/N);
        r.imag = -sin((2*pi*((2*i)+1)*k)/N);
        //multiply (for debugging, change '+=' to '=' to see intermediate calculations for n(i))
        result.real += (x[2*i].real*r.real - x[2*i].imag*r.imag);
        result.imag += (x[2*i].imag*r.real + x[2*i].real*r.imag);
    }
    return result;
}

/*
	This function will be used to compute the odd part of
	the FFT.
    param: complex array list of given samples, int k which output are we calculating
*/
struct complex oddFFT(struct complex x[N], int k){
    int i;
    struct complex result = {0, 0}, r;
    for(i=0; i<=(N/2)-1; i++){
        r.real = cos((2*pi*((2*i)+1)*k)/N);
        r.imag = -sin((2*pi*((2*i)+1)*k)/N);
        //multiply
        result.real += (x[2*i+1].real*r.real - x[2*i+1].imag*r.imag);
        result.imag += (x[2*i+1].imag*r.real + x[2*i+1].real*r.imag);
    }
    return result;
}
/*
 * twiddle factor needed for combining odd and even parts of fft
 * param int k this is the final output location ie X[k]
 */
struct complex twiddleFact(int k)
{
    struct complex result;
    result.real = cos(2*pi*k/N);
    result.imag = -sin(2*pi*k/N);

    return result;
}

int main(int argc, char* argv[]){

    int size, rank, source;

    MPI_Init(&argc, &argv);      /* starts MPI */
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);        /* get current process id */
    MPI_Comm_size(MPI_COMM_WORLD, &size);        /* get number of processes */
    struct complex out[N];
    struct complex x[N], odd, even, twiddle;
    double realList[N], imgList[N];
    //check if the size is correct if not close out
    if(size == 16 || size == 32)
    {
        int i = 7;

        //add data:
        x[0].real = 3.6;
        x[0].imag = 2.6;
        x[1].real = 2.9;
        x[1].imag = 6.3;
        x[2].real = 5.6;
        x[2].imag = 4;
        x[3].real = 4.8;
        x[3].imag = 9.1;
        x[4].real = 3.3;
        x[4].imag = 0.4;
        x[5].real = 5.9;
        x[5].imag = 4.8;
        x[6].real = 5;
        x[6].imag = 2.6;
        x[7].real = 4.3;
        x[7].imag = 4.1;
        while(i < N)
        {
            x[i].real = 0;
            x[i].imag = 0;
            i++;
        }

        //if process 0
        if(rank == 0)
        {
            //calculate first (N/size)
            //computation
            int k = 0;
            while(k < N/2)
            {
                even = evenFFT(x, k); //even part
                odd = oddFFT(x, k); //odd part
                twiddle = twiddleFact(k);

                out[k].real = even.real + (twiddle.real * odd.real);
                out[k].imag = even.imag + (twiddle.imag * odd.imag);
                //calculate the matching set of real imaginary
                out[k + N/2].real = even.real + (twiddle.real * odd.real);
                out[k + N/2].imag = even.imag - (twiddle.imag * odd.imag);
                k++;
            }
        }
            //slave process
        else
        {
            int offset = rank * SUB;
            //computation
            int k = 0;
            while(k < SUB)
            {
                even = evenFFT(x, k + offset); //even part
                odd = oddFFT(x, k + offset); //odd part
                twiddle = twiddleFact(k + offset);
                //using the real and img list store all the values for calculated pairs
                realList[k] = even.real + (twiddle.real * odd.real);
                imgList[k] = even.imag + (twiddle.imag * odd.imag);
                k++;
                realList[k] = even.real + (twiddle.real * odd.real);
                imgList[k] = even.imag - (twiddle.imag * odd.imag);
                k++;
            }
        }
        //send both the real and imaginary parts
        MPI_Gather(&imgList,SUB,MPI_DOUBLE, &imgList,SUB,MPI_DOUBLE,0,MPI_COMM_WORLD);
        MPI_Gather(&realList, SUB,MPI_DOUBLE, &imgList,SUB,MPI_DOUBLE,0,MPI_COMM_WORLD);
        if(rank == 0)
        {
            int i =SUB;
            while(i < N)
            {
                //combine the sent array in the correct order and into a sturct
                //it is known that the array is sent in batches so we can calcuate
                //the location of the matching pair for each X[k]
                //ie X[k] and X[k + N/2]
                out[i].real = realList[i];
                out[i].imag = imgList[i];

                out[i + N/2].real = realList[i + 1];
                out[i + N/2].imag = imgList[i + 1];
                i += 2;
            }

            //output into a txt file
            FILE* f = fopen("output.txt", "w");
            if (f == NULL)
            {
                printf("Error opening file!\n");
                return 0;
            }

            //print to file
            i = N;
            fprintf(f, "TOTAL PROCESSED SMAPLES: %lf", i);
            i = 0;
            fprintf(f, "=============================\n");
            while(i < N)
            {
                fprintf(f, "XR[%lf]: %lf  XR[%lf]: %lfi\n", i, out[i].real, i, out[i].imag);
                fprintf(f, "=============================\n");
                i++;
            }

        }
    }
    MPI_Finalize();

    return 0;
}
