#pragma once
#include "task.hpp"

const static size_t NUM = 6;

class ThreadPool
{
    private:
    locker _mtx; // 锁
    cond _cond; // 条件变量
    size_t _num; // 线程个数
    std::queue<Task*> _taskQueue;
    static ThreadPool* _tp; // 单例模式

    private:
    //构造函数，确定进程数量
    ThreadPool(size_t num=NUM) :_num(num)  {}
    //默认析构函数，互斥锁和条件变量会自动销毁
    ~ThreadPool(){};
    //为静态函数提供接口
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
    // 从线程取任务,输出型参数，外面获取队头任务
    void PopTask(Task* &task) // 
    {
        task = _taskQueue.front();
        _taskQueue.pop();
    }
    //关键函数，线程执行方法
    static void* ThreadRountine(void* args)
    {
        pthread_detach(pthread_self()); // 线程分离，不需要管理
        auto ptp = reinterpret_cast<ThreadPool*>(args); // 获取当前线程池的指针，从而调用某些接口

        while(1)
        {
            //从线程池申请锁
            ptp->lock();
            //然后等待信号，即任务队列里有任务，该过程会释放锁
            while(ptp->_taskQueue.empty()) 
            {
                ptp->waitcond();
            }
            // 到这里线程就要取任务来执行
            Task* t;
            ptp->PopTask(t);
            ptp->unlock();
            t->PorcessOn(); // 执行任务
            delete t;
            // 执行完毕后循环上去继续等任务到来
        }
    }  
    //初始化线程池
    void ThreadPoolInit()
    {
        LOG("ThreadPool start to init",INFO);
        for(size_t i = 0; i < _num; ++i)
        {
            pthread_t tid;
            //建立num个线程，每个线程都是执行函数ThreadRountine,然后会分离
            if(pthread_create(&tid, nullptr, ThreadRountine, this) != 0)
            {
                LOG("ThreadPool init failed(thread create failed)",ERROR);
                exit(5);
            }
        }
        LOG("ThreadPool init success",INFO);
    }
    public:
    //获得单例
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

    //主线程放任务
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