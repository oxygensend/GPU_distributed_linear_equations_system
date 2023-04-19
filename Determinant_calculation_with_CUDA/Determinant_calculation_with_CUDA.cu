#include "device_launch_parameters.h"
#include <fstream>
#include <iostream>
#include <cuda_runtime.h>
//#include "hdf5.h"
#include <highfive/H5File.hpp>


using HighFive::File;




// Function to compute the determinant of a matrix using the Laplace expansion method
__device__ double determinantLaplace(double* matrix, int n) {
    if (n == 1) {
        return matrix[0];
    }

    double result = 0;
    double sign = 1;

    // Allocate memory for submatrix
    double* submatrix ;
    cudaMalloc(&submatrix, (n - 1) * (n - 1)* sizeof(double));
    // Compute the determinant using the Laplace expansion method
    for (int i = 0; i < n; i++) {
        // Extract submatrix
        int sub_i = 0;
        for (int j = 1; j < n; j++) {
            int sub_j = 0;
            for (int k = 0; k < n; k++) {
                if (k != i) {
# if __CUDA_ARCH__>=200
                    printf("%f", submatrix[sub_i * (n - 1) + sub_j]);

#endif
                    
                    submatrix[sub_i * (n - 1) + sub_j] = matrix[j * n + k];
                    sub_j++;
                }
            }
            sub_i++;
        }

        // Compute the determinant of the submatrix
        double sub_det = determinantLaplace(submatrix, n - 1);

        // Add to result
        result += sign * matrix[i] * sub_det;
        sign = -sign;
    }

    // Free memory for submatrix
    delete[] submatrix;

    return result;
}

// Kernel function to compute the determinant of a matrix using the Laplace expansion method
__global__ void determinantLaplaceKernel(double* matrix, int n, double* det) {
    int tid = threadIdx.x;

    // Compute determinant using the Laplace expansion method
    *det = determinantLaplace(matrix,n);
    printf("%f", det);

}

// Function to compute the determinant of a matrix using the Laplace expansion method
double determinantLaplaceCUDA(double* matrix, int n) {
    // Allocate device memory
    double* d_matrix;
    double * d_det;
    cudaMalloc(&d_matrix, n * n * sizeof(double));
    cudaMalloc(&d_det, sizeof(double));

    // Copy matrix to device memory
    cudaMemcpy(d_matrix, matrix, n * n * sizeof(double), cudaMemcpyHostToDevice);
    int threadsPerBlock = 256;
    int blocksPerGrid =(n + threadsPerBlock - 1) / threadsPerBlock;
    // Launch kernel to compute determinant using the Laplace expansion method
    determinantLaplaceKernel << <1, 1 >> > (matrix,n,d_det);
 
    // Copy determinant from device to host memory
    double* result;
    cudaMemcpy(&result, d_det, sizeof(double), cudaMemcpyDeviceToHost);

    // Free device memory
    cudaFree(d_matrix);
    cudaFree(d_det);

    return  *result;
;
}

int mai2n(int argc, char* argv[]) {
    // Define matrix
    if (argc != 1) {
        perror("You must specify file name as first argument.");
        exit(-1);
    }
    
    std::fstream newfile;
    newfile.open("data0.txt", std::ios::in); //open a file to perform read operation using file object

    const int Nrows = 5;
    double h_A[Nrows*Nrows];
    

    for (int i = 0; i < Nrows * Nrows; i++) {
        newfile >> h_A[i];
    }


    std::vector<double> result;
    /*
    HighFive::File file("data37.h5", HighFive::File::ReadOnly);

    auto dataset = file.getDataSet("dataset_1");
    auto dataspace = dataset.getSpace();
    std::vector<size_t> dims(dataspace.getDimensions());
    std::vector<double> data(dims[0] * dims[1]);
    dataset.read(data.data());

    double* a = &data[0];
     */
    // Compute determinant
    double det = determinantLaplaceCUDA( h_A, Nrows);
    //double det2 = det(Nrows,0,WK,h_A);

    // Print result
    std::cout << "Determinant: " << det << std::endl;

    return 0;
} 



