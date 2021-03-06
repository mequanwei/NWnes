#include "../sem/sem.h"

#include <exception>
#include <semaphore.h>

Sem::Sem()
{

}
Sem::Sem(int num)
{
	//信号量初始化
	if (sem_init(&m_sem, 0, num) != 0)
	{
		throw std::exception();
	}
}
Sem::~Sem()
{
	 sem_destroy(&m_sem);
}

bool Sem::wait()
{
	return sem_wait(&m_sem) == 0;
}
bool Sem::post()
{
	return sem_post(&m_sem) == 0;
}
