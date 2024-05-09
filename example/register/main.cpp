#include "rpcprovider.h"

#include "user.pb.h"
#include "connect_pool.hpp"

//服务端(rpc提供方)编程示例：先把本地方法变成rpc方法
class RegisterRpc: public fixbug::RegisterService {
public:
    bool Register(const std::string& name, const std::string& pwd) {
        auto conn_ptr = ConnectPool::GetInstance().GetConnection();
        if (!conn_ptr) return false;
        bool result = false;
        char sql[1024] = {0};
        snprintf(sql, sizeof(sql), "select name from user where name = '%s'", name.c_str());
        auto res = conn_ptr->query(sql);
        if (!mysql_fetch_row(res)) {
            sprintf(sql, "insert into user(name, password, state) values('%s', '%s', 'offline');", name.c_str(), pwd.c_str());
            if (conn_ptr->update(sql)) result = true;
        }
        return result;
    }
    void Register(::google::protobuf::RpcController* controller,
                       const ::fixbug::RegisterRequest* request,
                       ::fixbug::RegisterResponse* response,
                       ::google::protobuf::Closure* done)
    {
        //就三步
        //1. 通过request调用本地方法处理业务
        bool res = Register(request->name(), request->password());
        //2. 结果填入response，业务执行成功与否可以根据res看，失败才写msg
        response->set_success(res);
        if (res) response->set_msg("register sucess!");
        else response->set_msg("register failed!");
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
    provider.NotifyService(new RegisterRpc); 
    // provider.NotifyService(new ProductService); //可以把多个方法发布成rpc服务
    
    //3. 启动服务器，开始提供服务
    provider.Run();
}
