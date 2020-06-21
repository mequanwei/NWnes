#include "app/handle.h"
#include "server/server.h"
#include "db/connectionpool.h"
#include "pool/pool.h"
#include "app/handle.h"
#include <stdio.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <iostream>
#include <sys/epoll.h>
#include <fcntl.h>
using namespace std;

#define ACCEPTED_ADDRESS INADDR_ANY
#define PORT 1234
#define BACKLOG 2
#define BUFFER_SIZE 1024


Server::Server()
{
	//init db pool
	ConnectionPool &m_pool = ConnectionPool::GetInstance();
	m_pool.init("localhost","root","mysql","server",3306,4,0); //quanwei use config file

	//init thread pool
	t_pool = new ThreadPool<Handle>(m_pool,3, 10);

	epollfd = 0;
	cout << "start server.\n";
}

int Server::start()
{
	int ret;
	int server_socket;
	int status;
	char msg[BUFFER_SIZE]; /*buffer*/
	struct sockaddr_in ser_addr; /* server address */
	struct sockaddr_in cli_addr; /* client address */

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

	ret = bind(server_socket, (struct sockaddr*) &ser_addr,
			sizeof(struct sockaddr_in));
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


	//	struct fd_set fds;
	cout <<"epoll begin";
	epollfd = epoll_create(200);
	cout <<"epoll";
	addfd(epollfd, server_socket);

	int number;

	while (1)
	{
		number = epoll_wait(epollfd, events, 20, 2000);
		printf("num: %d.\n",number);
		for (int i = 0; i < number; i++)
		{
			if (events[i].data.fd == ret)
			{
				cout << "new connetction."<<endl;
				int client_socket = accept(server_socket, NULL, NULL);  //第二三个参数用记录连接的客户端状态
				if (client_socket <0)
				{
					cout <<"error"<<endl;
				}
				addfd(epollfd, client_socket);
			}
			else if (events[i].events & EPOLLIN)
			{
				recv(events[i].data.fd , msg, (size_t) BUFFER_SIZE, 0);
				cout << "recv msg " + string(msg) <<endl;
//				printf("recv\n.");
			}
			else if (events[i].events & EPOLLOUT)
			{
				cout << "witre\n";
			}

		}
	}

	return 0;
}

void Server::addfd(int epollfd, int fd) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET|EPOLLRDHUP|EPOLLOUT;
    /* 针对connfd，开启EPOLLONESHOT，因为我们希望每个socket在任意时刻都只被一个线程处理 */

    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);

    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;

    fcntl(fd, F_SETFL, new_option);
    printf("add done\n");
}
