# File created for testing purpuse

import socket

HOST = "127.0.0.1"  # ip komputera
PORT = 65432  # port jakis woolny


while True:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.bind((HOST, PORT))
        s.listen()
        conn, addr = s.accept()
        with conn:
            print(f"Connected by {addr}")
