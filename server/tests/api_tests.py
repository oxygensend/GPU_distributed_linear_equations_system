import json
import pytest
from src import app

@pytest.fixture
def client():
    with app.test_client() as client:
        yield client

def test_register(client):
    """
    Test for registering a new worker
    """
    response = client.post('/register', json={'port': '8000'})

    assert response.status_code == 201
    assert json.loads(response.data) == {'status': 'ok'}

def test_register_existing_worker(client):
    """
    Test for trying to register an existing worker
    """
    client.post('/register', json={'port': '8000'})
    response = client.post('/register', json={'port': '8000'})

    assert response.status_code == 403
    assert json.loads(response.data) == {'status': 'worker is already registered.'}

def test_register_invalid_port(client):
    """
    Test for trying to register a worker with an invalid port number
    """
    response = client.post('/register', json={'port': 'invalid'})

    assert response.status_code == 400
    assert json.loads(response.data) == {'status': 'port must be an integer'}

def test_remove(client):
    """
    Test for removing a worker
    """
    client.post('/register', json={'port': '8000'})
    response = client.delete('/remove', json={'port': '8000'})

    assert response.status_code == 204

def test_remove_nonexistent_worker(client):
    """
    Test for trying to remove a nonexistent worker
    """
    response = client.delete('/remove', json={'port': '8000'})

    assert response.status_code == 404
    assert json.loads(response.data) == {'status': 'provided worker is not found'}
