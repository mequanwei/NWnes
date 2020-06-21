#ifndef SEM_H
#define SEM_H

#include <semaphore.h>
class Sem
{
public:
	Sem();
	Sem(int num);
	~Sem();
	bool post();
	bool wait();
private:
    sem_t m_sem;
};
#endif
