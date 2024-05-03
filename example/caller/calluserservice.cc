#include "rpcprovider.h"

#include "user.pb.h"

//客户端(rpc调用方)编程示例：
int main(int argc, char** argv) {
    //1.初始化框架
    auto& config = mprpc::MpRpcConfig::GetInstance();
    config.Init(argc, argv);
    std::cout << "zookeeper_ip: " << config.QuerryConfig("zookeeper_ip") << std::endl;
    std::cout << "zookeeper_port:" << config.QuerryConfig("zookeeper_port") << std::endl;
    
    //2. 创建stub类，用我们写的channel初始化它
    mprpc::MpRpcChannel channel;
    fixbug::UserServiceRpc_Stub stub(&channel);
    mprpc::MprpcController controller; //记录rpc调用中的状态和错误信息

    //3. 装填LoginRequest
    fixbug::LoginRequest login_request;
    login_request.set_name("zhangsan");
    login_request.set_pwd("123456");

    //4.调用rpc方法
    fixbug::LoginResponse login_response;
    //所有的rpc调用底层都是走MpRpcChannel重写的CallMethod函数
    stub.Login(&controller, &login_request, &login_response, nullptr);
    if (controller.Failed()) {//调用过程中出错
        std::cout << controller.ErrorText() << std::endl;
    } 
    else if (login_response.success()) {//rpc调用成功,再看业务执行情况
        std::cout << "login sucess!" << std::endl;
        //xxx
        //xxx
    } else {
        std::cerr << login_response.errmsg() << std::endl;
        //xxx
        //xxx
    }
    
        
    // fixbug::RegisterRequest register_request;
    // register_request.set_name("lisi");
    // register_request.set_pwd("123456");
    // fixbug::RegisterResponse register_response;

    // controller.Reset();
    // stub.Register(&controller, &register_request, &register_response, nullptr);
    // if (controller.Failed()) {
    //     std::cout << controller.ErrorText() << std::endl;
    // } else {
    //     if (register_response.result().errcode() == 0) {
    //         std::cout << "rpc register success: " << register_response.sucess() << std::endl;
    //     } else {
    //         std::cout << "rpc login error: " << register_response.result().errmsg() << std::endl;
    //     }
    // }
}