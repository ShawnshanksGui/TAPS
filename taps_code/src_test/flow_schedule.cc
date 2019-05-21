#include "../include_test/common.h"
#include "../include_test/flow_schedule.h"

#include <map>

using namespace std;

double LinkCapa = 1.0; // capacity of each link, unit is Gigabyte

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
			if(task[i].flows[k].flow_size / LinkCapa + (Link_state[out_link] - 0) > task[i].flows[k].ddl ||
			   task[i].flows[k].flow_size / LinkCapa + (Link_state[in_link]  - 0) > task[i].flows[k].ddl) {  
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
  
  int tmp = 0;
  
  for (int i = 0; i < 4; i++) {
    struct Task_elem tmp_task;
    for (int k = 0; k < 2; k++) {
      struct Flow_elem tmp_flow;
      tmp_flow.src = i;
      
      while ((tmp = rand()%4) == i); // tmp can't be same as i/src_id
      tmp_flow.dst = tmp;
      
      tmp_flow.flow_id = k;
      tmp_flow.flow_size = 1.0 + rand()%2;  //Gigabyte
      tmp_flow.ddl = 1.0 + rand() % 5;      
      
      tmp_task.flows.push_back(tmp_flow);
    }
    tmp_task.task_id = i;
    tmp_task.src = i;
    tmp_task.flow_num = 2; //set flow_num as 2
    _tasks.push_back(tmp_task);
  }
  
  return _tasks; 
}
