workers = []


def check_if_worker_exists_by_status_port_and_ip(port: int, ip: str, workers: list):
    for  key,worker in enumerate(workers):
        print(worker)
        if worker['status_port'] == port and worker['ip'] == ip:
            return key;
    return None
        
def check_if_worker_exists_by_app_port_and_ip(port: int, ip: str):
    for  key,worker in enumerate(workers):
        if worker['app_port'] == port and worker['ip'] == ip:
            return key;
    return None

def get_all_online_workers() -> dict:
        return {key: worker for key, worker in workers if worker["status_port"] == "online"}
