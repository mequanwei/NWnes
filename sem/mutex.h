#ifndef SEM_MUTEX_H_
#define SEM_MUTEX_H_
#include <semaphore.h>


class Mutex
{
public:
	Mutex();
	~Mutex();

	bool Lock();
	bool Unlock();
	pthread_mutex_t *GetMutex();

private:
    pthread_mutex_t m_mutex;
};


#endif
