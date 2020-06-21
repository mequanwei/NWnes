#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <errno.h>

#define BUFFER_SIZE 1024
#define PORT 1234
int main()
{
	int ret;
	int client_socket;
	int status;
	char msg[BUFFER_SIZE]; /*buffer*/
	struct sockaddr_in cli_addr; /* client address,unused */

	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 0)
	{
		printf("open socket error\n");
		return 0;
	}

	cli_addr.sin_family = AF_INET;
	cli_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	cli_addr.sin_port = htons(PORT);

	ret = connect(client_socket, (struct sockaddr*) &cli_addr,
			sizeof(sockaddr_in));
	if (ret < 0)
	{
		printf("connect error\n");
		return 0;
	}

	//begin working
	printf("success connect\n");

	strcpy(msg, "hello server,this is client");
	send(client_socket, msg, strlen(msg), 0);
//	recv(client_socket, msg, (size_t) BUFFER_SIZE, 0);
//	printf("%s\n", msg);
	status=0;

	while(status==0)
	{

	}

	close(client_socket);

}
