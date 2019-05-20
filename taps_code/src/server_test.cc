#include "../include/mySocket.h"
#include "../include/common.h"

using namespace std;

//server side
int main() {
	char sent_data[100] = {'c', 'd','\0'};
	char sent_data_a[100] = {'\0'};

	char recv_data[100] = {'\0'};
//	char recv_data_a[100] = {'\0'};

	Transmitter server_socket;
	Transmitter client_socket;

	char ip_self[] = "192.168.15.33";
	char ip_dst[] = "192.168.15.33";

	server_socket.transmitter_new_tcp_responsor(ip_self, "10000");
	server_socket.Recv_tcp(recv_data, 100);
	printf( "the data recved is %s\n", recv_data);
	server_socket.Send_tcp(recv_data, 100);

	usleep(5000);

	client_socket.transmitter_new_tcp_sponsor(ip_self, "20000", ip_dst, "20001");
	client_socket.Send_tcp(sent_data, 100);
	printf( "the data sent is %s\n", sent_data);
	client_socket.Recv_tcp(sent_data_a, 100);
	printf( "the data sent of recv is %s\n", sent_data_a);

	return 0;
}

/*
void _flow(Transmitter server_socket, struct Flow_elem flow, struct Sched_task_elem sched) {


}
*/