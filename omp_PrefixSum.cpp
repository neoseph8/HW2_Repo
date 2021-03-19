#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>
#include <iostream>
#include <time.h>
#include <math.h>
using namespace std;

int main()
{



    clock_t t;
    int* partial, * temp, * x, * z;
    int all_threads, work, n = 0;
    int i, this_thread, last;
    float tSeq, tMPI;
    //int* x = new int[n];

    std::cout << "Seq Time, " << "OMP Time, " << "Problem Size, " << "Thread Count";
    for (int nThread = 0; nThread < 8; nThread++) {
        for (int nCount = 0; nCount < 5; nCount++) {
            tSeq = 0;
            tMPI = 0;
            for (int myCount = 0; myCount < 3; myCount++) {


                n = 1 << (20 + nCount);
                if (!(x = (int*)malloc(sizeof(int) * n))) return -1;
                if (!(z = (int*)malloc(sizeof(int) * n))) return -1;
                for (int j = 0; j < n; j++) {
                    x[j] = 1;
                    z[j] = 1;

                }

                t = clock();
                for (int i = 1; i < n; i = i * 2)
                {
                    for (int j = n - 1; j > i - 1; j--)
                    {
                        z[j] = z[j] + z[j - i];
                    }
                }
                tSeq += (clock() - t)/3.0;
              






                t = clock();

                omp_set_num_threads(pow(2, nThread + 1));


#pragma omp parallel default(none) private(i, this_thread, last) shared(x, partial, temp, all_threads, work, n)
                {
#pragma omp single
                    {
                        all_threads = omp_get_num_threads();
                        if (!(partial = (int*)malloc(sizeof(int) * all_threads))) exit(-1);
                        if (!(temp = (int*)malloc(sizeof(int) * all_threads))) exit(-1);
                        work = n / all_threads + 1; /*sets length of sub-arrays*/
                    }


                    this_thread = omp_get_thread_num();

                    for (i = work * this_thread + 1; i < work * this_thread + work && i < n; i++)
                        x[i] += x[i - 1];
                    partial[this_thread] = x[i - 1];
#pragma omp barrier

                    for (i = 1; i < all_threads; i <<= 1) {
                        if (this_thread >= i)
                            temp[this_thread] = partial[this_thread] + partial[this_thread - i];
#pragma omp barrier
#pragma omp single
                        memcpy(partial + 1, temp + 1, sizeof(int) * (all_threads - 1));
                    }

                    for (i = work * this_thread; i < (last = work * this_thread + work < n ? work * this_thread + work : n); i++)
                        x[i] += partial[this_thread] - x[last - 1];
                }


                    free(x);
                    free(partial);
                    free(temp);

                tMPI += (clock() - t)/3.0;


            }
            std::cout << "\n" << tSeq <<", "<<tMPI<<", "<<log2(n)<<", "<<all_threads;


            

        }
    }

    return 0;
}