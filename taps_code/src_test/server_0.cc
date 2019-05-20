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
//packet type
#define REQUEST 0

char ip_controller[] = "192.168.15.33";  //IP for SDN controller

char * ip[4] = {"192.168.15.33", //IP for server 0
				 "192.168.15.33",  //IP for server 1
				 "192.168.15.43",  //IP for server 2
				 "192.168.15.33"};  //IP for server 3

char ip_self[] = "192.168.15.33";

char ip_dst[] = "192.168.15.33";


char port_connect_self[] = "15000"; // starting port used to connect to client
char port_connect_2_controller[] = "14990";//port used to connect to SDN controller

char port_connect_dst[]  = "7000";

char port_controller[] = "5000"; //controller's listening port
char port_listen_self[] = "6000"; // the server self's listen port
char * port_listen_dst[] = {"5000", "7000", "8000", "9000"}; //all servers' listen port


void responsor(); //for listening 
void responsor_subflow(int sock);
void sponsor_subflow(char *_ip_self, char *_port_connect_self, 
					 char *_ip_dst,  char *_port_connect_dst);

void request_schedule();
void encaps_request_packet(char *request_pkt, int self_id, int pkt_type);


//server side
int main() {
//respsonsor thread

	thread td_responsor;
	td_responsor = thread(responsor);
	td_responsor.detach();

	sleep(5);

	for(int i = 0; i < 1; i++) {
	// (1)	task_generator();
	// (2)  get_schedule();
		request_schedule();
		cout << "************************" << endl;
		cout << "get flow scheule result!" << endl;
		cout << "************************" << endl;
	//==============================================
	// (3)  completing thesse task
	
		vector<thread> sponsor_threads;
		for(int i = 0; i < 10; i++) {
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
	printf("the thread of responsor/listen id is： %ld\n", syscall(SYS_gettid));

	int sock_connect;

	Transmitter server_socket;
	server_socket.transmitter_new_tcp_responsor(ip_self, port_listen_self);

	vector<thread> responsor_threads;

	for(int i = 0; i < 20; i++) {
		sock_connect = server_socket.Accept();
		responsor_threads.push_back(thread(responsor_subflow, sock_connect));
		responsor_threads.back().detach();
	}
//	while(1)
	sleep(2);
//	close(server_socket.listen_sock_fd);

}


//responsor data flow
void responsor_subflow(int sock) {
	printf("the responsor_subflow thread id is： %ld\n", syscall(SYS_gettid));

	char recv_data[100] = {'\0'};

	for(int i = 0; i < 10; i++) {
		Recv_tcp(sock, recv_data, 100);
		printf( "the %d-th data recved is %s\n", i, recv_data);
		Send_tcp(sock, recv_data, 100);	
	}

	close(sock);
}


//sponsor data flow
void sponsor_subflow(char *_ip_self, char *_port_connect_self, 
					 char *_ip_dst,  char *_port_connect_dst) {
	
	printf("the sponsor_subflow thread id is： %ld, the port_self is %d\n", 
			syscall(SYS_gettid), ntohs(htons(atoi(_port_connect_self) )));
	
	char sent_data[100] = {'a', 'b','\0'};
	char sent_data_a[100] = {'\0'};
//sleep(5);
	Transmitter client_socket;
	client_socket.transmitter_new_tcp_sponsor(_ip_self, _port_connect_self, 
											  _ip_dst,  _port_connect_dst);
//	for(int i = 0; ; i++) {
	for(int i = 0; i < 10; i++) {
		Send_tcp(client_socket.sock_id, sent_data, 100);
		printf( "the %d-th packet sent is %s\n", i, sent_data);
		Recv_tcp(client_socket.sock_id, sent_data_a, 100);
		printf( "the %d-th packet sent of recv is %s\n", i, sent_data_a);
	}

	close(client_socket.sock_id);
}


void request_schedule() {
	char request_pkt[2] = {'\0'};
	char recv_info_pkt[1000] = {'\0'}; 
	char sched_pkt[10][1400] = {'\0'}; // at most support 10 tasks

//sleep(5);
	Transmitter client_socket;
	client_socket.transmitter_new_tcp_sponsor(ip_self, port_connect_2_controller, 
											  ip_controller,  port_controller);

	encaps_request_packet(request_pkt, 0, REQUEST); // 0 stand for server 0
	Send_tcp(client_socket.sock_id, request_pkt, 2);
// recv info about schedule
	Recv_tcp(client_socket.sock_id, recv_info_pkt, 1000);
	cout << recv_info_pkt << endl;
	//	for(int i = 0; ; i++) {
//recv detail schedule result for all task of this server	
/*	
	int task_num = *((int *)(&recv_info_pkt[4]));
	for(int i = 0; i < task_num; i++) {
		Recv_tcp(client_socket.sock_id, sched_pkt, *((unsigned int)recv_info_pkt));
	}
//=================================================================
	decap_schedule_pkt(sched_pkt);
*/
	close(client_socket.sock_id);
}

void encaps_request_packet(char *request_pkt, int self_id, int pkt_type) {
	request_pkt[0] = self_id;
	request_pkt[1] = pkt_type;
}
