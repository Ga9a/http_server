#pragma once
#include "tcpserver.hpp"
#include "../threadpool/threadpool.hpp"

using namespace std;

static const int default_port = 9090;

class httpserver
{
    public:

    httpserver(int port = default_port): _port(port),_stop(false){};

    void activate()
    {
        signal(SIGPIPE, SIG_IGN);
        tcpserver::GetInstance(_port)->InitServer();
        LOG("server start to access connecion.",INFO);

        ConnectionRecv();
    };

    private:
    void ConnectionRecv()
    {
        while (!_stop)
        {
            sockaddr_in peer;
            memset(&peer, 0, sizeof(peer));
            socklen_t len = sizeof(peer);

            int ClientSock = accept4(tcpserver::GetInstance(_port)->ListenSocketGet(),(sockaddr *)&peer, &len, SOCK_CLOEXEC);

            if(ClientSock < 0)
            {
                LOG("receieve coonection failed.",ERROR);
                continue;
            }

            string ClientIP = inet_ntoa(peer.sin_addr);
            int ClientPort = ntohs(peer.sin_port);

            LOG(std::move("Get new connection, Socket[" + to_string(ClientSock) + "], CLient Message::" + ClientIP + ":" + to_string(ClientPort)),INFO);

            timeval timeout{10, 0};
            setsockopt(ClientSock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
            setsockopt(ClientSock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));
            Task* t= new Task(ClientSock);
            ThreadPool::GetInstance()->PushTask(t);
        }
    }

    private:
    int _port;
    bool _stop;
};
