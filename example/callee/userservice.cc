#include "user.pb.h"
#include "rpcprovider.h"



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
        //3. done->Run(),这个要重写，无非就是序列化+网络发送
        done->Run();
    }
};



int main(int argc, char** argv) {
    RpcProvider provider; 
    //加载配置文件
    provider.LoadConfig(); 
    
    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)DataBank::Lock("config_map", DataBank::READ);

    std::cout << "rpc_server_ip: " << (*config_map_ptr)["rpc_server_ip"] << std::endl;
    std::cout << "rpc_server_port: " << (*config_map_ptr)["rpc_server_port"] << std::endl;
    std::cout << "zookeeper_server_ip: " << (*config_map_ptr)["zookeeper_server_ip"] << std::endl;
    std::cout << "zookeeper_server_port:" << (*config_map_ptr)["zookeeper_server_port"] << std::endl;
    DataBank::Unlock("config_map", DataBank::READ);
    
    //发布一个服务，就是把服务填入map表里
    provider.NotifyService(new UserService); 
    
    // //provider.NotifyService(new ProductService); 可以把多个方法发布成rpc服务
    // //循环等待请求
    // provider.Run();
}
