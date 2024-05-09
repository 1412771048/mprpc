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
    fixbug::RegisterService_Stub stub(&channel);
    mprpc::MprpcController controller; //记录rpc调用中的状态和错误信息

    //3. 装填LoginRequest
    fixbug::RegisterRequest register_request;
    register_request.set_name("wang wu");
    register_request.set_password("123456");

    //4.调用rpc方法
    fixbug::RegisterResponse register_response;
    //所有的rpc调用底层都是走MpRpcChannel重写的CallMethod函数
    stub.Register(&controller, &register_request, &register_response, nullptr);
    //调用过程中出错
    if (controller.Failed()) std::cout << controller.ErrorText() << std::endl;
    //rpc调用成功,再看业务执行情况
    else {
        std::cout << register_response.success() << std::endl;
        std::cout << register_response.msg() << std::endl;
    }
}