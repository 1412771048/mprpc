#include "rpcprovider.h"

#include "user.pb.h"

//服务端(rpc提供方)编程示例：先把本地方法变成rpc方法
class ChatService: public fixbug::ChatServiceRpc {
public:
    const std::string Chat(const std::string& question) {
        std::cout << "这是本地服务: Chat, question: " << question << std::endl; 
        return "我不知道答案";
    }
    void Chat(::google::protobuf::RpcController* controller, 
                const ::fixbug::ChatRequest* request,
                ::fixbug::ChatResponse* response, 
                ::google::protobuf::Closure* done) 
    {
        //就三步
        //1. 通过request调用本地方法处理业务
        const std::string res = Chat(request->question());
        //2. 结果填入response，业务执行成功与否可以根据res看，失败才写msg
        response->set_answer(res);
        //3. 其实就是执行OnMessage那边传入的方法：SendRpcResponse
        done->Run();
    }
};


int main(int argc, char** argv) {
    //1. 初始化框架：把配置文件写入configMap_
    auto& config = mprpc::MpRpcConfig::GetInstance(); //获取单例的引用
    config.Init(argc, argv); 
    std::cout << "zookeeper_ip: " << config.QuerryConfig("zookeeper_ip") << std::endl;
    std::cout << "zookeeper_port: " << config.QuerryConfig("zookeeper_port") << std::endl;
    std::cout << "rpc_ip: " << config.QuerryConfig("rpc_ip") << std::endl;
    std::cout << "rpc_port: " << config.QuerryConfig("rpc_port") << std::endl;
    std::cout << "nginx_ip: " << config.QuerryConfig("nginx_ip") << std::endl;
    std::cout << "nginx_port: " << config.QuerryConfig("nginx_port") << std::endl;
    //2. 发布服务: 把本地服务写入serviceMap_
    mprpc::RpcProvider provider;
    provider.NotifyService(new ChatService); 
    // provider.NotifyService(new ProductService); //可以把多个方法发布成rpc服务
    
    //3. 启动服务器，开始提供服务
    provider.Run();
}
