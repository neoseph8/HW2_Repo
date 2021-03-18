#include <iostream>
#include <math.h>
#include <fstream>
#include <time.h>       /* clock_t, clock, CLOCKS_PER_SEC */
#include <cuda.h>
using namespace std;
// Kernel function to add the elements of two arrays
__global__
void Shift(int n, float* x, float* y, int shift) //Places x(i-shift) into y(i)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;
    for (int i = index + shift; i < n; i += stride) {
        y[i] = x[i - shift];
    }

}
__global__
void Add(int n, float* x, float* y, int shift)
{
    int index = blockIdx.x * blockDim.x + threadIdx.x;
    int stride = blockDim.x * gridDim.x;

    for (int i = index; i < n; i += stride){
        if (i>shift-1)        x[i] = x[i] + y[i];
    }
}

int main(void)
{
    int N = 1 << 25;
    float* x, * y;
    //int jump = 0;

    // Allocate Unified Memory – accessible from CPU or GPU
    cudaMallocManaged(&x, N * sizeof(float));
    cudaMallocManaged(&y, N * sizeof(float));
   // cudaMallocManaged(&z, N * sizeof(float));


    // initialize x and y arrays on the host
    for (int i = 0; i < N; i++) {
        x[i] = 1.0f;
        y[i] = 0.0f;
 //       z[i] = 0.0f;
    }

    // Run kernel on 1M elements on the GPU
    
    int blockSize = 256;
    int numBlocks = (N + blockSize - 1) / blockSize;

    clock_t t;
    t = clock();
    for (int jump = 1; jump < N ; jump = jump * 2) {
          

        Shift << <numBlocks, blockSize >> > (N, x, y, jump);
        Add << <numBlocks, blockSize >> > (N, x, y, jump);
        cudaDeviceSynchronize();
// Wait for GPU to finish before accessing on host
    }
    

    
    t = clock()-t;
	std::cout<<"Time to completion (ticks): "<<t;
	


    // Free memory
    cudaFree(x);
    cudaFree(y);

    return 0;
}