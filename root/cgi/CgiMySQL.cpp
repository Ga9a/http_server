#include "CgiInclude.hpp"
#include "../../bin/outheader/mysql.h"

const char* error_page_path="../root/mysqlerror.html";
const char* fun_path="../root/penetration.html";

const char* host="127.0.0.1";
const char* user="root";
const char* password="1357488";
const char* db="yourdb";

const int timeout = 3;//三秒超时连接数据库

bool UserCheck(std::unordered_map<std::string,std::string> para);
void SendPage(const char* path);
int main(int argc, char ** argv)
{
    
    //mysql_init(&mysql);

    std::string query_string;
    std::string content_lenth=argv[1];
    GetQueryString(GetMethod(), query_string , content_lenth); //获取参数
    auto para = KVconstructor(query_string);
    //检查密码有效性
    if(UserCheck(para)) 
        SendPage(fun_path);
    else
        SendPage(error_page_path);

}
//数据库存储正确密码，password是用户输入的密码
bool UserCheck(std::unordered_map<std::string,std::string> para)
{
    MYSQL mysql;
    bool result=true;
    mysql_library_init(0,0,0);
    mysql_init(&mysql);

    mysql_options(&mysql,MYSQL_OPT_CONNECT_TIMEOUT,&timeout);

    if(!mysql_real_connect(&mysql,host,user,password,db,3306,0,0))
    {
        std::cerr << "mysql init error: " << mysql_error(&mysql) << endl;
    }
    else
    {
        std::cerr << "mysql connect success" << endl;
    };
std::cerr << para["username"] << std::endl;
    char request[1024];
    sprintf(request, "select * from user where username = '%s'", "agni");
    mysql_query(&mysql,request);

    MYSQL_RES* res = mysql_store_result(&mysql);

    if(!mysql_num_rows(res)) 
    {
        std::cerr << "username is not registed!" << std::endl;
        result = false;
    }
    else
    {
    MYSQL_ROW row;
    row = mysql_fetch_row(res);
    if(row[1]!=para["pwd"]) 
    {
        std::cerr <<  "real: "<<row[1]<<",input: "<< para["pwd"]<< std::endl;
        std::cerr << "password is wrong!" << std::endl;
        result = false;
    }
    }
    mysql_close(&mysql);
    mysql_library_end();
    return result;
}
void SendPage(const char* path)
{
        // 打开HTML文件
    std::ifstream file(path);
    
    // 检查文件是否成功打开
    if (!file.is_open()) {
        std::cerr << "Failed to open file." << std::endl;
        return;
    }
    // 读取文件内容并输出到控制台
    std::string line;
    while (std::getline(file, line)) {
        std::cout << line << std::endl;
    }
    // 关闭文件
    file.close();
}