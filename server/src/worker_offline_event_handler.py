from typing import Any
from  .worker_offline_event import WorkerOfflineEvent
from .workers import get_all_online_workers, workers
import json
from .solver_server import tasks


class WorkerOfflineEventHandler:
    global workers;
    global tasks;

    # na razie taski rozdzielane sa do wszystkich komputerow online po rowno, do dopracowania aby sprawdzic ktory komputer ma najmniej = jest najszybszy
    def __call__(self, event: WorkerOfflineEvent) -> Any:
        """Event handler responsible for error validation. If one of the workers is down all task should be passed to others machines."""

        print("Worker {} is offline. Handling event...".format(event.worker))

        if "to_solve" not in event.worker:
            print("Worker dont have any tasks");
            if event.worker in workers:
                workers.remove(event.worker)
            return

        workers_online = get_all_online_workers()

        # remained taks
        to_solve = self._get_tasks_to_solve(event.worker)

        # data partitioning
        num_workers = len(workers)
        num_tasks = len(to_solve)
        tasks_per_worker = num_tasks // num_workers
        extra_tasks = num_tasks % num_workers

        # resend data to others clients
        start_idx = 0
        for i, worker in enumerate(workers_online):
            end_idx = start_idx + tasks_per_worker
            if i < extra_tasks:
                end_idx += 1

            partition = to_solve[start_idx:end_idx]

            self._resend_task(partition, worker)

            worker["to_solve"] = worker["to_solve"] + partition
            start_idx = end_idx
        

        # remove worker at the end
        workers.remove(event.worker)

    def _get_tasks_to_solve(self, worker):
        """ Find tasks that should be resend"""
        to_solve = []
        for i, task in enumerate(tasks):
            if task[2] is None and task[0] == 1 and task[1] == worker['client']:
                to_solve.append(i);
        return to_solve

    def _resend_task(self,partition, worker):
        """Resend task to client"""

        # change tasks' owner
        for task in partition:
            tasks[task][1] = worker['client'];

        worker['client'].send_message(json.dumps({
            'type': 3,
            'to_solve': partition
        }))

        worker["to_solve"] = worker["to_solve"] + partition

