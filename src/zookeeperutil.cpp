#include "zookeeperutil.h"
#include "mprpcapplication.h"
#include <semaphore.h>
#include <iostream>


//全局的watcher观察器 zkserver给zkclient的通知      单独的线程（三个线程其中的一个）
void global_watcher(zhandle_t *zh,int type,int state,const char* path,void* watcherCtx)
{
    if(type==ZOO_SESSION_EVENT)//回调的消息类型是和会话相关的消息类型（连接断开）
    {
        if(state==ZOO_CONNECTED_STATE)//zkclient和zkserver连接成功
        {
          sem_t* sem = (sem_t*)zoo_get_context(zh);//获取上下文中的信号量
          sem_post(sem);
        }
    }
}


ZKClient::ZKClient():m_zhandle(nullptr)
{

}
ZKClient::~ZKClient()
{
    if(m_zhandle!=nullptr)
    {
      zookeeper_close(m_zhandle);//像mysql一样，关闭句柄释放资源
    }
}

//连接zkserver
void ZKClient::Start()
{
  std::string host =
      MprpcApplication::GetInstance().GetConfig().Load("zookeeperip");
  std::string port =
      MprpcApplication::GetInstance().GetConfig().Load("zookeeperport");
  std::string connstr = host + ":" + port;
  /*异步的，只有得到zkserver响应的时候才调用回调
  zookeeper_mt多线程版本
  zookeeper的API客户端提供了3个线程
  API调用线程调用zookeeper_init
  网络io线程zookeeper_init里面创建线程调用网络io（不是直接调用网络io的）poll
  watcher 创建线程，当链接zkserver成功的时候调用回调函数
  */
 //1.创建句柄（内存开辟初始化）成功，内部创建线程调用网络io，连接zkserver，并不是连接zkserver成功，成功连接zkserver时候，调用回调
  m_zhandle = zookeeper_init(connstr.c_str(), global_watcher, 30000, nullptr,
                             nullptr, 0);
    if(m_zhandle==nullptr)
    {
    std::cout << "zookeeper_init error" << std::endl;
    exit(EXIT_FAILURE); 
    }

    sem_t sem;
    sem_init(&sem, 0, 0);
    // 2。创建上下文，给zookeeper传参数   （给句柄传额外的参数，把信号量带入） 
    zoo_set_context(m_zhandle, &sem);
    //3.刚开始sem是0，因此这里是阻塞的，直到成功连接到zkserver，调用回调，在回调中，信号量+1，这里才执行
    sem_wait(&sem);

    std::cout << "zookeeper_init success!" << std::endl;
}

//创建节点
void ZKClient::Create(const char* path,const char* data,int datalen,int state)
{
   
  char path_buffer[128];
  int bufferlen = sizeof(path_buffer);
  int flag;
  //同步的，判断指定路径的znode节点是否存在
  flag = zoo_exists(m_zhandle, path, 0, nullptr);
  // 如果不存在，则创建  
  if (flag == ZNONODE) {
    // std::cout << data << std::endl;
    flag = zoo_create(m_zhandle, path, data, datalen,
                      &ZOO_OPEN_ACL_UNSAFE,  // 权限有关
                      state,                 // 永久性还是临时性节点
                      path_buffer, bufferlen);
    if(flag==ZOK)//zok代表操作成功
    {
      std::cout << "znode create success... path:" << path << std::endl;

    }
    else
    {
      std::cout << "flag:" << flag << std::endl;
      std::cout << "znode create error... path:" <<path<< std::endl;
      exit(EXIT_FAILURE); 
    }
  }
}
//根据指定的path获取节点的值
std::string ZKClient::GetData(const char* path)
{
    char buffer[64];
    int bufferlen = sizeof(buffer);
    int flag;
    flag = zoo_get(m_zhandle, path, 0, buffer, &bufferlen, nullptr);
    if(flag!=ZOK)
    {
      std::cout << "get znode error...path  " << path << std::endl;
      return "";
    }
    else{
      return buffer;
    }
}
