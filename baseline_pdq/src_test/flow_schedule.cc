#include "../include_test/common.h"
#include "../include_test/flow_schedule.h"
#include "../include_test/control.h"

#include <map>

using namespace std;

double LinkCapa = 1.0*1000*1000; // capacity of each link, unit is KB

// map<string link, int time_free> link_state ---> stand for when link is free(not occupied by any flow)
// Number "0", "1", "2", "3" stand for server 0, 1, 2, 3, respectively;
// And "s" stand for the switch;
//for example, {"0_s", 0} imply that the link from server 0 to switch  will be time 0;
map<string, double> Link_state = { 
						{"0_s", 0.0},
						{"s_0", 0.0},
						{"1_s", 0.0},
						{"s_1", 0.0},
						{"2_s", 0.0},
						{"s_2", 0.0},
						{"3_s", 0.0},
						{"s_3", 0.0} };     


//vector<struct Sched_task_elem> traffic_sched(vector<struct Task_elem> task, int time_start) { 
vector<struct Sched_task_elem> task_schedule(vector<struct Task_elem> &task) {
	// time_start=0: the absolute starting time for the current schedule cycle 
	vector<struct Sched_task_elem> sched;
	
	for(int i = 0; i < task.size(); i++) {
		struct Sched_task_elem tmp_sched_task_elem;
		for(int k = 0; k < task[i].flow_num; k++){
			string out_link = to_string(task[i].flows[k].src) + "_" + "s";  //the link from src server to switch
			string in_link  = "s";
      		in_link = in_link + "_" + to_string(task[i].flows[k].dst);  //the link from switch to dst server
		//	if(task[i].flows[k].flow_size / LinkCapa + (Link_state[link_id] - time_start) > task[i].flows[k].ddl) {
			if(task[i].flows[k].flow_size / LinkCapa * 1000 + (Link_state[out_link] - 0) > task[i].flows[k].ddl ||
			   task[i].flows[k].flow_size / LinkCapa * 1000 + (Link_state[in_link]  - 0) > task[i].flows[k].ddl) {  
				//if can't complete this task!
				tmp_sched_task_elem.Is_discard = YES;
				break;
			}
			else {
				tmp_sched_task_elem.Is_discard = NO;
				//flow starting moment
        int start_time = (Link_state[out_link] > Link_state[in_link]) ? Link_state[out_link] : Link_state[in_link];
				tmp_sched_task_elem.start_time.push_back(start_time);
				//flow durtion time length
				tmp_sched_task_elem.duration_len.push_back(task[i].flows[k].flow_size / LinkCapa);
        tmp_sched_task_elem.src_id = i;
				tmp_sched_task_elem.task_id = i;
				tmp_sched_task_elem.flow_num = task[i].flow_num;
// modify the Link_state
				Link_state[out_link] += task[i].flows[k].flow_size / LinkCapa;
				Link_state[in_link]  += task[i].flows[k].flow_size / LinkCapa;
			}
		}

		sched.push_back(tmp_sched_task_elem);
	}

	return sched;
}
 
vector<struct Task_elem> generate_task() {
  	vector<struct Task_elem> _tasks; 
 	srand((unsigned)time(NULL));
  
	ofstream task_file_out;
	task_file_out.open("file_tasks_total.txt");
/*	
	vector<ofstream> task_each_server;
	for(int i = 0; i < SERVER_NUM; i++) {
		ofstream tmp_file; 
		string file_name = "file_task_server";
		file_name = file_name + to_string(i) + ".txt";
		tmp_file.open(file_name);
		task_each_server.push_back(tmp_file);
	}
*/
  	int tmp = 0;  
  	for (int i = 0; i < SERVER_NUM*TASK_NUM; i++) {
		double task_ddl = 0.0;
		task_ddl = 30 + (rand()%21);
		
		task_file_out << "task " << i << ": " << "task_ddl:" << task_ddl << endl;
		task_file_out << "flow_id: arrival_time(ms)  flow_size(KB)  deadline(ms)" << endl;
/*
		task_each_server[i%4] << "task " << i/4 << ":" << endl;
        task_each_server[i%4] << "flow_id: arrival_time(ms)  flow_size(KB)  deadline(ms)" << endl;
*/
    	struct Task_elem tmp_task;
    	for (int k = 0; k < FLOW_NUM; k++) {
      		struct Flow_elem tmp_flow;
      		//(feature 1)
	 		tmp_flow.src = i%4;
      		//(feature 2)
      		while ((tmp = rand()%4) == i%4); // tmp can't be same as i/src_id
      		tmp_flow.dst = tmp;
      		//(feature 3)
      		tmp_flow.flow_id = k;
			if(i < 2) {tmp_flow.arrival_time = rand()%50;}
			else {tmp_flow.arrival_time = 25*(i-2)+rand()%50;}
 			//(feature 4)
      		tmp_flow.flow_size = 180.0 + (rand()%5)*10.0;  //KB
      		//(feature 5)
			double tmp_ddl = 30.0 + (rand() % 5)*5.0;        //ms
      		if(tmp_ddl < task_ddl) {tmp_flow.ddl = tmp_ddl;}
			else {tmp_flow.ddl = task_ddl;}

      		tmp_task.flows.push_back(tmp_flow);
			
			task_file_out << "flow " << k << ":" << "  " << tmp_flow.arrival_time;
			task_file_out << "  " << tmp_flow.flow_size << "  " << tmp_flow.ddl;
			task_file_out << endl;
/*
            task_each_server[i%4] << "flow " << k << ":" << "  " << tmp_flow.arrival_time;
            task_each_server[i%4] << "  " << tmp_flow.flow_size << "  " << tmp_flow.ddl;
            task_each_server[i%4] << endl;
*/
    	}
    	tmp_task.task_id = i/4;
    	tmp_task.src = i%4;
    	tmp_task.flow_num = FLOW_NUM; //set flow_num as 2
		tmp_task.ddl = task_ddl;
    	_tasks.push_back(tmp_task);
  	}
/*
	for(int i = 0; i < SERVER_NUM; i++) {
		task_each_server[i].close();
	}
*/ 
  	return _tasks; 
}
