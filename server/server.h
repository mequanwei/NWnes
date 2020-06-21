#ifndef SERVER_H
#define SERVER_H
#include "pool/pool.h"
#include "app/handle.h"
#include <sys/epoll.h>
class Server
{
public:
	Server();
	int start();

private:
	ThreadPool<Handle>* t_pool;
	int epollfd;
	epoll_event events[20];
	void addfd(int epollfd, int fd);


};

#endif
