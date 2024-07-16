#pragma once
#include<iostream>
#include<string>

using namespace std;

enum LogLevel
{
	INFO,
	WARNING,
	ERROR,
	FATAL
};

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define RELATIVE_PATH(file) (std::string(file).substr(std::string(SOURCE_ROOT).length() + 1))

#define LOG(message, level) Log(#level,message,RELATIVE_PATH(__FILE__),__LINE__)

void Log(const string& level,const string& message,const string& filename,long line)
{
    //日志格式：[level](filename:line)  message (time)
    time_t t;
    time(&t);
    cout << "[" << level << "](" << filename << ":" << line << ") " << message << "\t" << ctime(&t);
}