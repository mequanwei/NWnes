#include "mutex.h"
#include <semaphore.h>
#include <pthread.h>
#include <exception>

Mutex::Mutex()
{
	if (pthread_mutex_init(&m_mutex, NULL) != 0)
	{
		throw std::exception();
	}
}

Mutex::~Mutex()
{
	pthread_mutex_destroy(&m_mutex);
}

bool Mutex::Lock()
{
	return pthread_mutex_lock(&m_mutex) == 0;
}

bool Mutex::Unlock()
{
	return pthread_mutex_unlock(&m_mutex) == 0;
}

pthread_mutex_t* Mutex::GetMutex()
{
	return &m_mutex;
}
