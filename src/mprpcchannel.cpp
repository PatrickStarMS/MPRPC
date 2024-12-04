#include "mprpcchannel.h"
#include <string>
#include "rpcheader.pb.h"
#include <sys/types.h>
#include <sys/socket.h>
#include "mprpcapplication.h"
//添加网络的头文件
#include <arpa/inet.h>
#include <netinet/in.h>
//包含close
#include <unistd.h>

#include "mprpccontroller.h"

#include "zookeeperutil.h"
/*
header_size+service_name method_name args_size+args
*/

void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method,
                          google::protobuf::RpcController* controller, const google::protobuf::Message* request,
                          google::protobuf::Message* response, google::protobuf::Closure* done)
{
    //可以通过方法获取所属服务
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    //获取参数的序列化字符串长度 args_size
    uint32_t args_size = 0;
    std::string args_str;
    if(request->SerializeToString(&args_str))
    {
      args_size = args_str.size();
    } 
    else {
      
      controller->SetFailed("serialize request error!");
      return;
    }

    //定义rpc的请求header
    mprpc::RpcHeader rpcHeader;
    rpcHeader.set_service_name(service_name);
    rpcHeader.set_method_name(method_name);
    rpcHeader.set_args_size(args_size);

    //填好之后，序列化成字符串
    uint32_t header_size = 0;
    std::string rpc_header_str;
    if(rpcHeader.SerializeToString(&rpc_header_str))
    {
      header_size = rpc_header_str.size();
    }
    else{
       
      controller->SetFailed("serialize rpcHeader error!");
      return;
    }
    //现在将两个序列化好的数据拼在一起(x)，直接拼起来（按照字符存储）不行，按照字来存储，才能按照header_size正确解析4个字节，header是占4个字节不是字符，通过网络发出去
    std::string send_rpc_str;
    send_rpc_str.insert(0, std::string((char*)&header_size, 4));
    send_rpc_str += rpc_header_str;
    send_rpc_str += args_str;
    // 打印 调试信息
   std::cout << "=======================================" << std::endl;
   std::cout << "header_size:" << header_size << std::endl;
   std::cout << "rpc_header_str:" << rpc_header_str << std::endl;
   std::cout << "service_name:" << service_name << std::endl;
   std::cout << "method_name:" << method_name << std::endl;
   std::cout << "args_size:" << args_size << std::endl;
   std::cout << "args_str:" << args_str << std::endl;
   std::cout << "=========================================" << std::endl;

   //发送，户端不涉及高并发，因此进行简单的tcp编程即可，，不需要使用muduo库了
    //1.创建socket
   int clientfd = socket(AF_INET, SOCK_STREAM, 0);
   if (clientfd == -1) 
   {
     char errtxt[512] = {0};
     sprintf(errtxt,"create socket error! errno: %d", errno);
     controller->SetFailed(errtxt);
     return;
   }
   //2.创建sockAddr，主机的ip与port
        //服务器的ip和端口号
        // std::string ip =
        // MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
        // uint16_t port = atoi(
        // MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
        

        //使用zk服务：rpc调用方通过service_name method_name,去zkserver上查找所在host的ip与port
   std::string method_path = "/" +service_name + "/" + method_name;
   ZKClient zkCli;
   zkCli.Start();
   std::string host_data = zkCli.GetData(method_path.c_str());
   //127.0.0.1:8000
   //解析
   if(host_data=="")
   {
     controller->SetFailed(method_path + "  is not exist!");
     return;
   }
   
     int idx = host_data.find(":");
     if(idx==-1)
     {
      controller->SetFailed(method_path + "address os invalid!");
     return;
     }
     std::string ip = host_data.substr(0, idx);
     uint16_t port = atoi(host_data.substr(idx + 1, sizeof(host_data) - idx).c_str());
   

   // 网络编程
   sockaddr_in server_addr;
   //  memset(&server_addr, 0, sizeof(sockaddr_in));
   // 地址家族
   server_addr.sin_family = AF_INET;
   server_addr.sin_port = htons(port);
   // server_addr.sin_addr.s_addr =
   // inet_addr(ip.c_str());//如果报错，man看看是不是缺少头文件,看看使用哪个函数
   inet_pton(AF_INET, ip.c_str(), &server_addr.sin_addr);
   // std::cout << server_addr.sin_port << std::endl;

   // 连接
   if (connect(clientfd, (sockaddr*)&server_addr, sizeof(sockaddr_in)) == -1) {
     char errtxt[512] = {0};
     sprintf(errtxt,"connect error! errno:  %d", errno);
     controller->SetFailed(errtxt);
     close(clientfd);
     return;
    }

    //发送
    if(send(clientfd,send_rpc_str.c_str(),send_rpc_str.size(),0)==-1)
    {
      std::cout << "send error! errno: " << errno << std::endl;
      close(clientfd);
      return;
    }
    //接收请求的响应值
    char recv_buf[1024] = {0};
    int recv_size = 0;
    if (-1==(recv_size=(recv(clientfd, recv_buf, 1024, 0)))) 
    {
      
      char errtxt[512] = {0};
     sprintf(errtxt,"recv error! errno:   %d", errno);
     controller->SetFailed(errtxt);
      close(clientfd);
      return;
    }
    
    //进行反序列化
    // std::string response_str(send_rpc_str,0,recv_size);
    
    // if(response->ParseFromString(response_str)==-1)
    if(!response->ParseFromArray(recv_buf,recv_size))
    {
  
      char errtxt[512] = {0};
     sprintf(errtxt,"parse error! errno:   %s", recv_buf);
     controller->SetFailed(errtxt);
      
      close(clientfd);
      return;
    }
    close(clientfd);
}