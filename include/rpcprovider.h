#pragma once
#include <unordered_map>
#include <functional>
#include <string>
#include "google/protobuf/service.h"
#include "google/protobuf/descriptor.h"
#include "muduo/net/TcpServer.h"
#include "muduo/net/EventLoop.h"
#include "muduo/net/InetAddress.h"
#include "muduo/net/TcpConnection.h"
#include "SimpleIni.h"
#include <iostream>
#include "databank.h"


class RpcProvider {
public:
    void LoadConfig(); //初始化，把配置文件写入config_map

    //发布服务
    void NotifyService(google::protobuf::Service* service); //形参用抽象基类接受任意的service子类

    //循环等待rpc请求
    void Run();

private:
    typedef struct ServiceInfo {
        google::protobuf::Service* service;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> methodMap;//方法名-方法具体描述
    } ServiceInfo;

    std::unordered_map<std::string, ServiceInfo> m_serviceMap; // 服务名-服务的具体信息


    muduo::net::EventLoop m_eventLoop;
    void OnConnection(const muduo::net::TcpConnectionPtr& conn);
    void OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time);

    //CallMethod需要传入一个google::protobuf::Closure *done，我们需要自己写一个函数，为了得到done，做什么呢：序列化响应和网络发送
    void SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message*);
};