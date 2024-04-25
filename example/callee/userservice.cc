#include "rpcprovider.h"

#include "user.pb.h"


//服务端编程示例：把Login方法编程rpc方法
class UserService: public fixbug::UserServiceRpc {
public:
    bool Login(std::string name, std::string pwd) {
        std::cout << "这是本地服务: Login, name: " << name << ", pwd: " << pwd << std::endl; 
        return true;
    }
    void Login(::google::protobuf::RpcController* controller, 
                const ::fixbug::LoginRequest* request,
                ::fixbug::LoginResponse* response, 
                ::google::protobuf::Closure* done) 
    {
        //就三步
        //1. 通过request调用本地方法处理业务
        bool res = Login(request->name(), request->pwd());
        //2. 结果填入response
        response->set_sucess(res);
        auto ResultCode = response->mutable_result();
        ResultCode->set_errcode(0);
        ResultCode->set_errmsg("");
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
        //就2步：1.装填RegisterResponse的proto 2. done->Run();
        response->set_sucess(Register(request->id(), request->name(), request->pwd()));
        response->mutable_result()->set_errmsg("");
        response->mutable_result()->set_errcode(0);
        done->Run();
    }
};



int main(int argc, char** argv) {
    //把日志写入队列，再std::string msg = time.second + lock_queue.pop();加上时间头部，写入文件
    LOG_INFO("%s:%s:%d", __FILE__, __FUNCTION__, __LINE__);//哪个源文件的哪个函数的哪一行
    
    RpcProvider::Init(argc, argv); 
    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)RpcProvider::Lock("config_map", READ);
    std::cout << "rpc_server_ip: " << (*config_map_ptr)["rpc_server_ip"] << std::endl;
    std::cout << "rpc_server_port: " << (*config_map_ptr)["rpc_server_port"] << std::endl;
    std::cout << "zookeeper_server_ip: " << (*config_map_ptr)["zookeeper_server_ip"] << std::endl;
    std::cout << "zookeeper_server_port:" << (*config_map_ptr)["zookeeper_server_port"] << std::endl;
    RpcProvider::Unlock(READ);

    RpcProvider provider; 
    //发布一个服务，就是把服务填入map表里
    provider.NotifyService(new UserService); 
    // //provider.NotifyService(new ProductService); 可以把多个方法发布成rpc服务
    // //循环等待请求
    provider.Run();
}