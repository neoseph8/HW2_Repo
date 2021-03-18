#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include<iostream>
using namespace std;

int main()
{
    
    int* partial, * temp;
    int all_threads, work, n = 1<<10;
    int i, this_thread, last;
    int* x = new int[n];



    
    for (int j = 0; j < n; j++) {
        x[j] = 1;

    }




#pragma omp parallel default(none) private(i, this_thread, last) shared(x, partial, temp, all_threads, work, n)
    {
#pragma omp single
        {
            all_threads = omp_get_num_threads();
   //         if (!(partial = (int*)malloc(sizeof(int) * num_threads))) exit(-1);
     //       if (!(temp = (int*)malloc(sizeof(int) * num_threads))) exit(-1);
            work = n / all_threads + 1; /*sets length of sub-arrays*/
        }
        this_thread = omp_get_thread_num();
        //calculate prefix-sum for each subarray
        for (i = work * this_thread + 1; i < work * this_thread + work && i < n; i++)
            x[i] += x[i - 1];
        partial[this_thread] = x[i - 1];
#pragma omp barrier
        //calculate prefix sum for the array that was made from last elements of each of the previous sub-arrays
        for (i = 1; i < all_threads; i <<= 1) {
            if (this_thread >= i)
                temp[this_thread] = partial[this_thread] + partial[this_thread - i];
#pragma omp barrier
#pragma omp single
            memcpy(partial + 1, temp + 1, sizeof(int) * (all_threads - 1));
        }
        //update original array
        for (i = work * this_thread; i < (last = work * this_thread + work < n ? work * this_thread + work : n); i++)
            x[i] += partial[this_thread] - x[last - 1];
    }
    

    cout << "The first value of X is: " << x[0];
    cout << "\nThe last value of X is: " << x[n - 1];
    return 0;
}