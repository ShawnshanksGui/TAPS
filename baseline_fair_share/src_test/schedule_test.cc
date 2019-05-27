//#include "../include_test/mySocket.h"
#include "../include_test/common.h"
//#include "../include_test/task.h"
#include "../include_test/flow_schedule.h"

#include <string>
#include <vector>
#include <map>

using namespace std;

//global variables which is visible for every thread
vector<struct Task_elem> tasks;
vector<struct Sched_task_elem> schedule_result;


//SDN controller side
int main() {
//divide the whole controller's program into 3 parts 
//==================================================================
//(1)  At the start, generate task
	tasks = generate_task();
	schedule_result = task_schedule(tasks);
//==================================================================
  cout << "************************" << endl;
  cout << "get flow scheule result!" << endl;
  cout << "************************" << endl;


	return 0;
}