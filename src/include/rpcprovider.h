#pragma once
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <unordered_map>
#include <functional>
#include <string>
#include "mprpcapplication.h"


// 框架提供的专门发布rpc服务的网络对象类
class RpcProvider {
public:
    // 这里是框架提供给外部使用的，可以发布rpc方法的函数接口
    void NotifyService(google::protobuf::Service* service); //形参用抽象基类接受任意的service子类

    // 启动rpc服务节点，开始提供rpc远程网络调用服务
    void Run();

private:
    typedef struct ServiceInfo {
        google::protobuf::Service* service;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> methodMap;//方法名-方法具体描述
    }ServiceInfo;


    std::unordered_map<std::string, ServiceInfo> m_serviceMap; // 服务名-服务的具体信息

    muduo::net::EventLoop m_eventLoop;
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time);

    //CallMethod需要传入一个google::protobuf::Closure *done，我们需要自己写一个函数，为了得到done，做什么呢：序列化响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message*);
};