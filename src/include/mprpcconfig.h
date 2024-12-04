#pragma once
#include <unordered_map>
#include <string>
//框架配置文件类
// rpcserver_ip= rpcserver_port= zookeeper_ip= zookeeper_port=
class MprpcConfig 
{

public:
//负责加载解析配置文件
 void loadConfig(const char* config_file);
//从map中取value,查询配置项信息，引用效率快
 std::string Load(const std::string &key);

private:
 std::unordered_map<std::string, std::string> m_configMap;
 void Trim(std::string &src_buf);
};