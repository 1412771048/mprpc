#include "mprpcapplication.h"
#include <iostream>
#include <getopt.h>
#include <string>

MprpcConfig MprpcApplication::config_;


void MprpcApplication::Init(int argc, char **argv) {
    //加载配置文件，就不要在这个类里干了，再开一个config类
    config_.LoadConfig();//把配置文件写入map表了
    std::cout << "rpcserverip: " << config_.load("rpc_server_ip") << std::endl;
    std::cout << "rpcserverport: " << config_.load("rpc_port") << std::endl; 
    std::cout << "zookeeperip: " << config_.load("zookeeper_server_ip") << std::endl;
    std::cout << "zookeeperport: " << config_.load("zookeeper_port") << std::endl;
}

MprpcApplication& MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}

MprpcConfig& MprpcApplication::GetConfig() {
    return config_;
}