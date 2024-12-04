#include "mprpcapplication.h"
#include <iostream>
#include <unistd.h>

MprpcConfig MprpcApplication::_mprpcConfig;

void ShowArgHelp() { std::cout << "command -i <configfile>" << std::endl; }

void MprpcApplication::Init(int argc,char** argv)
{
   //-i config.conf
   if(argc<2)
   {
     ShowArgHelp();
     exit(EXIT_FAILURE);
   }

   //读文件，使用getopt
   int c = 0;
   std::string config_file;
   while ((c = getopt(argc, argv, "i:")) != -1) {
     switch (c) {
       case 'i':
         config_file = optarg;
         break;
      case '?':
        std::cout << "invalid args!" << std::endl;
        exit(EXIT_FAILURE);
        break;
      case ':':
         std::cout << "need <configfile>!" << std::endl;
         ShowArgHelp();
         exit(EXIT_FAILURE);
      default:
        break;
      }
   }
   //开始加载配置文件了 rpcserver_ip= rpcserver_port= zookeeper_ip= zookeeper_port=
   _mprpcConfig.loadConfig(config_file.c_str());
   std::cout << "rpcserverip" << _mprpcConfig.Load("rpcserverip") << std::endl;
   std::cout << "rpcserverport" << _mprpcConfig.Load("rpcserverport") << std::endl;
   std::cout << "zookeeperip" << _mprpcConfig.Load("zookeeperip") << std::endl;
   std::cout << "zookeeperport" << _mprpcConfig.Load("zookeeperport") << std::endl;
}
MprpcApplication& MprpcApplication::GetInstance()
 {
    static MprpcApplication app;
    return app;
 }

 MprpcConfig& MprpcApplication::GetConfig()
{ 
  return _mprpcConfig;
}