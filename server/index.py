import threading
from src import app, check_workers, SolverServer
from simple_websocket_server import WebSocketServer, WebSocket



if __name__ == '__main__':
    server = WebSocketServer('', 1111, SolverServer)
    threading.Thread(target=server.serve_forever).start()
    threading.Thread(target=check_workers).start()
    app.run(host='0.0.0.0', port=3000)