import numpy as np
import time
import h5py

class ProblemGenerator:

    x_size = 1000
    batch_size = 100
    x_min = -10
    x_max = 10
    a_min = -10
    a_max = 10


    @classmethod
    def generate_matrices(cls):

        W = 1
        M = []
        tasks = []

        start_time = time.time()


        A = np.random.rand(cls.x_size, cls.x_size)*(cls.a_max-cls.a_min)+cls.a_min
        X_old = np.random.rand(cls.x_size, 1)*(cls.x_max-cls.x_min)+cls.x_min
        B = np.matmul(A, X_old)

        W = np.linalg.det(A)

        X = np.empty([cls.x_size, 1])
        
        for i in range(cls.x_size):
            tmp = np.copy(A)
            for k in range(cls.x_size):
                tmp[k][i] = B[k][0]
            M.append(tmp)
            tasks.append([0, 0, 0])

        # for ind, matrix in enumerate(M):
        #     with open('data/data' + str(ind) + '.txt','w') as f:
        #         np.savetxt(f, matrix, fmt='%g', delimiter=' ')
        #     if ind%1 == 0:
        #         print(str(ind) + "/" + str(x_size))  
        
        for ind, matrix in enumerate(M):
            h5f = h5py.File('../data/data'+str(ind)+'.h5', 'w')
            h5f.create_dataset('dataset', data=matrix)
            print(str(ind) + "/" + str(cls.x_size))  
        
        print("generated in " + str(time.time() - start_time))
        
        return tasks
