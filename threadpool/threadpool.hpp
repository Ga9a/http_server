#pragma once
#include "task.hpp"

const static size_t NUM = 6;

class ThreadPool
{
    private:
    locker _mtx;
    cond _cond;
    size_t _num;
    std::queue<Task*> _taskQueue;
    static ThreadPool* _tp;

    private:

    ThreadPool(size_t num=NUM) :_num(num)  {}

    ~ThreadPool(){};

    void lock(){
        _mtx.lock();
    }
    void unlock(){
        _mtx.unlock();
    }
    void waitcond(){
        _cond.wait(_mtx.get());
    }
    void wakethread(){
        _cond.signal();
    }

    void PopTask(Task* &task) //
    {
        task = _taskQueue.front();
        _taskQueue.pop();
    }

    static void* ThreadRountine(void* args)
    {
        pthread_detach(pthread_self());
        auto ptp = reinterpret_cast<ThreadPool*>(args);

        while(1)
        {

            ptp->lock();

            while(ptp->_taskQueue.empty())
            {
                ptp->waitcond();
            }

            Task* t;
            ptp->PopTask(t);
            ptp->unlock();
            t->PorcessOn();
            delete t;

        }
    }

    void ThreadPoolInit()
    {
        LOG("ThreadPool start to init",INFO);
        for(size_t i = 0; i < _num; ++i)
        {
            pthread_t tid;

            if(pthread_create(&tid, nullptr, ThreadRountine, this) != 0)
            {
                LOG("ThreadPool init failed(thread create failed)",ERROR);
                exit(5);
            }
        }
        LOG("ThreadPool init success",INFO);
    }
    public:

    static ThreadPool* GetInstance()
    {
        static locker mtx;
        if(_tp == nullptr)
        {
            mtx.lock();
            if(_tp == nullptr)
            {
                _tp = new ThreadPool();
                _tp->ThreadPoolInit();
            }
            mtx.unlock();
        }
        return _tp;
    }

    void PushTask(Task* task)
    {
        _mtx.lock();
        _taskQueue.push(task);
        _mtx.unlock();
        wakethread();
        LOG("push task success",INFO);
    }
};

ThreadPool* ThreadPool::_tp = nullptr;
