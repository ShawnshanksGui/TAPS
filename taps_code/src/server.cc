
namespace std;



//server side
int main() {
	for(; ;) {
		task = task_generator();
		sched = request_process(task);

		vector<vector<Transmitter>> server_socket;
		vector<vector<thread>> flow_worker;

		for (int i = 0; i < task.task_num; i++) {
			if(YES != task.Is_discard) {
				//prepare socket for each flow				
				vector<Transmitter> tmp_server_socket(task[i].flow_num);
				for (int f; f < task[i].flow_num; f++) {
					tmp_server_socket[f].transmitter_new_tcp("192.168.15."task[i].flows[f].src, );
				}
				server_socket.push_back(tmp_server_socket);

				//prepare thread for each flow
				vector<thread> tmp_worker;
				for (int k = 0; k < task[i].flow_num; k++) {
					tmp_worker.push_back(thread(_flow, ref(server_socket[i][k]), ref(task[i].flows[k]), ref(sched[i])));
				}
				flow_worker.push_back(tmp_worker);
			}
			else {
				/*......*/
				continue;
			}
		}	
	}

	return 0;
}



void _flow(Transmitter server_socket, struct Flow_elem flow, struct Sched_task_elem sched) {


}