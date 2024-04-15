#include <iostream>
#include "../../src/include/mprpcapplication.h"
#include "../user.pb.h"
#include "../../src/include/mprpcchannel.h"

//客户端，调用rpc: 先初始化框架，再调用stub类方法
int main(int argc, char** argv) {
    MprpcApplication::Init(argc, argv); //单例
    //调用端需要用stub类,传入一个RpcChanel，我们重写它，传入子类
    fixbug::UserServiceRpc_Stub stub(new MprpcChannel); //RpcChanel就是个统一的接口

    fixbug::LoginRequest* request;
    fixbug::LoginResponse* response;
    request->set_name("zhang san");
    request->set_pwd("123456");
    //同步阻塞的rpc调用：MprpcChannel::CallMethod
    stub.Login(nullptr, request, response, nullptr); //stub调用所有的rpc方法底层都是调用你传入的RpcChanel::CallMethod
    //rpc调用完成，读response
    if (response->result().errcode() == 0) {
        std::cout << "rpc login response success: " << response->sucess() << std::endl;
    } else {
        std::cerr << "rpc login response error: " << response->result().errmsg() << std::endl;
    }
}
