import numpy as np
import json 
from simple_websocket_server import WebSocketServer, WebSocket
import time
import h5py

W = 1
M = []
clients = []
started = False

def generate_matrices():
    global W
    global M

    start_time = time.time()

    x_min = -10
    x_max = 10
    a_min = -10
    a_max = 10

    x_size = 1000

    A = np.random.rand(x_size, x_size)*(a_max-a_min)+a_min
    X_old = np.random.rand(x_size, 1)*(x_max-x_min)+x_min
    B = np.matmul(A, X_old)

    W = np.linalg.det(A)

    X = np.empty([x_size, 1])
    
    for i in range(x_size):
        tmp = np.copy(A)
        for k in range(x_size):
            tmp[k][i] = B[k][0]
        M.append(tmp)

    
    for ind, matrix in enumerate(M):
        with open('data/data' + str(ind) + '.txt','w') as f:
            np.savetxt(f, matrix, fmt='%g', delimiter=' ')
        if ind%1 == 0:
            print(str(ind) + "/" + str(x_size))  
    
    for ind, matrix in enumerate(M):
        h5f = h5py.File('data/data'+str(ind)+'.h5', 'w')
        h5f.create_dataset('dataset_1', data=matrix)
        print(str(ind) + "/" + str(x_size))  
    
    print("generated in " + str(time.time() - start_time))

    '''
    for ind, matrix in enumerate(M):
        np.linalg.det(matrix)
    print("generated in " + str(time.time() - start_time))    
    '''

class SolverServer(WebSocket):
    def handle(self):
        '''
        0 - login gpu
        1 - generate data
        2 - start computing
        '''

        data = json.loads(self.data)

        if data['type'] == 0:
            clients.append(self)
        
        if data['type'] == 1:
            generate_matrices()

    def connected(self):
        print("connected")

    def handle_close(self):
        clients.remove(self)

generate_matrices()
server = WebSocketServer('', 1111, SolverServer)
server.serve_forever()



