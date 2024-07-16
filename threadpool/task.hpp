#pragma once

#include "../bin/include.hpp"
#include "../httpmsg/protocal.hpp"

class ThreadCallBack 
{
    public:
        void operator()(int sock){
            Handler(sock);
        }

    static void* Handler(int sock){
        LOG("thread[" + to_string(pthread_self()) + "] handling socket[" + to_string(sock) + "] start",INFO);
    

      //#define DEBUG 1
      #ifdef DEBUG
      debug_Handler(sock);
      return nullptr;
      #else
      shared_ptr<ServerPoint> serv_p(new ServerPoint(sock));
      if(serv_p->RecvRequest())
      {
          serv_p->BuildResponse();
          LOG("build response success",INFO);
          serv_p->SendResponse();
          LOG("send response success",INFO);
      }
      return nullptr;
      #endif
      LOG("thread[" + std::to_string(pthread_self()) + "] handling sock[" + std::to_string(sock) + "] over",INFO);
      return nullptr;
    }

    static void debug_Handler(int sock)
    {
        char buff[2048];
      size_t size = read(sock, buff, sizeof(buff));
      if(size > 0)
      { // 读取到数据
        buff[size] = 0;
        cout << "--------------------------------------" << endl;
        cout << buff << endl;
        cout << "--------------------------------------" << endl;
      }
      else if(size == 0)
      { // 对端关闭
        cout << "client closed" << endl;
      }
      else 
      {
        cout << "read err" << endl;
      }
    }
};
class Task
{
    private:
        int _sock; // 通信套接字
        ThreadCallBack _Threadcb; //线程反馈

    public:
        //构造函数
        Task(int sock=-1) : _sock(sock)
        {
            cout << "socket[" << _sock << "] task start!============================" <<endl;
        }

        //析构函数,关闭套接字
        ~Task()
        {
            cout << "socket[" << _sock << "] task end!============================" <<endl;
            if (_sock!=-1) close(_sock);
        }

        //调用线程反馈伪函数查看报文
        void PorcessOn()
        {
            _Threadcb(_sock); // 调用ThreadCallback的仿函数，从而直接调用处理Http请求的逻辑
        }


};