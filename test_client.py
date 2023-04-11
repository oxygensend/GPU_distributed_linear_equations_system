import websocket
import time
import numpy as np
import threading
import json
import h5py
import socket

to_solve = []

def on_message(ws, message):
    global to_solve
    data = json.loads(message)
    print(data)
    if data['type'] == 3:
        to_solve = data['to_solve'].copy()

def on_error(ws, error):
    print(error)

def on_close(ws, close_status_code, close_msg):
    print("### closed ###")

def on_open(ws):
    ws.send(json.dumps({'type': 0, 'status_port': 65432}))
    print("Connected")


if __name__ == "__main__":
    websocket.enableTrace(True)
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.bind(('127.0.0.1', 5589));
    sock.connect(('127.0.0.1', 1111))
    
    ws = websocket.WebSocketApp("ws://localhost:1111",
                            on_open=on_open,
                            on_message=on_message,
                            on_error=on_error,
                            on_close=on_close,
                            socket=sock)
    wst = threading.Thread(target=ws.run_forever)
    wst.daemon = True
    wst.start()

    while True:
        print(to_solve)
        if len(to_solve) > 0:
            for task in to_solve:
                hf = h5py.File('data/data' + str(task) + '.h5', 'r')
                matrix = np.array(hf["dataset"][:])

                ws.send(json.dumps({'type': 4, 'task': task, 'solution': np.linalg.det(matrix)}))
                to_solve.remove(task)

        else:
            time.sleep(0.1)