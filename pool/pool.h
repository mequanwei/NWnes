#ifndef POOL_POOL_H_
#define POOL_POOL_H_

#include "sem/mutex.h"
#include "sem/sem.h"
#include "db/connectionpool.h"
#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>


template <typename T>
class ThreadPool
{
public:
    /*thread_number是线程池中线程的数量，max_requests是请求队列中最多允许的、等待处理的请求的数量*/
    ThreadPool(ConnectionPool& pool_m, int thread_number = 8, int max_request = 10000);
    ~ThreadPool();
    bool append(T *request);

private:
    /*工作线程运行的函数，它不断从工作队列中取出任务并执行之*/
    static void *worker(void *arg);
    void run();

private:
    int m_thread_number;        //线程池中的线程数
    int m_max_requests;         //请求队列中允许的最大请求数
    pthread_t *m_threads;       //描述线程池的数组，其大小为m_thread_number
    std::list<T *> m_workqueue; //请求队列
    Mutex m_queuelocker;       //保护请求队列的互斥锁
    Sem m_queuestat;            //是否有任务需要处理
    ConnectionPool *m_connPool;  //数据库
};


#include "pool.h"
#include "db/connectionpool.h"
#include "sem/mutex.h"
#include <list>
#include <cstdio>
#include <exception>
#include <pthread.h>



template <typename T>
ThreadPool<T>::ThreadPool(ConnectionPool& connPool, int thread_number, int max_requests):m_thread_number(thread_number), m_max_requests(max_requests), m_threads(NULL),m_connPool(&connPool)
{
    if (thread_number <= 0 || max_requests <= 0)
        throw std::exception();
    m_threads = new pthread_t[m_thread_number];
    if (!m_threads)
        throw std::exception();
    for (int i = 0; i < thread_number; ++i)
    {
        if (pthread_create(m_threads + i, NULL, worker, this) != 0)
        {
            delete[] m_threads;
            throw std::exception();
        }
        if (pthread_detach(m_threads[i]))
        {
            delete[] m_threads;
            throw std::exception();
        }
    }
}

template <typename T>
ThreadPool<T>::~ThreadPool()
{
    delete[] m_threads;
}

template <typename T>
bool ThreadPool<T>::append(T *request)
{
    m_queuelocker.Lock();
    if (m_workqueue.size() >= m_max_requests)
    {
        m_queuelocker.Unlock();
        return false;
    }
    m_workqueue.push_back(request);
    m_queuelocker.Unlock();
    m_queuestat.post();
    return true;
}
template <typename T>
void *ThreadPool<T>::worker(void *arg)
{
	ThreadPool *pool = (ThreadPool *)arg;
    pool->run();
    return pool;
}

template <typename T>
void ThreadPool<T>::run()
{
    while (true)
    {
        m_queuestat.wait();
        m_queuelocker.Lock();
        if (m_workqueue.empty())
        {
            m_queuelocker.Unlock();
            continue;
        }
        T *request = m_workqueue.front();
        m_workqueue.pop_front();
        m_queuelocker.Unlock();
        if (!request)
        {
        	continue;
        }

//		m_connPool = &request->mysql;
		request->process();
    }
}

#endif

