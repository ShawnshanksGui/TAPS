#include "../include_test/mySocket.h"
#include "../include_test/common.h"
#include "../include_test/task.h"

#include <chrono>
#include <string>
#include <vector>
#include <thread>

#include "unistd.h"
#include "sys/types.h"
#include "sys/syscall.h"  //for obtaining thread_id

using namespace std;

void fun() {
	
}

//SDN controller side
int main() {
//divide the whole controller's program into 3 parts 
//==================================================================
//(1)  At the start, generate task
//	task = generate_task();
//	schedule_result = schedule_task(task);
//==================================================================

//fisrtly test 1 time slice	
	for(int i = 0; i < 1; i++) {
//Responsor_for_controller thread: 
//(2)  Receive requests of schedule and reply schedule result
		thread td_responsor;
		td_responsor = thread(fun);
		td_responsor.detach();

//==================================================================
//(3)  at the last, Collect performance feedback!
//..................................................................
//==================================================================
	}

	while(1) {
		sleep(5);
	}

	return 0;
}