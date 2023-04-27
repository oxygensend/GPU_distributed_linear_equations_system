import numpy as np
import json 
from simple_websocket_server import WebSocketServer, WebSocket
import time
import h5py
from .problem_generator import ProblemGenerator
from .workers import workers, check_if_worker_exists_by_app_port_and_ip, check_if_worker_exists_by_status_port_and_ip

tasks = []
task_marker = 0
solution = []
main_matrix_w = 0

class SolverServer(WebSocket):
    def handle(self):
        global task_marker
        global tasks
        global workers
        global solution
        global main_matrix_w
        '''
        0 - login gpu
        1 - generate data
        2 - start computing
        3 - task list for gpu
        4 - solution receive
        '''

        data = json.loads(self.data)

        if data['type'] == 0:
            ip, port = self.address
            worker_key = check_if_worker_exists_by_status_port_and_ip(data['status_port'], ip)
            if worker_key is not None:
                    workers[worker_key]['client'] = self
                    workers[worker_key]['app_port'] = port
                

        
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
                            tasks[task_marker][2] = None
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
            print(data)
            tasks[data['task']][0] = 2
            tasks[data['task']][2] = data['solution']
            if data['task'] == 0:
                main_matrix_w = data['solution']


        # if machine finished all tasks this type is sent, for know it wait for all machines and print the results.
        # but if we want to send data in small batches we can sent here data to cpus with faster gpu
        if data['type'] == 5:
            ip, port = self.address
            worker_key = check_if_worker_exists_by_app_port_and_ip(port, ip)
            if worker_key is  None:
                raise Exception("WHO IS THIS!!!!!!!!!!!! HANDEHOFFFFFFF")

            # if not every task is solved leave
            for task in tasks:
                if len(task) < 3 or task[2] is None:
                    return
 
            # result
            print([task[2]/main_matrix_w for task in tasks[1:]])


    def connected(self):
        print("connected")


