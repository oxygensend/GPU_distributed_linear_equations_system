#include "device_launch_parameters.h"
#include <fstream>
#include <iostream>
#include <cuda_runtime.h>

#include <highfive/H5File.hpp>

__global__ void detGaussKernel(double* mat, int n, double* det) {
    int i, j, k;
    double ratio;

    // Forward elimination
    for (i = 0; i < n - 1; i++) {
        for (j = i + 1; j < n; j++) {
            ratio = mat[j * n + i] / mat[i * n + i];

            for (k = i + 1; k < n; k++) {
                mat[j * n + k] -= ratio * mat[i * n + k];
            }
        }
    }

    // Calculate determinant
    *det = 1.0;
    for (i = 0; i < n; i++) {
        *det *= mat[i * n + i];
    }
}

int main(int argc, char* argv[]) {


    if (argc != 3) {
        perror("Pass file name as second argument and dataset name as third. \n");
        return -1;
    }
    double* mat, * d_mat;
    double det, * d_det;

    HighFive::File file(argv[1], HighFive::File::ReadOnly);

    auto dataset = file.getDataSet(argv[2]);
    auto dataspace = dataset.getSpace();
    std::vector<size_t> dims(dataspace.getDimensions());
    std::vector<double> data(dims[0] * dims[1]);
    dataset.read(data.data());

  

    const int n = sqrt(data.size());

    mat = new double[n * n];
    mat = &data[0];

    cudaMalloc(&d_mat, n * n * sizeof(double));
    cudaMalloc(&d_det, sizeof(double));

    

    // Copy matrix to device
    cudaMemcpy(d_mat, mat, n * n * sizeof(double), cudaMemcpyHostToDevice);

    // Calculate determinant on device
    detGaussKernel << <1, 1 >> > (d_mat, n, d_det);

    // Copy determinant from device
    cudaMemcpy(&det, d_det, sizeof(double), cudaMemcpyDeviceToHost);

    //std::cout << "Determinant: " << det << std::endl;
    
    std::ofstream output_file;
    output_file.open("..\\outputFile.txt", std::fstream::in | std::fstream::trunc);
    output_file << det;


    output_file.close();
    // Free memory
    //delete[] mat;
    cudaFree(d_mat);
    cudaFree(d_det);

    return 0;
}
