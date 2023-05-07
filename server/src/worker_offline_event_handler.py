from typing import Any
from  .worker_offline_event import WorkerOfflineEvent
from .workers import get_all_online_workers, workers
import json


class WorkerOfflineEventHandler:
    global workers;

    # na razie taski rozdzielane sa do wszystkich komputerow online po rowno, do dopracowania aby sprawdzic ktory komputer ma najmniej = jest najszybszy
    # powinnismy jeszcze w tablicy task podmienic klienta przypsianego do taska na wypadek dalszych turbulencji
    def __call__(self, event: WorkerOfflineEvent) -> Any:
        """Event handler responsible for error validation. If one of the workers is down all task should be passed to others machines."""

        print("Worker {} is offline. Handling event...".format(event.worker))

        if "to_solve" not in event.worker:
            print("Worker dont have any tasks");
            if event.worker in workers:
                workers.remove(event.worker)
            return

        to_solve = event.worker["to_solve"]

        workers_online = get_all_online_workers()
        num_workers = len(workers)
        num_tasks = len(to_solve)
        tasks_per_worker = num_tasks // num_workers
        extra_tasks = num_tasks % num_workers

        start_idx = 0
        for i, worker in enumerate(workers_online):
            end_idx = start_idx + tasks_per_worker
            if i < extra_tasks:
                end_idx += 1

            tasks = to_solve[start_idx:end_idx]
            worker['client'].send_message(json.dumps({
                'type': 3,
                'to_solve': tasks
            }))

            worker["to_solve"] = worker["to_solve"] + tasks
            start_idx = end_idx
        

        # remove worker at the end
        workers.remove(event.worker)

