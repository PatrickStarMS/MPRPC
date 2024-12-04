#include "logger.h"
#include <time.h>
#include <iostream>

Logger::Logger()
{
    //启动专门的写日志线程,定义线程的职责，
    std::thread writeLogTask([&]() {
        for(;;)
        {
            //去当前的日期，然后获取日志信息，写入相应的日志文件当中 a+
            time_t now = time(nullptr);
            tm* nowtm = localtime(&now);

            //获取日志
            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year + 1900,
                    nowtm->tm_mon + 1, nowtm->tm_mday);
            FILE* pf = fopen(file_name, "a+");
            if(pf==nullptr)
            {
              std::cout << "logger file : " << file_name << " open error"
                        << std::endl;
              exit(EXIT_FAILURE);
            }

            //往文件写内容
                //从队列取出一条，放入文件
            std::string msg = m_lockQue.Pop();
            //加上具体的时间
            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d=>[%s]", nowtm->tm_hour, nowtm->tm_min,
                     nowtm->tm_sec,(m_loglevel==INFO?"info":"error"));
            msg.insert(0, time_buf);
            msg.append("\n");
            fputs(msg.c_str(), pf);
            

            fclose(pf);
        }
    });
    //设置分离线程
    writeLogTask.detach();
}

//设置日志级别
void Logger::setLogLervel(LogLeverl level) { m_loglevel = level; }
// 写日志,把日志信息写道缓冲区
void Logger::Log(std::string msg) 
{
    m_lockQue.Push(msg);
}

Logger& Logger::GetInstance()
{
    static Logger logger;
    return logger;//返回值引用，不是返回指针
}