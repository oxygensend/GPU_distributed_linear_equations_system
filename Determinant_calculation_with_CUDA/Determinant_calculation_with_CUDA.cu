#include "device_launch_parameters.h"
#include <fstream>
#include <iostream>
#include <cuda_runtime.h>
//#include "hdf5.h"
#include <highfive/H5File.hpp>

using HighFive::File;

__device__ double det3(int n, int w, int* WK, double** A)
{
    int    i, j, k, m, * KK;
    double s;

    if (n == 1)                     // sprawdzamy warunek zakoñczenia rekurencji

        return A[w][WK[0]];    // macierz 1 x 1, wyznacznik równy elementowi

    else
    {

        KK = new int[n - 1];        // tworzymy dynamiczny wektor kolumn

        s = 0;                         // zerujemy wartoœæ rozwiniêcia
        m = 1;                         // pocz¹tkowy mno¿nik

        for (i = 0; i < n; i++)       // pêtla obliczaj¹ca rozwiniêcie
        {

            k = 0;                       // tworzymy wektor kolumn dla rekurencji

            for (j = 0; j < n - 1; j++) // ma on o 1 kolumnê mniej ni¿ WK
            {
                if (k == i) k++;          // pomijamy bie¿¹c¹ kolumnê
                KK[j] = WK[k++];     // pozosta³e kolumny przenosimy do KK
            }

            s += m * A[w][WK[i]] * det3(n - 1, w + 1, KK, A);

            m = -m;                      // kolejny mno¿nik

        }

        delete[] KK;                 // usuwamy zbêdn¹ ju¿ tablicê dynamiczn¹

        return s;                      // ustalamy wartoœæ funkcji

    }
}


// Function to compute the determinant of a matrix using the Laplace expansion method
__device__ double determinantLaplace(double* matrix, int n) {
    if (n == 1) {
        return matrix[0];
    }

    double result = 0;
    double sign = 1;

    // Allocate memory for submatrix
    double* submatrix = new double[(n - 1) * (n - 1)];

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
__global__ void determinantLaplaceKernel(int n, int w, int* WK, double** A, double* det) {
    int tid = threadIdx.x;

    // Compute determinant using the Laplace expansion method
    det[tid] = det3( n,w,WK,A);
}

// Function to compute the determinant of a matrix using the Laplace expansion method
double determinantLaplaceCUDA(int n, int w, int* WK, double** A) {
    // Allocate device memory
    double* d_matrix, * d_det;
    cudaMalloc(&d_matrix, n * n * sizeof(double));
    cudaMalloc(&d_det, n * sizeof(double));

    // Copy matrix to device memory
    cudaMemcpy(d_matrix, A, n * n * sizeof(double), cudaMemcpyHostToDevice);

    // Launch kernel to compute determinant using the Laplace expansion method
    determinantLaplaceKernel << <1, n >> > (n, w, WK, A,d_det);

    // Copy determinant from device to host memory
    double result;
    cudaMemcpy(&result, d_det, sizeof(double), cudaMemcpyDeviceToHost);

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
    double** h_A;
    h_A = new double* [Nrows];

    for (int i = 0; i < Nrows; i++)
    {

        h_A[i] = new double[Nrows];    

        for (int j = 0; j < Nrows; j++)
            newfile >> h_A[i][j]; 

    }
    int* WK = new int[Nrows];              

    for (int i = 0; i < Nrows; i++)       
        WK[i] = i;

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
    double det = determinantLaplaceCUDA(Nrows, 0, WK, h_A);
    //double det2 = det(Nrows,0,WK,h_A);

    // Print result
    std::cout << "Determinant: " << det << std::endl;

    return 0;
}



