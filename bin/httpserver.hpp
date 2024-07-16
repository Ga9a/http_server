#include "tcpserver.hpp"
#include "../threadpool/threadpool.hpp"

using namespace std;

static const int default_port = 9090;

class httpserver
{
    public:
    //构造函数
    httpserver(int port = default_port): _port(port),_stop(false){};

    //运行函数
    void activate()
    {
        signal(SIGPIPE, SIG_IGN); // 服务端写客户端关闭，此时会导致服务端进程接收到SIGPIPE新型号，直接被os杀掉，忽略这个信号，防止因这种情况导致服务端关闭
        tcpserver::GetInstance(_port)->InitServer();
        LOG("server start to access connecion.",INFO);

        ConnectionRecv();
    };

    private:
    void ConnectionRecv()
    {
        while (!_stop)
        {
            sockaddr_in peer;               // 接收到客户端信息
            memset(&peer, 0, sizeof(peer)); // 初始化0
            socklen_t len = sizeof(peer);

            int ClientSock = accept(tcpserver::GetInstance(_port)->ListenSocketGet(),(sockaddr *)&peer, &len);

            if(ClientSock < 0)//如果接受客户端失败，则再次接受连接
            {
                LOG("receieve coonection failed.",ERROR);
                //continue;
            }
            // 获取客户端IP和port
            string ClientIP = inet_ntoa(peer.sin_addr);
            int ClientPort = ntohs(peer.sin_port);

            LOG(move("Get new connection, Socket[" + to_string(ClientSock) + "], CLient Message::" + ClientIP + ":" + to_string(ClientPort)),INFO);
        
            Task* t= new Task(ClientSock);
            ThreadPool::GetInstance()->PushTask(t);
        }
    }


    private:
    int _port;//设置服务器端口
    bool _stop;//标志服务器是否需要停止    
};