#include "CgiInclude.hpp"

const char* nmap_path ="nmap";

int main(int argc, char ** argv)
{
    std::string query_string;
    std::string content_lenth=argv[1];
    GetQueryString(GetMethod(), query_string , content_lenth); //获取参数
    auto para = KVconstructor(query_string);

    execl(nmap_path,nmap_path,para["ip"].c_str(),nullptr);
    CGILOG(ERROR,"execl failed");
}