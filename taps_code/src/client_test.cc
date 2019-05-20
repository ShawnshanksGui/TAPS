#include "../include/mySocket.h"
#include "../include/common.h"

using namespace std;

//client side
int main() {
	char sent_data[100] = {'a', 'b','\0'};
	char sent_data_a[100] = {'\0'};

	char recv_data[100] = {'\0'};
//	char recv_data_a[100] = {'\0'};

	Transmitter client_socket;
	Transmitter server_socket;

	char ip_self[] = "192.168.15.33";
	char ip_dst[] = "192.168.15.33";

	client_socket.transmitter_new_tcp_sponsor(ip_self, "10001", ip_dst, "10000");
	client_socket.Send_tcp(sent_data, 100);
	printf( "the data sent is %s\n", sent_data);
	client_socket.Recv_tcp(sent_data_a, 100);
	printf( "the data sent of recv is %s\n", sent_data_a);	

//	client_socket.transmitter_new_tcp_client("192.168.15." + to_string(33), 1001);

	server_socket.transmitter_new_tcp_responsor(ip_self, "20001");
	server_socket.Recv_tcp(recv_data, 100);
	printf( "the data recved is %s\n", recv_data);
	server_socket.Send_tcp(recv_data, 100);

	return 0;
}

/*
void _flow(Transmitter server_socket, struct Flow_elem flow, struct Sched_task_elem sched) {


}
*/