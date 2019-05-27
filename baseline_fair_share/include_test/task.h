#include <vector>

using namespace std;

struct Flow_elem{
	int src;
	int dst;
	int flow_id;
	double arrival_time;
	double flow_size;
//	int ex_tran_time;    //expected trans time
	double ddl;      //deadline
};

struct Task_elem{
	int task_id;
	int src;   //the server which generate task and send data 
	double arrival_time;
	double ddl;
	int flow_num; //
	vector<Flow_elem> flows;
};

struct Sched_task_elem{
	int Is_discard;
	int task_id;
	int src_id;  //the server which generate task and send data 
	int flow_num;
	vector<double> start_time;
	vector<double> duration_len;
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
