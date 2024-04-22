#include <iostream>
#include "rpcchannel.h"
#include "rpcprovider.h"

#include "user.pb.h"

//客户端编程示例：
int main(int argc, char** argv) {
    RpcProvider::Init(argc, argv);
    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)RpcProvider::Lock("config_map", RpcProvider::READ);
    std::cout << "rpc_server_ip: " << (*config_map_ptr)["rpc_server_ip"] << std::endl;
    std::cout << "rpc_server_port: " << (*config_map_ptr)["rpc_server_port"] << std::endl;
    std::cout << "zookeeper_server_ip: " << (*config_map_ptr)["zookeeper_server_ip"] << std::endl;
    std::cout << "zookeeper_server_port:" << (*config_map_ptr)["zookeeper_server_port"] << std::endl;
    RpcProvider::Unlock(RpcProvider::READ);

    //客户端就用stub类调用rpc服务
    fixbug::UserServiceRpc_Stub stub(new MpRpcChannel);
    fixbug::LoginRequest login_request;
    login_request.set_name("zhangsan");
    login_request.set_pwd("123456");
    fixbug::LoginResponse login_response;

    //所有的rpc调用底层都是走MpRpcChannel重写的CallMethod函数
    stub.Login(nullptr, &login_request, &login_response, nullptr);
    if (login_response.result().errcode() == 0) {
        std::cout << "rpc login success: " << login_response.sucess() << std::endl;
    } else {
        std::cout << "rpc login error: " << login_response.result().errmsg() << std::endl;
    }
        
    fixbug::RegisterRequest register_request;
    register_request.set_name("lisi");
    register_request.set_pwd("123456");
    fixbug::RegisterResponse register_response;
    stub.Register(nullptr, &register_request, &register_response, nullptr);
    if (register_response.result().errcode() == 0) {
        std::cout << "rpc register success: " << register_response.sucess() << std::endl;
    } else {
        std::cout << "rpc login error: " << register_response.result().errmsg() << std::endl;
    }
}