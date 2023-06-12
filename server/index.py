import threading
from src import app,  SolverServer
from simple_websocket_server import WebSocketServer, WebSocket



if __name__ == '__main__':
    server = WebSocketServer('0.0.0.0', 1111, SolverServer)
    threading.Thread(target=server.serve_forever).start()
    app.run(host='0.0.0.0', port=3000)
