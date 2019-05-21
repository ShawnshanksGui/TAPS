//#include "../include/thread_core_affinity.h"
#include "../include_test/common.h"
#include "../include_test/mySocket.h"

#include "unistd.h"
#include "sys/types.h"
#include "sys/syscall.h"

#define ON_REUSEADDR  1  //you can reuse the addr after binding addr without no waiting time 
//#define OFF_REUSEADDR 0

#define DATA 1
#define CONTROL 2

#define SEG_STOP 0;
//
#define LISTENQ 100

int Terminal_AllThds;
int Terminal_SendThds;

typedef unsigned char uchar;

Transmitter::
~Transmitter() {
	if(sock_id > 0) {
		close(sock_id);
	}
	if(listen_sock_fd > 0) {
		close(listen_sock_fd);
	}
}

void Transmitter::
Socket_for_udp() {
	if((sock_id = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		perror("\nCreate udp socket falied\n");
		exit(0);
	}
}

//responsor
void Transmitter::
Socket_for_tcp_listen() {
	if((listen_sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("\nCreate tcp listen socket falied\n");
		exit(0);
	}
}

//sponsor
void Transmitter::
Socket_for_tcp() {
	if((sock_id = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Create socket falied\n");
		exit(0);
	}	
}

void Transmitter::
Setsockopt(int sock, int level, int option_name,
               void *option_value, int option_len) {
    if(-1 == setsockopt(sock, level, 
    	option_name, option_value, option_len)) {
        perror("setsockopt failed!\n");
        exit(0);
    }
}

void Transmitter::
Bind(int sock, SA *addr_self, int len) const 
{
	if(-1 == bind(sock, addr_self, len)) {
		perror("bind failed!!!");
		exit(0);
	}
}

void Transmitter::
Listen() {
	if(-1 == listen(listen_sock_fd, LISTENQ)) {
		perror("\nlisten error happened!\n");
		exit(0);
	}
}

/*
void Transmitter::
Accept() {
	if(-1 == (sock_id = accept(listen_sock_fd, (SA *)&client_addr,
						 &client_addr_len))) {
		perror("\nAccept failed!\n");
		exit(0);
	}
}
*/

//support for concurrency 
int Transmitter::
Accept() {
	int sock_connect = 0;
	if(-1 == (sock_connect = accept(listen_sock_fd, (SA *)&client_addr,
						 &client_addr_len))) {
		perror("\nAccept failed!\n");
		exit(0);
	}
	return sock_connect;
}

//sponsor
int Transmitter::
CheckConnect(int iSocket) {
	struct pollfd fd;
	int ret = 0;
	socklen_t len = 0;

	fd.fd = iSocket;
	fd.events = POLLOUT;

	while ( poll (&fd, 1, -1) == -1 ) {
		if( errno != EINTR ){
			perror("poll");
//			return -1;
			exit(0);
		}
	}

	len = sizeof(ret);
	if ( getsockopt (iSocket, SOL_SOCKET, SO_ERROR, &ret, &len) == -1 ) {
    	perror("getsockopt");
//		return -1;
		exit(0);
	}

	if(ret != 0) {
		fprintf (stderr, "socket %d connect failed: %s\n",
                 iSocket, strerror (ret));
//		return -1;
		exit(0);
	}

	return 0;
}

//  tcp needs to establish a connections!!!
//sponsor
void Transmitter::
Connect() {
//printf("the port is %d\n", server_addr.sin_port);
	if(connect(sock_id, (SA *)&server_addr, sizeof(server_addr)) < 0) {
    	if(errno != EINPROGRESS) { 
	  		perror("Connect socket failed: ");
	  		printf("thread id is锛?%ld, the port_dst is %d\n", 
	  				syscall(SYS_gettid), server_addr.sin_port);	
        	exit(0);
		}
		else{
			perror("connect");
			printf("check delay connection\n");
			CheckConnect(sock_id);			
		}	
 	}
}


void Transmitter::
transmitter_new_udp(char *addr_self, char *port_self, 
	                 char *addr_dst,  char *port_dst) {
	memset(&(server_addr), 0, sizeof(server_addr));
	memset(&(client_addr), 0, sizeof(client_addr));
	
	Socket_for_udp();

//enable fastly recover the port which just has been occupied. 
	int state_reuseAddr              = ON_REUSEADDR;
	Setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR, 
		       &state_reuseAddr, sizeof(state_reuseAddr));

//set the size of recv buffer
	int recv_buf_size=1*1024*1024*1024;
	Setsockopt(sock_id, SOL_SOCKET, SO_RCVBUF, (char *)&recv_buf_size, sizeof(int));

	client_addr.sin_family  = AF_INET;
	client_addr.sin_port    = htons(atoi(port_dst));
	inet_pton(AF_INET, addr_dst, &(client_addr.sin_addr));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(atoi(port_self));
	inet_pton(AF_INET, addr_self, &(server_addr.sin_addr));

	Bind(sock_id, (SA *)&server_addr, sizeof(server_addr));
//  udp needn't establish any connection!!!
//	Connect(sock_id, (struct sockaddr *)&server_addr, sizeof(struct sockaddr));  
}


void Transmitter::
transmitter_new_tcp_responsor(char *addr_self, char *port_self) {
	memset(&(server_addr), 0, sizeof(server_addr));
//	memset(&(client_addr), 0, sizeof(client_addr));

	Socket_for_tcp_listen();

//enable fastly recover the port which just has been occupied. 
	int state_reuseAddr = ON_REUSEADDR;
	Setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, 
		       &state_reuseAddr, sizeof(state_reuseAddr));
//set the size of recv buffer
	int recv_buf_size = 1*1024*1024*1024;
	Setsockopt(listen_sock_fd, SOL_SOCKET, SO_RCVBUF, 
			   (char *)&recv_buf_size, sizeof(int));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(atoi(port_self));
	inet_pton(AF_INET, addr_self, &(server_addr.sin_addr));

	Bind(listen_sock_fd, (SA *)&server_addr, sizeof(server_addr));

	Listen();
//	Accept();
}


//defult blocking model
void Transmitter::
transmitter_new_tcp_sponsor(char *addr_self, char *port_self,
                			char *addr_dst, char *port_dst) {
    memset(&(server_addr), 0, sizeof(server_addr));
    memset(&(client_addr), 0, sizeof(client_addr));

    Socket_for_tcp();
//enable fastly recover the port which just has been occupied. 
    int state_reuseAddr              = ON_REUSEADDR;
    Setsockopt(sock_id, SOL_SOCKET, SO_REUSEADDR,
               &state_reuseAddr, sizeof(state_reuseAddr));
//set the size of recv buffer
    int recv_buf_size = 1*1024*1024*1024;
    Setsockopt(sock_id, SOL_SOCKET, SO_RCVBUF, (char *)&recv_buf_size, sizeof(int));

    server_addr.sin_family  = AF_INET;
    server_addr.sin_port    = htons(atoi(port_dst));
    inet_pton(AF_INET, addr_dst, &(server_addr.sin_addr));

    client_addr.sin_family = AF_INET;
    client_addr.sin_port   = htons(atoi(port_self));
    inet_pton(AF_INET, addr_self, &(client_addr.sin_addr));

//Bind will cause "Can't assign requested address!!!"
//    Bind(sock_id, (struct sockaddr *)&client_addr, sizeof(client_addr));

//  tcp need to establish a connection!!!
  	Connect();  

}


/*

void Transmitter::
transmitter_new_tcp_non_b(char *addr_self, char *port_self) {
	memset(&(server_addr), 0, sizeof(server_addr));
//	memset(&(client_addr), 0, sizeof(client_addr));

	Socket_for_tcp_listen();

//enable fastly recover the port which just has been occupied. 
	int state_reuseAddr = ON_REUSEADDR;
	Setsockopt(listen_sock_fd, SOL_SOCKET, SO_REUSEADDR, 
		       &state_reuseAddr, sizeof(state_reuseAddr));
//set the size of recv buffer
	int recv_buf_size = 1*1024*1024*1024;
	Setsockopt(listen_sock_fd, SOL_SOCKET, SO_RCVBUF, 
			   (char *)&recv_buf_size, sizeof(int));

	server_addr.sin_family = AF_INET;
	server_addr.sin_port   = htons(atoi(port_self));
	inet_pton(AF_INET, addr_self, &(server_addr.sin_addr));

	Bind(listen_sock_fd, (SA *)&server_addr, sizeof(server_addr));

	Listen();
	Accept();

//set non-blocking mode
    int flags = fcntl(sock_id, F_GETFL, 0);
    fcntl(sock_id, F_SETFL, flags|O_NONBLOCK);
}
*/

//=======================================================================
//********UDP*********
int Send_udp(int sock, char *data_src, int len) {
	int num_sent = 0;

	if((num_sent = sendto(sock, data_src, len, 0, 
		(SA *)&(client_addr), sizeof(SA))) < 0) {
		printf("\n!!!send failed, send %d bytes!!!!!!\n", num_sent);
		exit(0);
	}
	return num_sent;
}

int Recv_udp(int sock, char *data_dst, int len) {
	int num_recv = 0;
	socklen_t len_server_addr;

	if ((num_recv = recvfrom(sock, data_dst, len, 0, 
	   (SA *)&(client_addr), &len_server_addr)) < 0) {
		printf("\n!!!recv failed, just recv %d bytes!!!!!!\n", num_recv);
		exit(0);
	}
	return num_recv;
}

//********TCP*********
int Send_tcp(int sock_connect, char *data, int len) {
	int len_sent = 0;
	len_sent = send(sock_connect, data, len, 0);
	if(-1 == len_sent) {
		perror("send failed");
		exit(0);
	}
	else if(len_sent != len) {
		printf("\nOnly send %dbytes data\n", len_sent);
	}
	return len_sent;
}

//default blocking mode
int Recv_tcp(int sock_connect, char *data_dst, int len) {
	int len_recv;
	len_recv = recv(sock_connect, data_dst, len, 0);
	if(-1 == len_recv) {
		perror("\nrecv failed\n");
		exit(0);
	}
	else if(len_recv != len) {
		printf("\nOnly recv %dbytes data\n", len_recv);
//		exit(0);
	}
	return len_recv;
}



/*
//set non-blocking mode
int Transmitter::
Send_tcp_non_b(char *buf, int buf_len) {
	size_t tmp = 0;
	size_t total = buf_len;
	char *p = buf;
//	len_sent = send(sock_id, data, len, MSG_DONTWAIT);

	while(1) {

//        if(Terminal_AllThds || Terminal_SendThds) {
//            return 0;
//        }
	
		if(total<=0)return buf_len;
//		printf("start entering the send func in Send_tcp_non_b func\n");
//		tmp = send(sock_id, p, total, MSG_DONTWAIT);
		tmp = send(sock_id, p, total, 0);

//		printf("send    \n");
		if(tmp < 0) {
			if(errno == EAGAIN) {
				usleep(20);
				continue;
			}
			else {
				perror("send failed in sende_tcp_non_b!\n");
				exit(0);
			}
		}
		else if(tmp == total) { //send successfully!
				return buf_len;
		}
		total -= tmp;
		p += tmp;
	}
}



//set non-blocking mode
int Transmitter::
Recv_tcp_non_b(char *data_dst, int len) {
	int len_recv;
	len_recv = recv(sock_id, data_dst, len, MSG_DONTWAIT);

	while(-1 == len_recv && EAGAIN != errno) {
    	if(Terminal_AllThds || Terminal_SendThds) {
    		return 0;
    	}		
    	len_recv = recv(sock_id, data_dst, len, MSG_DONTWAIT);
	}

	return len_recv;
}

int Transmitter::
Recv_tcp_fixed_len(char *data_dst, int len) {
    int tmp = 0;
    char *loc = data_dst;
    int len_specified = len;

    while(1) {
        if(Terminal_AllThds || Terminal_SendThds) {
            break;
        }
        tmp = Recv_tcp(loc, len_specified);
        if(tmp < len_specified) {
            len_specified -= tmp;
            loc += tmp;
        }
        else {break;}
    }

    return len;
}


int Transmitter::
Recv_tcp_non_b_fixed_len(char *data_dst, int len) {
    int tmp = 0;
    char *loc = data_dst;
    int len_specified = len;

    while(1) {
//      printf("already enter the Recv_tcp_non_b_fixed_len while\n");

        if(Terminal_AllThds || Terminal_SendThds) {
            break;
        }

//      printf("start entering  Recv_tcp_non_b\n");
        tmp = Recv_tcp_non_b(loc, len_specified);
//      printf("leave the Recv_tcp_non_b\n");

        if(tmp < len_specified) {
            len_specified -= tmp;
            loc += tmp;
        }
        else {break;}
    }
    printf("recv %d bytes\n", len);

    return len;
}



int Transmitter::
Sendto_tcp(char *data, int len) {
	int len_sent = 0;
	len_sent = sendto(sock_id, data, len, 0, (SA *)&client_addr,
					  client_addr_len);
	if(-1 == len_sent) {
		perror("sendto failed");
		exit(0);
	}
	else if(len_sent != len) {
		printf("\nOnly send %dbytes data\n", len_sent);
	}
	return len_sent;
}

int Transmitter::
Recvfrom_tcp(char *data_dst, int len) {
	int len_recv;
	len_recv = recvfrom(sock_id, data_dst, len, 0, (SA *)&client_addr, 
						&client_addr_len);
	if(-1 == len_recv) {
		perror("\nrecv failed\n");
		exit(0);
	}
	else if(len_recv != len) {
		printf("\nOnly recv %dbytes data\n", len_recv);
		exit(0);
	}
	return len_recv;
}
//==========================================================================
*/
