import socket
from time import sleep
import queue
from .worker_offline_event import WorkerOfflineEvent

workers = []
SLEEP_TIME_S = 1    
event_queue = queue.Queue();



def check_workers():
    """
    Method that check if workers are alive and change their status if not.
    """
    global workers
    global event_queue
    while True:
        for worker in workers:
            try:
                s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                s.settimeout(1)
                s.connect((worker['ip'], worker['status_port']))
                s.sendall(b'ping')
                s.close()
                worker['status'] = 'online'
                print('Stable connection with ', worker)
            except Exception as e:
                worker['status'] = 'offline'
                print(e, worker)
                # send event if worker is offline
                event_queue.put(WorkerOfflineEvent({"a": "a"}))


        sleep(SLEEP_TIME_S)



def check_if_worker_exists_by_status_port_and_ip(port: int, ip: str):
    for  key,worker in enumerate(workers):
        if worker['status_port'] == port and worker['ip'] == ip:
            return key;
        else:
            return None
        
def check_if_worker_exists_by_app_port_and_ip(port: int, ip: str):
    for  key,worker in enumerate(workers):
        if worker['app_port'] == port and worker['ip'] == ip:
            return key;
        else:
            return None

def get_all_online_workers() -> dict:
        return {key: worker for key, worker in workers if worker["status_port"] == "online"}