#include "rpcprovider.h"
#include "mprpcapplication.h"
#include "rpcheader.pb.h"
#include "logger.h"
#include "zookeeperutil.h"
/*service_name =>service描述
                        service*记录服务对象
                        method_name=>method方法对象*/
//这里是框架提供给外界使用的，可以发布rpc方法的函数接口
//基类中有对服务对象的描述,多态，指向谁就是描述谁
 void RpcProvider::NotifyService(google::protobuf::Service *service)
 {
    //将注册的服务与方法，放到表中（成员变量），存储起来
   ServiceInfo service_info;

   // 返回值是const，所以加const
   const google::protobuf::ServiceDescriptor* pserviceDesc = service->GetDescriptor();
    
    //获取服务信息
    std::string service_name = pserviceDesc->name();
    //获取服务对象service的方法数量
    int methodCnt = pserviceDesc->method_count();
    LOG_INFO("service_name: %s", service_name.c_str());
    // 使用服务方法
    for (int i = 0; i < methodCnt;i++)
    {
        //获取服务对象指定下标的服务方法的描述（抽象描述）
      const google::protobuf::MethodDescriptor* pmethodDesc =
          pserviceDesc->method(i);

        //放入表中
      std::string method_name = pmethodDesc->name();
      //先填一行（先房间），再造表（造楼）
      service_info.m_methodMap.insert({method_name, pmethodDesc});
      LOG_INFO("method_name: %s", method_name.c_str());
    }
    service_info.m_service = service;
    m_serviceMap.insert({service_name, service_info});
    // std::cout << "注册成功" << std::endl;
 }

 //启动rpc服务节点，开始提供rpc远程网络服务
 void RpcProvider::run()
 {
   std::string ip =
       MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
   uint16_t port = atoi(
       MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());
   
  //  std::cout << "等待连接的ip" <<ip<< "等待连接的port"<<port<< std::endl;
   LOG_INFO("等待连接的iP:%s,等待连接的port:%d", ip.c_str(), port);
   // 设置启动tcpserver的参数
   muduo::net::InetAddress address(ip, port);
   //启动tcpserver
   muduo::net::TcpServer server(&m_eventLoop, address, "RpcServer");


   //绑定连接回调和消息读写回调方法，分离了网络和业务代码
   server.setConnectionCallback(std::bind(&RpcProvider::OnConnection,this,std::placeholders::_1));

    //绑定读写回调
   server.setMessageCallback(
       std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1,std::placeholders::_2,std::placeholders::_3));
   // 设置muduo库的线程数量
   server.setThreadNum(4);

//把当前rpc节点上要发布的服务全部注册到zk上面，让rpc client可以从zk上发现服务
// （就是之前用的是表存储的服务名字与对应的方法，现在是放到zk上去了），父节点是服务的名字，子节点名字是服务的方法，子节点内容是当前服务节点主机的ip和port
 //session timeout 30s zkclient   网络i/o线程 1/3*timeout时间发送ping消息
   ZKClient zkCli;
   zkCli.Start();
   //service_name为永久性节点，method_name为临时性节点
   for(auto& sp:m_serviceMap)
   {
     
     std::string service_path = "/" + sp.first;  // sp.first是服务名字
     
     zkCli.Create(service_path.c_str(), nullptr, 0);
     
     for(auto &mp :sp.second.m_methodMap)
     {
      // /service_name/method_name
      std::string method_name = service_path + "/" + mp.first;
      char method_path_data[128] = {0};
      sprintf(method_path_data, "%s:%d",ip.c_str(),port);
      //创建子节点
      
      zkCli.Create(
          method_name.c_str(), method_path_data, strlen(method_path_data),
          ZOO_EPHEMERAL  // 临时性节点，provider断开的时候，就不能提供该服务了
      );
     }
   }

   //启动网络服务
   server.start();
  //  std::cout << "等待连接" << std::endl;
   // 启动epoll进行检测
   m_eventLoop.loop();


 }
 void RpcProvider::OnConnection(const  muduo::net::TcpConnectionPtr& conn)
 {
  if(!conn->connected())
  {
    //连接断开，就把该连接关闭即可
    conn->shutdown();
  }
 }


 /*在框架内部，RpcProvider和RpcConsummer协商好了之间沟通用的protobuf数据类型
 service_name  method_name,args 定义proto的message类型（格式化，统一格式知道怎么解析），进行数据头序列化和反序列化
                            通过service_name找到对应的对应的服务描述，
                            通过method_name在服务描述中找到对应的方法描述
                             service_name  method_name,args_size防止粘包
 16UserSviceLoginzhang san123456
传过来的数据组成（两边定义好的）header_size(4字节)+header_str+args_str来正确的读取各部分
  10  "10"
  1000 "1000"
  字符串四字节可长可短，不能直接编程整数，不知道4字节表示的是哪几位，但是在内存中是一样的，前四个字节的内容,
  因此拷贝前4个字节,就是定义的rpcheader
  std::string  insert和copy方法
  */
 void RpcProvider::OnMessage(const  muduo::net::TcpConnectionPtr& conn,
                           muduo::net::Buffer* buffer, muduo::Timestamp timestamp)
{
  //网络上接受的远程rpc调用请求的字符流 Login args
   std::string recv_buff = buffer->retrieveAllAsString();
   //从buff里面按照上面规定好的格式（使用protoc生成的rpcheader定义好）解析数据

   //从字符流中读取前四个字节的内容
   uint32_t header_size = 0;
   recv_buff.copy((char*)&header_size, 4, 0);
  //  std::cout << "server recv_buff"<<recv_buff << std::endl;

   //根据header_size读取数据头的原始字符流,先看开始的位置包含不包含
   std::string rpc_header_str = recv_buff.substr(4, header_size);

  //反序列化数据，得到rpc请求的详细信息
   mprpc::RpcHeader rpcheader;
   std::string service_name;
   std::string method_name;
   uint32_t args_size;
   if (rpcheader.ParseFromString(rpc_header_str)) {
     std::cout << "序列头反序列化成功" << std::endl;
     //取数据
     service_name = rpcheader.service_name();//proto根据变量名自动生成的读写方法
     method_name = rpcheader.method_name();
     args_size = rpcheader.args_size();
   } else {
     std::cout << "序列头反序列化失败" <<"rpc_header_str"<<rpc_header_str<< std::endl;
     return;
   }
   //解析传过来的参数信息
   std::string args_str = recv_buff.substr(4 + header_size, args_size);
   // 打印 调试信息
   std::cout << "=======================================" << std::endl;
   std::cout << "header_size: " << header_size << std::endl;
   std::cout << "rpc_header_str:  " << rpc_header_str << std::endl;
   std::cout << "service_name:  " << service_name << std::endl;
   std::cout << "method_name: " << method_name << std::endl;
   std::cout << "args_size: " << args_size << std::endl;
   std::cout << "args_str:  " << args_str << std::endl;
   std::cout << "=========================================" << std::endl;

   //获取service对象和method对象
   auto it = m_serviceMap.find(service_name);
  if(it==m_serviceMap.end())
  {
    std::cout << service_name<<"service_name id not exist" << std::endl;
    return;
  }
  auto mit = it->second.m_methodMap.find(method_name);
  if(mit==it->second.m_methodMap.end())
  {
    std::cout << service_name<<method_name<<"method_name id not exist" << std::endl;
    return;
  }
  //看声明（声明是看的原始定义）来写
   google::protobuf::Service* service = it->second.m_service;//获取dervice对象 比如new UserService
   const google::protobuf::MethodDescriptor* method = mit->second;//获取method对象 比如Login

   //生成rpc方法调用的请求request和响应response参数，抽象层面
    // protobuf提供了抽象类来统一表示请求和响应,
   google::protobuf::Message* request =
       service->GetRequestPrototype(method).New();
    if(!request->ParseFromString(args_str))
    {
      std::cout << "request parse error" << args_str << std::endl;
      return;
    }

   google::protobuf::Message* response =
       service->GetResponsePrototype(method).New();

  //在框架上根据远端rpc请求，调用当前rpc节点上发布的方法，通过调用抽象类，将方法名，参数都传入
  //相当于new UserService().Login(controller，request，resopnse，done)，（而且将参数传入）
      //给下面的method方法的调用，绑定一个Closure的回调函数
      //SendRpcResponse是成员方法，需要绑定对象，NewCallback有对应的重载版本
      //推到失败的情况下，可以自己指明；如果不认识，看看1.头文件；2.作用域
      //或者写一个Closure的派生类对象，或者lambda生成匿名对象，重写run方法
   google::protobuf::Closure* done=google::protobuf::NewCallback<RpcProvider,const muduo::net::TcpConnectionPtr&,google::protobuf::Message*>
   (this,&RpcProvider::SendRpcResponse,conn,response);

   service->CallMethod(method, nullptr,request,response,done);
 }


 void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn,
                     google::protobuf::Message* response)
{
   std::string response_str;
   if(response->SerializeToString(&response_str))
   {
    //序列化成功后，通过网络把rpc方法执行的结果发送回rpc的调用方
    
    conn->send(response_str);

   }
   else
   {
     std::cout << "serialize response_str error!" << std::endl;
     
   }
    //短链接，发送完就断开
    conn->shutdown();
 }