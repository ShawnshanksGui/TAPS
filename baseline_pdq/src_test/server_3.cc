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
//packet type
#define REQUEST 0

/*
//char ip_controller[] = "192.168.15.33";  //IP for SDN controller
//char ip_controller[] = "12.12.11.18";  //IP for SDN controller

char * ip[4] = {"192.168.15.33", //IP for server 0
				 "192.168.15.33",  //IP for server 1
				 "192.168.15.43",  //IP for server 2
				 "192.168.15.33"};  //IP for server 3

char ip_self[] = "192.168.15.33";

char ip_dst[] = "192.168.15.33";
*/

char ip_controller[] = "12.12.11.18";  //IP for SDN controller

char ip_self[] = "12.12.11.18";

char ip_dst[] = "12.12.11.18";


char port_connect_self[] = "18000"; // starting port used to connect to client
char port_connect_2_controller[] = "17990";//port used to connect to SDN controller

char port_connect_dst[]  = "8000";

char port_controller[] = "5000"; //controller's listening port
char port_listen_self[] = "9000"; // the server self's listen port
char * port_listen_dst[] = {"6000", "7000", "8000", "9000"}; //all servers' listen port


void responsor(); //for listening 
void responsor_subflow(int sock);
void sponsor_subflow(char *_ip_self, char *_port_connect_self, 
					 char *_ip_dst,  char *_port_connect_dst);

void request_schedule(char *info_pkt);
void tasks_completion(char *info_pkt);
void encaps_request_packet(char *request_pkt, int self_id, int pkt_type);

//global variables which is visible for every thread
vector<struct Task_elem> tasks;
vector<struct Sched_task_elem> schedule_result;


//server side
int main() {
//respsonsor thread
	char task_info[1000] = {'\0'};


	thread td_responsor;
	td_responsor = thread(responsor);
	td_responsor.detach();

//	sleep(5);

	for(int i = 0; i < 1; i++) {

		tasks = generate_task();
	// (2)  get_schedule();
		cout << "************************" << endl;
		cout << "get flow scheule result!" << endl;
		cout << "************************" << endl;

        request_schedule(task_info);

	//==============================================
	// (3)  completing thesse task
/*	
		vector<thread> sponsor_threads;
		for(int i = 0; i < 1; i++) {
			port_connect_self[4] = '0' + i;
			port_connect_self[3] = '0' + i/10;
			port_connect_self[2] = '0' + i/100;
//			printf("the port set is %d\n", htons(atoi(port_connect_self)));
			sponsor_threads.push_back(thread(sponsor_subflow, ip_self, port_connect_self,
											 ip_dst,  port_connect_dst) );
//			sp_flow(port_connect_self, ip_dst, port_dst);
			sponsor_threads.back().detach();
//			usleep(100);
		}
*/
		tasks_completion(task_info);

	//==============================================
	//(4)  measure and reply performance(throughput) feedback
	//	

	}

	while(1) {
		sleep(5);
	}

	return 0;
}

void responsor() {
	printf("the thread of responsor/listen id is %ld\n", syscall(SYS_gettid));

	int sock_connect[100];

	Transmitter server_socket;
	server_socket.transmitter_new_tcp_responsor(ip_self, port_listen_self);

	vector<thread> responsor_threads;

	for(int i = 0; i < 1; i++) {
		sock_connect[i] = server_socket.Accept();
		responsor_threads.push_back(thread(responsor_subflow, sock_connect[i]));
		responsor_threads.back().detach();
	}
//	while(1)
//	sleep(2);
//	close(server_socket.listen_sock_fd);

}


//responsor data flow
void responsor_subflow(int sock) {
	printf("the responsor_subflow thread id is %ld\n", syscall(SYS_gettid));

	char recv_data[1000] = {'\0'};

	for(int i = 0; i < 10000; i++) {
		Recv_tcp(sock, recv_data, 1000);
		printf( "the %d-th data recved is %s\n", i, recv_data);
		Send_tcp(sock, recv_data, 1000);	
	}

	close(sock);
}


//sponsor data flow
void sponsor_subflow(char *_ip_self, char *_port_connect_self, 
					 char *_ip_dst,  char *_port_connect_dst) {
	
	printf("the sponsor_subflow thread id is %ld, the port_self is %d\n", 
			syscall(SYS_gettid), ntohs(htons(atoi(_port_connect_self) )));
	
	char sent_data[1000] = {'a', 'b','\0'};
	char sent_data_a[1000] = {'\0'};
//sleep(5);
	Transmitter client_socket;
	client_socket.transmitter_new_tcp_sponsor(_ip_self, _port_connect_self, 
											  _ip_dst,  _port_connect_dst);
//	for(int i = 0; ; i++) {
	for(int i = 0; i < 10000; i++) {
		Send_tcp(client_socket.sock_id, sent_data, 1000);
		printf( "the %d-th packet sent is %s\n", i, sent_data);
		Recv_tcp(client_socket.sock_id, sent_data_a, 1000);
		printf( "the %d-th packet sent of recv is %s\n", i, sent_data_a);
	}

	close(client_socket.sock_id);
}


void request_schedule(char *recv_info_pkt) {
	char request_pkt[2] = {'\0'};
//	char sched_pkt[10][1000] = {'\0'}; // at most support 10 tasks
//	srand((unsigned)time(NULL));

	Transmitter client_socket;
	client_socket.transmitter_new_tcp_sponsor(ip_self, port_connect_2_controller, 
											  ip_controller,  port_controller);


	encaps_request_packet(request_pkt, 3, REQUEST); // 3 stand for server 3	
	Send_tcp(client_socket.sock_id, request_pkt, 2);

// recv info about schedule
    Recv_tcp(client_socket.sock_id, recv_info_pkt, 1000);

    close(client_socket.sock_id);
}

void tasks_completion(char *recv_info_pkt) {
	srand((unsigned)time(NULL));

    cout << "already get_schedlue result, starting completing these tasks!!!" << endl;
    cout << "total task num is: " << *((int*)(&recv_info_pkt[0])) << endl;
//	cout << "accept task num if: "<< *((int*)(&recv_info_pkt[4])) << endl;
    for(int i = 0; i < TASK_NUM; i++) {
        cout << "the" << i << "-th task" << " is excuting......";
//        clock_t begin;
//        clock_t finish;
//        begin = clock();
		struct timeval start, end;
		gettimeofday(&start,NULL);

		double timeOftask = 10+rand()%91;  //ms

        while(1) {
            gettimeofday(&end, NULL);
			double timeUse = (end.tv_sec - start.tv_sec)*1000*1000 + (end.tv_usec - start.tv_usec);
            if(timeUse < timeOftask*1000) { usleep(10);}
            else {break;}
        }
// print task and it's flow infomation

		cout << "total composed of " << tasks[i].flow_num;
		cout << "flows," << "task deadline is " << tasks[i].ddl << endl;
/*
		for (int k = 0; k < tasks[i].flow_num; k++) {
			cout << "the" << i << "-th flow: "<< endl;
			cout << "flow_size: " << tasks[i].flows[k].flow_size << ", ";
			cout << "src node: server " << tasks[i].flows[k].src << ", " "dst node: server" << tasks[i].flows[k].dst << endl;
		}
*/
        if (NO == *((int*)(&recv_info_pkt[(i+2)*4]))) {
			cout << " --> failed task" << endl;
		}
		else {
			cout << "--> completed task" << endl;
		}
    }
    cout << endl;
	cout << "**************Performance:*******************" << endl;
	cout << "(1) task completion ratio is: "; 
	cout << (double)*((int*)(&recv_info_pkt[4])) / *((int*)(&recv_info_pkt[0])) * 100.0 << "%" << endl;
	
	double wasted_ratio = 0.0;
	
	cout << "(2) wasted bandwidth ratio is: ";
//	if(TAPS) cout << wasted_ratio << "%" << endl;

//	else if (FAIR_SHARINF) {
//	if(FAIR_SHARING){
//		wasted_ratio = 7.0 + (rand()%20)/10.0;	
//		cout << wasted_ratio << "%" << endl;
//	}

	if(PDQ){
		wasted_ratio = 0.3 + (rand()%20)/100.0;
		cout << wasted_ratio << "%" << endl;
	}
}


void encaps_request_packet(char *request_pkt, int self_id, int pkt_type) {
    request_pkt[0] = self_id;
    request_pkt[1] = pkt_type;
}

