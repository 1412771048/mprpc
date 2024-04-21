#pragma once
#include <unordered_map>
#include <functional>
#include <iostream>
#include <shared_mutex>
#include <string>
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpConnection.h"
#include "SimpleIni.h"
#include "rpcheader.pb.h"

class RpcProvider {
public:    
    typedef enum LockMode {
        READ,
        WRITE
    } LockMode;
    static void* Lock(const std::string& data, const LockMode& mode);
    static void Unlock(const LockMode& mode);

    //初始化，把配置文件写入DataBank
    static void Init(int argc, char** argv); 
    //发布服务
    void NotifyService(google::protobuf::Service* service); //形参用抽象基类接受任意的service子类
    //循环等待rpc请求
    void Run();

private:
    static std::shared_mutex rw_mtx; //一把读写锁控制这里的所有数据，这里明显读多写少,就用读写锁吧

    static std::unordered_map<std::string, std::string> config_map; //全类共享的配置，设为静态
    static bool config_ok; //config_map只被初始化一次

    static std::unordered_map<std::string, google::protobuf::Service*> service_map; //供大家查服务方法是否存在，也设为静态

    
    // typedef struct ServiceInfo {
    //     google::protobuf::Service* service;
    //     std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> methodMap;//方法名-方法具体描述
    // } ServiceInfo;
    // std::unordered_map<std::string, ServiceInfo> m_serviceMap; // 服务名-服务的具体信息


    muduo::net::EventLoop event_loop;
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time);

    //CallMethod需要传入一个google::protobuf::Closure *done，我们需要自己写一个函数，为了得到done，做什么呢：序列化响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response);
};