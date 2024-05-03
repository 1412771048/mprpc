#include "rpcprovider.h"

#include "user.pb.h"

//服务端(rpc提供方)编程示例：先把本地方法变成rpc方法
class UserService: public fixbug::UserServiceRpc {
public:
    bool Login(std::string name, std::string pwd) {
        std::cout << "这是本地服务: Login, name: " << name << ", pwd: " << pwd << std::endl; 
        return false;
    }
    void Login(::google::protobuf::RpcController* controller, 
                const ::fixbug::LoginRequest* request,
                ::fixbug::LoginResponse* response, 
                ::google::protobuf::Closure* done) 
    {
        //就三步
        //1. 通过request调用本地方法处理业务
        bool res = Login(request->name(), request->pwd());
        //2. 结果填入response，业务执行成功与否可以根据res看，失败才写msg
        response->set_success(res);
        if (!res) response->set_errmsg("login failed!,用户名或密码错误！");
        //3. 其实就是执行OnMessage那边传入的方法：SendRpcResponse
        done->Run();
    }


    bool Register(uint32_t id, std::string name, std::string pwd) {
        std::cout << "这是本地服务: Register,id: " << id << ", name: " << name << ", pwd: " << pwd << std::endl; 
        return true;
    }
    void Register(::google::protobuf::RpcController* controller, 
                const ::fixbug::RegisterRequest* request,
                ::fixbug::RegisterResponse* response, 
                ::google::protobuf::Closure* done) 
    {
        bool res = Register(request->id(), request->name(), request->pwd());
        response->set_success(res);
        response->set_errmsg("");
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
    provider.NotifyService(new UserService); 
    // provider.NotifyService(new ProductService); //可以把多个方法发布成rpc服务
    
    //3. 启动服务器，开始提供服务
    provider.Run();
}
