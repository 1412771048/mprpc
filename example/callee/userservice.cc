#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.h"
#include "rpcprovider.h"

//callee是rpc服务提供者，那他如何提供的呢，即如何把本地的方法发布成可供远程调用的rpc方法?
//先要定义一个proto，生成2个服务类，UserServiceRpc类是给我们使用的，重写虚函数，写好后给框架调用
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
        //这里面就三步
        //1. 通过request调用本地方法处理业务
        std::string name = request->name(), pwd = request->pwd(); //LoginRequest*他是这个类啊
        bool res = Login(name, pwd);
        //2. 结果填入response
        response->set_sucess(res);
        auto ResultCode = response->mutable_result();
        ResultCode->set_errcode(0);
        ResultCode->set_errmsg("");
        //3. 执行回调：done
        done->Run();//这里也需要重写，把response序列化，通过网络发出去
    }
};

//callee的业务已经写好了，现在我需要使用框架
int main(int argc, char** argv) {
    //框架的使用套路(类似muduo啊)
    //1. 框架初始化
    MprpcApplication::Init(argc, argv);
    //2. 把UserService对象发布到rpc节点上
    RpcProvider provider; //provider是一个网络服务对象
    provider.NotifyService(new UserService); //把服务写到map表里，后续远端服务来了就查map表即可。
    //provider.NotifyService(new ProductService); 可以把多个方法发布成rpc服务
    //3. 启动一个rpc服务发布节点
    provider.Run();//run以后，进程阻塞等待远程调用请求，这里就是muduo网络模块
}
