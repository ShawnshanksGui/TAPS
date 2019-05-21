#ifndef _MYSOCKET_H_
#define _MYSOCKET_H_

#include "fcntl.h"
#include "sys/types.h"
#include "sys/socket.h"
#include "poll.h"

#include "unistd.h"

#include "netinet/in.h"
#include "netinet/ip.h"

typedef struct sockaddr SA;

class Transmitter {
public:
	int sock_id;
	int listen_sock_fd;

	struct sockaddr_in client_addr;
	struct sockaddr_in server_addr;

	socklen_t client_addr_len;
	socklen_t server_addr_len;

	void Socket_for_udp();

//responsor
	void Socket_for_tcp_listen();

	void Setsockopt(int , int , int, void *, int);
	void Bind(int, SA *, int) const;
	void Listen();
//	void Accept();

//the below is sponsor's 3 function
	void Socket_for_tcp();	
	void Connect();
	int CheckConnect(int iSocket);
//  Udp socket doesn't need to establish any connections!!!
//	void Connect(int sock_id, struct sockaddr *serv_addr, int len_sock_addr) const;



//	Transmitter(int, struct sockaddr_in, struct sockaddr_in);
	Transmitter() {
		client_addr_len = sizeof(client_addr);
		server_addr_len = sizeof(server_addr); 
	}

	~Transmitter();

//support concurrency	
	int Accept();

	void transmitter_new_udp(char *addr_self, char *port_self, 
		                 char *addr_dst,  char *port_dst); 
                                      
//	void transmitter_new_tcp(char *add_self, char *port_self);
	void transmitter_new_tcp_responsor(char *addr_self, char *port_self);
	void transmitter_new_tcp_sponsor(char *addr_self, char *port_self,
                					 char *addr_dst, char *port_dst);

	void transmitter_new_tcp_non_b(char *addr_self, char *port_self);
	

};

//********UDP********
int Send_udp(int sock, char *data_src, int len);
int Recv_udp(int sock, char *buf_dst, int len);

//********TCP********
int Send_tcp(int sock_connect, char *data_src, int len);
//	int Send_tcp_non_b(char *data, int len);
int Recv_tcp(int sock_connect, char *data_dst, int len);


#endif
