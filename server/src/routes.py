from flask import request, Flask
from .workers import workers

app = Flask(__name__)

@app.route('/register', methods=['POST'])
def register():
    """
    Route for appending new workers 
    """
    global workers
    body = request.get_json()
    ip = request.remote_addr

    violations = validatePort(body)
    if violations is not None:
        return violations
    else:
        status_port = body['status_port']

    worker = {
        'ip': ip,
        'status_port': status_port,
        'app_port': None,
        'status': 'online',
        'client': None
    }

    if worker not in workers:
        workers.append(worker)
        return {'status': 'ok'}, 201
    else:
        return {'status': 'worker is already registered.'}, 403

@app.route('/remove', methods=['DELETE'])
def remove():
    """
    Route for removing worker from list
    """
    global workers
    body = request.get_json()
    ip = request.remote_addr

    violations = validatePort(body)
    if violations is not None:
        return violations
    else:
        port = body['status_port']

    for worker in workers:
        if worker['ip'] == ip and worker['status_port'] == port:
            workers.remove(worker)
            break
    else:
        return {'status': 'provided worker is not found'}, 404

    return "Success", 204



def validatePort(body):
    if 'status_port' not in body:
        return {'status': 'port not found'}, 400

    try:
        int(body['status_port'])
    except ValueError:
        return {'status': 'port must be an integer'}, 400

    return None