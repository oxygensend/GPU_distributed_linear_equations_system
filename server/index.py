import threading
from src import app, check_workers



if __name__ == '__main__':
    threading.Thread(target=check_workers).start()
    app.run(host='0.0.0.0', port=3000)