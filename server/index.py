import threading
from src import app, check_workers, SolverServer, WorkerOfflineEventHandler, event_queue
from simple_websocket_server import WebSocketServer, WebSocket



if __name__ == '__main__':
    server = WebSocketServer('0.0.0.0', 1111, SolverServer)
    threading.Thread(target=server.serve_forever).start()
    threading.Thread(target=check_workers).start()
    threading.Thread(target=lambda q: [(WorkerOfflineEventHandler())(evt) for evt in iter(q.get, None)], args=(event_queue,)).start()

    app.run(host='0.0.0.0', port=3000)