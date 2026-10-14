#pragma once
#include "include.hpp"

using namespace std;

const static int WAIT_LENGTH = 5;

class tcpserver
{
    private:
    int _port;
    int _listenSock;

    private:
    static tcpserver *_tcpserv;

    tcpserver(int port) : _port(port),_listenSock(-1) {};

    tcpserver (const tcpserver& cp) = delete;
    tcpserver &operator= (const tcpserver& cp) = delete;

    public:

    static tcpserver *GetInstance(int port){
        if(_tcpserv == nullptr)
        {
            static locker mtx;
            mtx.lock();
            if(_tcpserv == nullptr)
            {
                _tcpserv = new tcpserver(port);
            }
            mtx.unlock();

        }
        return _tcpserv;
    }

    void InitServer(){
        SocketCreate();
        SocketBind();
        SocketListen();
    }

    void SocketCreate(){
        _listenSock = socket(AF_INET,SOCK_STREAM | SOCK_CLOEXEC,0);//tcp
        if (_listenSock < 0)
        {
        LOG("create listen sock failed.",FATAL);
        exit(1);
        }

        int opt = 1;

        setsockopt(_listenSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        LOG( "create listen sock successfully.",INFO);
    }

    void SocketBind(){
        sockaddr_in local;
        memset(&local, 0, sizeof(local));
        local.sin_family = AF_INET;
        local.sin_port = htons(_port);
        local.sin_addr.s_addr = INADDR_ANY;
        if (::bind(_listenSock, (sockaddr *)&local, sizeof(local)) < 0)
        {
        LOG("bind failed.",INFO);
        exit(2);
        }
        LOG("bind successfully.",INFO);
    }

    void SocketListen(){
        if (listen(_listenSock, WAIT_LENGTH) < 0)
        {
        LOG("listen fail.",INFO);
        exit(3);
        }
        LOG("listen successully.",INFO);
    }

    int ListenSocketGet(){
       return _listenSock;
    }

      ~tcpserver(){
        if(_listenSock !=-1){
        close(_listenSock);
        }
    }

};

tcpserver *tcpserver::_tcpserv = nullptr;
