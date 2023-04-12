import socket
from time import sleep

workers = []
SLEEP_TIME_S = 1    


def check_workers():
    """
    Method that check if workers are alive and change their status if not.
    """
    global workers
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
                # TODO if is not alive pass his job to another process

        sleep(SLEEP_TIME_S)


