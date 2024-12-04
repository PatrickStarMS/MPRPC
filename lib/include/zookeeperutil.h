#pragma once

#include <semaphore.h>
#include <zookeeper/zookeeper.h>
#include <string>
//封装zeekeeper的客户端类
class ZKClient
{
    public:
     ZKClient();
     ~ZKClient();

     //zkclient启动连接zkserver
     void Start();
     //zkserver上根据指定的path创建znode节点，state代表是永久的还是临时的，默认是0，永久性
     //c口的api
     void Create(const char *path,const char* data,int datalen,int state=0);
     //根据参数指定的znode路径获取zonde节点的值
     std::string GetData(const char* path);
     private:
     //zk的客户端句柄，通过客户端的句柄操作zkserver
      zhandle_t* m_zhandle;
};