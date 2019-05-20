#include <vector>

using namespace std;

struct Flow_elem{
	int src;
	int dst;
	int flow_id;
	int flow_size;
//	int ex_tran_time;    //expected trans time
	int ddl;      //deadline
};

struct Task_elem{
	int task_id;
	int src_id;   //the server which generate task and send data 
	int flow_num; //
	vector<Flow_elem> flows;
};

struct Sched_task_elem{
	int Is_discard;
	int task_id;
	int src_id;  //the server which generate task and send data 
	int flow_num;
	vector<int> start_time;
	vector<int> duration_len;
};


/*
map<char, int> link_state= { {"1_0", 0},
						{"0_1", 0},
						{"2_0", 0},
						{"0_2", 0},
						{"3_0", 0},
						{"0_3", 0},
						{"4_0", 0},
						{"0_4", 0} };     // map<char link, int time_free> link_state;
*/