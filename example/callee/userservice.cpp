#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"
#include "logger.h"
//UserService原来是一个本地服务，提供了两个进程内的本地方法，Login和GetFriendLists
//注意proto生成的代码是在命名空间fixbu中，不加命名空间会找不到的
class UserService:public fixbug::UserServiceRpc//使用在rpc服务发布端（rpc服务提供者）
{
public:
    bool Login(std::string name,std::string pwd)
    {
      std::cout << "doing local service: Login " << std::endl;
      std::cout << "name:" << name << "   pwd: " << pwd << std::endl;
      return true;
    }

    //下面的这些方法都是框架直接调用的
    //要重写基类中的rpc服务方法Login,注意这个login是proto中的服务方法和上面这个不是同一个
    //这两个虽然同名，但是返回值和传参有一个不一样就行，这里传参一般数不一样的
    //
    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        //4套动作
        //框架给业务上报了请求参数LoginRequest,应用层直接从框架传来的请求中获取相应数据做本地业务
        std::string name = request->name();
        std::string pwd = request->pwd();
        //有效性检查

        //调用本地方法做本地业务
        bool login_result = Login(name, pwd);

        //把业务处理结果填入响应response指针，交给框架，其他的不用管了，框架会完成，并通过网络返回回去
        //先写错误码，再写错误消息、返回值
        fixbug::ResultCode* code = response->mutable_result();
        code->set_errcode(0);
        code->set_errmsg("");
        response->set_success(login_result);

        //调用执行回调操作（这里再包装一层提供一个统一的接口），通过纯虚函数实现各异化操作
        //Run方法是纯虚函数，需要重写，包括序列化，网络发送（都是由框架完成的），
        //对应上面网络发送与反序列化是有框架完成的，应用层直接使用
        done->Run();
    }
};

//要调用远程的方法：1.远程方法的名字 2.远程方法的参数 3.远程方法的返回值
//这里选择protobuf来选择将上面进行序列化和反序列化
//protobuf 生成的两个UserServiceRpc  UserServiceRpc_Stub
int main(int argc ,char** argv)
{
    //调用框架的初始化操作,比如配置，日志、像之前的推理框架的内存分配等等（这是一个套路）
    //rpc站点也是一个服务器，需要ip和port，不能在代码上写死,都是从配置文件而来，因此传递参数
    MprpcApplication::Init(argc ,argv);

    //创建rpc服务对象，把UserService对象发布到rpc节点上（类似于redis的发布订阅）
    RpcProvider provider;
    provider.NotifyService(new UserService());
    

    //启动一个rpc服务节点，进入阻塞状态，等待远程的rpc调用请求
    provider.run();
    
}