


int main () {
	
	for(; ;) {
		vector<struct Task_elem> task;
		vector<struct Sched_task_elem> sched;

		task = task_generator();
		sched = traffic_sched(task);

		distri_sched(sched, task);

	}

	return 0;
}