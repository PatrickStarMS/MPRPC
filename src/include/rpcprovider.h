#pragma once
#include "google/protobuf/service.h"
// #include <memory>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <functional>
#include <iostream>
#include <google/protobuf/descriptor.h>
#include <unordered_map>
//框架提供的专门提供发布的rpc服务的网络对象类
class RpcProvider 
{
public:
//这里是框架提供给外界使用的，可以发布rpc方法的函数接口
 void NotifyService(google::protobuf::Service *service);

 //启动rpc服务节点，开始提供rpc远程网络服务
 void run();
private:
//组合了TcpServer，这里只有run一个函数使用了Tcpserver，可以写成局部变量，
//eventloop不行，多个方法需要访问它，因此是成员变量
//  std::unique_ptr<muduo::net::TcpServer> m_tcpserverPtr; 
//组合了EventLoop(就是epoll) EventLoop 对象并不由 RpcProvider 所拥有,是main中所有的，
//它的生命周期可能跨越多个 RpcProvider 对象的生命周期
 muduo::net::EventLoop m_eventLoop;

//服务类型信息---对服务的描述，不止一个服务，一个服务不止一个方法-------》房间：房间号+人（不止一个）
struct ServiceInfo
{
    google::protobuf::Service *m_service;//保存服务对象
    std::unordered_map<std::string, const google::protobuf::MethodDescriptor*>
        m_methodMap;//保存服务方法，一个服务对象不止一个服务方法，通过对应的服务名字是用
};

//一个服务名字对应一个服务描述信息，通过名字调用-----》第几栋楼
std::unordered_map<std::string, ServiceInfo> m_serviceMap;

// 连接新socket连接回调
void OnConnection(const muduo::net::TcpConnectionPtr& conn);
// 读写事件回调,如果远程有一个rpc服务的调用请求，那么OnMessage就会响应 
void OnMessage(const muduo::net::TcpConnectionPtr& conn,
               muduo::net::Buffer* buffer, muduo::Timestamp timestamp);
//Closure的回调操作，用于序列化rpc的响应和网络发送,发送响应需要网络，一次传入的是网络连接与response
void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,
                     google::protobuf::Message* message);
};