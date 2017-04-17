#include <stdio.h>
#include <pthread.h>

int main() {

    //create 100million int array and set to zero
    int arrayLarge[100000000] = { 0 };
    //create two buffers holding 100,000 ints
    //create a counter that will hold which chunk of array is processed
    //notice there is no lock on arraylarge this is because the two threads should never be reading/writing from the
    //  same element in the array as they are offset by the buffer size of 100,000 at all times

    //main thread
    // lock buffer 1
    //fill buffer 1
    //unlock buffer 1
    //lock buffer 2
    //fill buffer 2
    //unlock buffer 2

    //create thread worker

    //worker
    //example of loop
    //for int i < arraylarge size/100,000
    //  while buffered array1 is locked
    //      wait
    //  lock buffered array1
    //  process buffer array1 putting results into arrayLarge then update counter with +100,000
    //  unlock buffered array1
    //  while buffered array 2 is locked
    //      wait
    //  lock buffered array2
    //  process buffer array2 putting results into arrayLarge then update counter with +100,000
    //  lock buffered array2



    //for int i = 200,000  < arraylarge size/100,000
    //  while buffered array1 is locked
    //      wait
    //  lock buffered array1
    //  fill buffer array1
    //  unlock buffered array1
    //  while buffered array 2 is locked
    //      wait
    //  lock buffered array2
    //  fill buffer array2
    //  unlock buffer array2

    return 0;
}