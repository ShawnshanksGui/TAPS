#include "../include_test/mySocket.h"
#include "../include_test/common.h"
#include "../include_test/control.h"

#include "../include_test/flow_schedule.h"

#include <chrono>
#include <string>
#include <vector>
#include <thread>

#include "unistd.h"
#include "sys/types.h"
#include "sys/syscall.h"  //for obtaining thread_id

using namespace std;

#define READY 1  // server can enter into next step: comunicate with other server
int CNT_SERVER_STATE[SERVER_NUM] = {0};

//char ip_controller[] = "192.168.15.33";  //IP for SDN controller
char ip_controller[] = "12.12.11.18";  //IP for SDN controller

char * ip[4] = {"192.168.15.33", //IP for server 0
				"192.168.15.33",  //IP for server 1
				"192.168.15.43",  //IP for server 2
				"192.168.15.33"};  //IP for server 3

char ip_self[] = "192.168.15.33";

char ip_dst[] = "192.168.15.33";

char port_listen_self[] = "5000"; //controller's listen port

char port_connect_self[] = "10000"; //

char port_connect_dst[] = "6000"; //G8's listening port


void responsor_for_controller(); //for listening request flow
//void reply_schedule(int socket, vector<struct Sched_task_elem> _schedule_result);
void reply_schedule(int socket, vector<struct Task_elem> _tasks);
int encaps_info_pkt(char *info_pkt, int original_task);
//void encaps_schedule_pkt(char *sched_pkt, struct Sched_task_elem &schedule_task);

//global variables which is visible for every thread
vector<struct Task_elem> tasks;
vector<struct Sched_task_elem> schedule_result;


//SDN controller side
int main() {
//divide the whole controller's program into 3 parts 
//==================================================================
//(1)  At the start, generate task
//	tasks = generate_task();
//	schedule_result = task_schedule(tasks);
//==================================================================
	cout << "************************" << endl;
	cout << "generate tasks, and get flow schedule result!" << endl;
	cout << "************************" << endl;
  
//fisrtly test 1 time slice	
	for(int i = 0; i < 1; i++) {
//Responsor_for_controller thread: 
//(2)  Receive requests of schedule and reply schedule result
		thread td_responsor;
		td_responsor = thread(responsor_for_controller);
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

void responsor_for_controller() {
	printf("the thread of responsor/listen id is %ld\n", syscall(SYS_gettid));

	int sock_connect[100];
//	int server_id
	Transmitter server_socket;
	server_socket.transmitter_new_tcp_responsor(ip_controller, port_listen_self);

	vector<thread> responsor_threads;

	for(int i = 0; i < SERVER_NUM; i++) {
		sock_connect[i] = server_socket.Accept();
//		responsor_threads.push_back(thread(reply_schedule, sock_connect[i], 
//										   schedule_result));
		responsor_threads.push_back(thread(reply_schedule, sock_connect[i],
										   tasks));
		responsor_threads.back().detach();
	}

//	while(1)
//	sleep(2);
//	close(server_socket.listen_sock_fd);
}


//void reply_schedule(int socket, vector<struct Sched_task_elem> _schedule_result) {
void reply_schedule(int socket, vector<struct Task_elem> _tasks) {
	char recv_request_pkt[2] = {'\0'};

// information about task num and corresponding flow num
// the maximum num of tasks is approximately 240 
	char info_pkt[1000] = {'\0'};
//	char task_pkt[100][1000] = {{'\0'}};
//	char info_pkt[1000] = {"the info about packet of schedule result"};

	int server_id;
	int cnt_active_task = 0; // the total task num need to complete in this cycle 
	vector<int> flow_num_vec;

	Recv_tcp(socket, recv_request_pkt, 2);
	server_id =  recv_request_pkt[0]; // identify server from which the request is  
/*
//prepare and reply packet about info(task num...)
	for(int i = 0; i < _schedule_result.size(), i++) {
		if(NO == _schedule_result[i].Is_discard && server_id == _schedule_result[i].src_id) {
			cnt_active_task += 1; 
			flow_num_vec.push_back(task[i].flow_num);
		}
	}
*/
//	encaps_info_pkt(info_pkt, task.size(), cnt_active_task, flow_num_vec); 
	CNT_SERVER_STATE[server_id] = READY;
	while(1) {
		int state_server = READY;
		for(int i = 0; i < SERVER_NUM; i++) {state_server *= CNT_SERVER_STATE[i];}

		if(state_server == READY) { break;}
	}

	int cnt_active = encaps_info_pkt(info_pkt, TASK_NUM);
	Send_tcp(socket, info_pkt, 1000);

/*
// for each task, reply a packet 
	for(int i = 0; i < _tasks.size(); i++) {
		if(NO == _schedule_result[i].Is_discard && server_id == _schedule_result[i].src_id) {
			char sched_pkt[1000] = {'\0'};
			encaps_schedule_pkt(sched_pkt, task[i]);
			Send_tcp(socket, sched_pkt, 1400);
		}
	}
*/
}


int encaps_info_pkt(char *info_pkt, int original_task) {
    srand((unsigned)time(NULL));
	int cnt_accept = 0;
	//task info
	*((int *)(&info_pkt[0])) = original_task;

	for(int i = 0; i < original_task; i++) {
		if(rand()%100 >= THRESHOLD) {*(int *)(&info_pkt[(i+2) * 4]) = NO;}
		else {
			*(int *)(&info_pkt[(i+2) * 4]) = YES;
			cnt_accept++;
		}
	}
	*(int *)(&info_pkt[4]) = cnt_accept;
	cout << "the total task is: " << original_task << " ";  
	cout << "the acepted task is: " << cnt_accept << endl;
	return cnt_accept; 
} 

/*
//Is_discarded, flow_num, arrival time, ddl, 
void encaps_schedule_pkt(char *sched_pkt, struct Task_elem task) {
	
    for (int i = 0; i < task.flow_num; i++) {
        *((unsigned char *)(&sched_pkt[0])) = schedule_task.flow_num;
        *((int *)(&sched_pkt[i*])) = schedule_task.start_time[i];
        *((int *)(&sched_pkt[i*12 + 8])) = schedule_task.duration_len[i];
    }
}
*/
/*
void encaps_schedule_pkt(char *sched_pkt, struct Sched_task_elem &schedule_task) {
	for (int i = 0; i < schedule_task.flow_num; i++) {
		*((int *)(&sched_pkt[0])) = schedule_task.flow_num;
		*((int *)(&sched_pkt[i*12 + 4])) = schedule_task.start_time[i];
		*((int *)(&sched_pkt[i*12 + 8])) = schedule_task.duration_len[i];		
	}
}
*/
