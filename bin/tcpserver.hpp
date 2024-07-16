#pragma once
#include "include.hpp"

using namespace std;

const static int WAIT_LENGTH = 5;//listen函数连接请求队列长度

class tcpserver
{
    private:
    int _port;       // 端口
    int _listenSock; // 监听套接字

    //懒汉模式
    private:
    static tcpserver *_tcpserv; //单例

    //私有化构造函数，防止外部初始化
    tcpserver(int port) : _port(port),_listenSock(-1) {};

    //显示禁止拷贝和赋值
    tcpserver (const tcpserver& cp) = delete;//构造函数形式，禁止拷贝
    tcpserver &operator= (const tcpserver& cp) = delete;//运算符重载，禁止使用=赋值

    public:
    //单例模式用于获得对象
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
    
    //服务器套接字编程初始化三件套
    void InitServer(){
        SocketCreate();
        SocketBind();
        SocketListen();
    }

    void SocketCreate(){
        _listenSock = socket(AF_INET,SOCK_STREAM,0);//tcp
        if (_listenSock < 0)
        { // 创建监听套接字失败
        LOG("create listen sock failed.",FATAL);
        exit(1);
        }

        int opt = 1;
        // 设置地址复用，防止服务器崩掉后进入TIME_WAIT，短时间连不上当前端口
        setsockopt(_listenSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
        LOG( "create listen sock successfully.",INFO);
    }

    void SocketBind(){
        sockaddr_in local;
        memset(&local, 0, sizeof(local));   // 给sin_zero置0
        local.sin_family = AF_INET;         // 协议家族和创建socket时相同
        local.sin_port = htons(_port);      // 主机转网络字节序
        local.sin_addr.s_addr = INADDR_ANY; // 将服务端IP直接给为0.0.0.0，能接受到任意网卡传来的信息
        if (bind(_listenSock, (sockaddr *)&local, sizeof(local)) < 0)
        { // 绑定失败
        LOG("bind failed.",INFO);
        exit(2);
        }
        LOG("bind successfully.",INFO);
    }

    void SocketListen(){
        if (listen(_listenSock, WAIT_LENGTH) < 0)
        { // 监听失败
        LOG("listen fail.",INFO);
        exit(3);
        }
        LOG("listen successully.",INFO);
    }

    //获得监听套接字
    int ListenSocketGet(){
       return _listenSock;    
    }

    //析构函数，关闭套接字
      ~tcpserver(){
        if(_listenSock !=-1){
        close(_listenSock);
        }
    }


};

tcpserver *tcpserver::_tcpserv = nullptr;// 类外初始化静态成员变量，创建单例模式对象