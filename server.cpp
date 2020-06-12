#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <sys/select.h>


#define ACCEPTED_ADDRESS INADDR_ANY
#define PORT 1234
#define BACKLOG 2
#define BUFFER_SIZE 1024

int main()
{
	int ret;
	int server_socket;
	int client_socket;
	int status;
	char msg[BUFFER_SIZE]; /*buffer*/
	struct sockaddr_in ser_addr; /* server address */
	struct sockaddr_in cli_addr; /* client address */

//	struct fd_set fds;
	fd_set* f;
	FILE *fp;
	int maxfdp;


	//create socket
	//协议族/域，协议类型，协议编号
    //AF_INET :ipV4
	//SOCK_STREAM: socket
	//0:auto
	server_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (server_socket < 0)
	{
		printf("open socket error\n");
		return 0;
	}

	//bind socket
	ser_addr.sin_family = AF_INET;
	ser_addr.sin_addr.s_addr = htonl(ACCEPTED_ADDRESS);
	ser_addr.sin_port = htons(PORT);

	ret = bind(server_socket, (struct sockaddr*) &ser_addr,sizeof(struct sockaddr_in));
	if (ret < 0)
	{
		printf("bind error\n");
		return 0;
	}

	//set listening status;
	ret = listen(server_socket, BACKLOG);
	if (ret < 0)
	{
		printf("listening error\n");
		return 0;
	}
	printf("begin listening\n");

	//handle listening
	socklen_t len = 0;
	while (1)
	{
//		FD_ZERO(&fds);
//		FD_SET(server_socket,&fds);
//		 maxfdp=server_socket>fp?server_socket+1:fp+1; //描述符最大值加1


		client_socket = accept(server_socket, (sockaddr*)&cli_addr, (socklen_t *)&len); //第二三个参数用记录连接的客户端状态
		if (client_socket < 0)
		{
			printf("handle error\n");
			return 0;
		}
		printf("establish conection to %s..\n",inet_ntoa(cli_addr.sin_addr));
		if (fork()==0)
		{
			printf("my pid %d\n",getpid());
			recv(client_socket, msg, (size_t) BUFFER_SIZE, 0);
			printf("%d:%s\n", getpid(),msg);
			strcpy(msg, "hello,client,this is server_s");
			send(client_socket, msg, sizeof(msg), 0); //sent msg
			status=0;
			while(status==0)
			{
				//wait
			}
			close(client_socket);
		}else{
			close(client_socket);
			printf("keep listening..\n ");
		}
	}
	close(server_socket);
}
