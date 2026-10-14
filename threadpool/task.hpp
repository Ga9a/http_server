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
        char buff[2049];
      size_t size = read(sock, buff, sizeof(buff) - 1);
      if(size > 0)
      {
        buff[size] = 0;
        cout << "--------------------------------------" << endl;
        cout << buff << endl;
        cout << "--------------------------------------" << endl;
      }
      else if(size == 0)
      {
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
        int _sock;
        ThreadCallBack _Threadcb;

    public:

        Task(int sock=-1) : _sock(sock)
        {
            cout << "socket[" << _sock << "] task start!============================" <<endl;
        }

        ~Task()
        {
            cout << "socket[" << _sock << "] task end!============================" <<endl;
            if (_sock!=-1) close(_sock);
        }

        void PorcessOn()
        {
            _Threadcb(_sock);
        }

};
