#include "../include_test/common.h"
#include "../include_test/task.h"

int LinkCapa = 128*1000*1000; //unit is Byte

// map<char link, int time_free> link_state; time_free stand for the time 
map<char, int> Link_state= { {"1_0", 0},
						{"0_1", 0},
						{"2_0", 0},
						{"0_2", 0},
						{"3_0", 0},
						{"0_3", 0},
						{"4_0", 0},
						{"0_4", 0} };     


//vector<struct Sched_task_elem> traffic_sched(vector<struct Task_elem> task, int time_start) { 
vector<struct Sched_task_elem> traffic_sched(vector<struct Task_elem> task) {
	// time_start=0: the absolute starting time for the current schedule cycle 
	vector<struct Sched_task_elem> sched;
	for(int i; i < task.size(); i++) {
		struct Sched_task_elem tmp_sched_task_elem;
		for(int k = 0; k < task[i].flow_num; k++){
			string link_id = to_str(task[i].flows[k].src) + "_" + to_str(task[i].flows[k].dst);			
		//	if(task[i].flows[k].size / LinkCapa + (Link_state[link_id] - time_start) > task[i].flows[k].ddl) {
			if(task[i].flows[k].size / LinkCapa + (Link_state[link_id] - 0) > task[i].flows[k].ddl) {  
				//if can't complete this task!
				tmp_sched_task_elem.Is_discard = YES;
				break;
			}
			else {
				tmp_sched_task_elem.Is_discard = NO;
				tmp_sched_task_elem.start_time.push_back(Link_state[link_id]);
				tmp_sched_task_elem.duration_len.push_back(task[i].flows[k].size / LinkCapa);
				tmp_sched_task_elem.task_id = i;
				tmp_sched_task_elem.flow_num ++;

				Link_state[link_id] += task[i].flows[k].size / LinkCapa;
			}
		}
		if(YES == sched[i].Is_discard) {
			continue;
		}
		else {
			sched.push_back(tmp_sched_task_elem);
		}
	}
	return sched;
}
 