import websocket
import _thread
import time
import rel
import numpy as np
import threading
import json
import h5py

to_solve = []

def on_message(ws, message):
    global to_solve
    data = json.loads(message)
    if data['type'] == 3:
        to_solve = data['to_solve'].copy()

def on_error(ws, error):
    print(error)

def on_close(ws, close_status_code, close_msg):
    print("### closed ###")

def on_open(ws):
    ws.send(json.dumps({'type': 0}))
    print("Connected")


if __name__ == "__main__":
    websocket.enableTrace(True)
    ws = websocket.WebSocketApp("ws://localhost:1111",
                              on_open=on_open,
                              on_message=on_message,
                              on_error=on_error,
                              on_close=on_close)
    wst = threading.Thread(target=ws.run_forever)
    wst.daemon = True
    wst.start()

    while True:
        print(to_solve)
        if len(to_solve) > 0:
            for task in to_solve:
                hf = h5py.File('data/data' + str(task) + '.h5', 'r')
                matrix = np.array(hf["dataset"][:])

                print(matrix)

                ws.send(json.dumps({'type': 4, 'task': task, 'solution': np.linalg.det(matrix)}))
                to_solve.remove(task)

        else:
            time.sleep(0.1)