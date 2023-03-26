#include <stdio.h>
#include <stdlib.h>
#include <cuda_runtime.h>
#include <cusolverDn.h>

int main() {
    const int n = 3; // size of the matrix
    const int nrhs = 1; // number of right-hand sides

    // example matrix - macierz zapisujemy {[0,0], [0,1], [0,2], [1,0], [1,1], [1,2] ,[2,0], [2,1], [2,2]}  -kolumnami
    float A[n*n] = {2, 4, -2, 1, -6, 7, 1, 0, 2};
    float B[n*nrhs] = {5,-2,9};

    // allocate memory on the device
    float *dA, *dB, *dX;
    int *dipiv; // pivoting information

    cudaMalloc((void**)&dA, n * n * sizeof(float));
    cudaMalloc((void**)&dB, n * nrhs * sizeof(float));
    cudaMalloc((void**)&dX, n * nrhs * sizeof(float));
    cudaMalloc((void**)&dipiv, n * sizeof(int));

    // copy input matrices to the device
    cudaMemcpy(dA, A, n * n * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(dB, B, n * nrhs * sizeof(float), cudaMemcpyHostToDevice);

    // create cusolverDn handle
    cusolverDnHandle_t handle;
    cusolverDnCreate(&handle);

    // factorize and solve the linear system
    int niter, dinfo;
    size_t workspace_size;
    cusolverStatus_t status = cusolverDnSSgesv_bufferSize(handle, n, nrhs, dA, n, dipiv, dB, n, dX, n, NULL, &workspace_size);

    if (status != CUSOLVER_STATUS_SUCCESS) {
        printf("Error querying workspace size: %d\n", status);
        return 1;
    }

    status = cusolverDnSSgesv(handle, n, nrhs, dA, n, dipiv, dB, n, dX, n, NULL, workspace_size, &niter, &dinfo);

    printf("%d %long %d %d\n", status, workspace_size, dinfo, niter);
    if (status != CUSOLVER_STATUS_SUCCESS) {
        printf("Error solving linear system: %d\n", status);
        return 1;
    }

    if (dinfo != 0) {
        printf("Error solving linear system: dinfo = %d\n", dinfo);
        return 1;
    }

    // copy the solution back to the host
    float X[n*nrhs];
    cudaMemcpy(X, dX, n * nrhs * sizeof(float), cudaMemcpyDeviceToHost);

    // print the solution
    printf("Solution:\n");
    for (int i = 0; i < n; i++) {
        printf("%f\n", X[i]);
    }

    // free memory
    cudaFree(dA);
    cudaFree(dB);
    cudaFree(dX);
    cudaFree(dipiv);
    cusolverDnDestroy(handle);

    return 0;
}
