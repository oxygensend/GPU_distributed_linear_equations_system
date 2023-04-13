#include "device_launch_parameters.h"
#include <fstream>
#include <iostream>
#include <cuda_runtime.h>

// Function to compute the determinant of a matrix using the Laplace expansion method
__device__ float determinantLaplace(float* matrix, int n) {
    if (n == 1) {
        return matrix[0];
    }

    float result = 0;
    float sign = 1;

    // Allocate memory for submatrix
    float* submatrix = new float[(n - 1) * (n - 1)];

    // Compute the determinant using the Laplace expansion method
    for (int i = 0; i < n; i++) {
        // Extract submatrix
        int sub_i = 0;
        for (int j = 1; j < n; j++) {
            int sub_j = 0;
            for (int k = 0; k < n; k++) {
                if (k != i) {
                    submatrix[sub_i * (n - 1) + sub_j] = matrix[j * n + k];
                    sub_j++;
                }
            }
            sub_i++;
        }

        // Compute the determinant of the submatrix
        float sub_det = determinantLaplace(submatrix, n - 1);

        // Add to result
        result += sign * matrix[i] * sub_det;
        sign = -sign;
    }

    // Free memory for submatrix
    delete[] submatrix;

    return result;
}

// Kernel function to compute the determinant of a matrix using the Laplace expansion method
__global__ void determinantLaplaceKernel(float* matrix, float* det, int n) {
    int tid = threadIdx.x;

    // Compute determinant using the Laplace expansion method
    det[tid] = determinantLaplace(matrix, n);
}

// Function to compute the determinant of a matrix using the Laplace expansion method
float determinantLaplaceCUDA(float* matrix, int n) {
    // Allocate device memory
    float* d_matrix, * d_det;
    cudaMalloc(&d_matrix, n * n * sizeof(float));
    cudaMalloc(&d_det, n * sizeof(float));

    // Copy matrix to device memory
    cudaMemcpy(d_matrix, matrix, n * n * sizeof(float), cudaMemcpyHostToDevice);

    // Launch kernel to compute determinant using the Laplace expansion method
    determinantLaplaceKernel << <1, n >> > (d_matrix, d_det, n);

    // Copy determinant from device to host memory
    float result;
    cudaMemcpy(&result, d_det, sizeof(float), cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_matrix);
    cudaFree(d_det);

    return result;
}

int main(int argc, char* argv[]) {
    // Define matrix
    if (argc != 1) {
        perror("You must specify file name as first argument.");
        exit(-1);
    }

    std::fstream newfile;
    newfile.open("data0.txt", std::ios::in); //open a file to perform read operation using file object

    const int Nrows = 100;
    float h_A[Nrows*Nrows];

    for (int i = 0; i < Nrows*Nrows; i++){
            newfile >> h_A[i];
    }
    // Compute determinant
    float det = determinantLaplaceCUDA(h_A, Nrows);

    // Print result

    // Print result
    std::cout << "Determinant: " << det << std::endl;

    return 0;
}



