import numpy as np
import json 
from simple_websocket_server import  WebSocket
import math
from .problem_generator import ProblemGenerator
from .workers import workers
import queue

event_queue = queue.Queue()
tasks = []
solution = []
main_matrix_w = 0
workers = []

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
            global workers
            print('device logged')
            workers.append(self)

        if data['type'] == 1:
            tasks = ProblemGenerator.generate_matrices()
            print(workers)

        if data['type'] == 2:
            task_marker = 0
            initial_batch = math.ceil((len(tasks))/len(workers))
            print(workers, initial_batch) 
            for worker in workers:
                to_solve = []
                for i in range(initial_batch):
                        '''
                        task: [state, solver, result]
                        task state:
                        0 - not send to solver
                        1 - waiting for result
                        2 - solved
                        '''
                        if(tasks[task_marker][0] == 0):
                            to_solve.append(task_marker)
                            tasks[task_marker][1] = self
                            tasks[task_marker][0] = 1
                            tasks[task_marker][2] = None
                            task_marker += 1
                            if(task_marker == ProblemGenerator.x_size):
                                break
                worker.to_solvee = to_solve
                worker.send_message(json.dumps({
                    'type': 3,
                    'to_solve': to_solve
                }))
           
           

        if data['type'] == 4:
            print(data)
            #print(self.to_solvee)
            tasks[data['task']][0] = 2
            tasks[data['task']][2] = data['solution']
            if data['task'] == 0:
                main_matrix_w = data['solution']
            end = True
            for task in tasks:
                if task[0] != 2:
                    end = False
                    break

            if end:
                print("CALCULATIONS COMPLETED")
                print([task[2]/main_matrix_w for task in tasks[1:]])
                

        # if machine finished all tasks this type is sent, for know it wait for all machines and print the results.
        # but if we want to send data in small batches we can sent here data to cpus with faster gpu
        if data['type'] == 5:
            tam = 0
            print("ask for more")
            to_solve = []
            for tid, task in enumerate(tasks):
                if task[0] == 0:
                    tam += 1
                    tasks[tid][0] = 1
                    #task[tid][2] = None
                    tasks[tid][1] = self
                    to_solve.append(tid)
                    if(tam == ProblemGenerator.batch_size):
                        break
            print(to_solve)
            self.send_message(json.dumps({
                'type': 3,
                'to_solve': to_solve
            }))


    def connected(self):
        print("connected")

    def handle_close(self):
        for tid in self.to_solvee:
            if tasks[tid][0] != 2:
                tasks[tid][0] = 0
        print("disconnected!!!!")


