import numpy as np
import json 
from simple_websocket_server import WebSocketServer, WebSocket
import time
import h5py
from .problem_generator import ProblemGenerator
from .workers import workers

tasks = []
batch_size = 10
x_size = 100
task_marker = 0

class SolverServer(WebSocket):
    def handle(self):
        global task_marker
        global tasks
        global workers
        '''
        0 - login gpu
        1 - generate data
        2 - start computing
        3 - task list for gpu
        4 - solution receive
        '''

        data = json.loads(self.data)

        if data['type'] == 0:
            id, port = self.address
            print(id)
            for  worker in workers:
                if worker['status_port'] == data['status_port']:
                    worker['client'] = self
                    worker['status'] = 'online'
                    worker['app_port'] = port
                

        
        if data['type'] == 1:
            tasks = ProblemGenerator.generate_matrices()

        if data['type'] == 2:
            for worker in workers:

                to_solve = []
                safety = 0
                for i in range(ProblemGenerator.batch_size):
                    while True:
                        '''
                        task: [state, solver, result]
                        task state:
                        0 - not send to solver
                        1 - waiting for result
                        2 - solved
                        '''
                        if(tasks[task_marker][0] == 0):
                            to_solve.append(task_marker)
                            print(to_solve)
                            tasks[task_marker][1] = self
                            tasks[task_marker][0] = 1
                            task_marker += 1
                            if task_marker == ProblemGenerator.x_size:
                                task_marker = 0
                            break
                            
                        safety += 1    
                        if safety > ProblemGenerator.x_size:
                            break
                worker['client'].send_message(json.dumps({
                    'type': 3,
                    'to_solve': to_solve
                }))

        if data['type'] == 4:
            tasks[data['task']][0] = 2
            tasks[data['task']][2] = data['solution']
            print(data)

    def connected(self):
        print("connected")


