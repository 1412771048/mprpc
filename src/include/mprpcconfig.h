#pragma once
#include <unordered_map>
#include <string>


//框架读取配置文件类: rpcserver_ip,port   zookeeper_ip,port
class MprpcConfig {
public:
    MprpcConfig() = default;
    ~MprpcConfig() = default;
    void LoadConfig(); //解析配置文件
    const std::string load(const std::string& key); //通过key查询配置
private:
    std::unordered_map<std::string, std::string> config_map_;
};