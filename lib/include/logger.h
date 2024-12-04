#pragma once
#include "lockqueue.h"
#include <string>
enum LogLeverl  // 日志级别
{
  INFO,
  ERROR,
};

//Mprpc框架提供的日志系统
class Logger
{
public:
//设置日志级别
 void setLogLervel(LogLeverl level);
 //写日志
 void Log(std::string msg);

 static Logger& GetInstance();

private:
//单例
 Logger();
 Logger(const Logger&) = delete;
 Logger(Logger &&) = delete;
 int m_loglevel;                  // 记录日志级别
 LockQueue<std::string> m_lockQue;//日志缓冲队列
};

//定义宏方便使用,相当于一个函数
#define LOG_INFO(logmsgformat, ...)                 \
  do {                                              \
    Logger &logger = Logger::GetInstance();         \
    logger.setLogLervel(ERROR);                      \
    char c[1024] = {0};                             \
    snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
    logger.Log(c);\
}while (0);

//定义宏方便使用
#define LOG_ERROR(logmsgformat, ...)                 \
  do {                                              \
    Logger &logger = Logger::GetInstance();         \
    logger.setLogLervel(INFO);                      \
    char c[1024] = {0};                             \
    snprintf(c, 1024, logmsgformat, ##__VA_ARGS__); \
    logger.Log(c);\
}while (0);