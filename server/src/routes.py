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
        port = body['port']

    worker = {
        'ip': ip,
        'port': port,
        'status': 'online'
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
        port = body['port']

    for worker in workers:
        if worker['ip'] == ip and worker['port'] == port:
            workers.remove(worker)
            break
    else:
        return {'status': 'provided worker is not found'}, 404

    return "Success", 204



def validatePort(body):
    if 'port' not in body:
        return {'status': 'port not found'}, 400

    try:
        int(body['port'])
    except ValueError:
        return {'status': 'port must be an integer'}, 400

    return None