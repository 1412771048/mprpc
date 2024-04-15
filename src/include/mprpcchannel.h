#pragma once
#include <google/protobuf/service.h>

//RpcChannel是纯虚类，我们重写CallMethod方法
class MprpcChannel: public RpcChannel {
public:
    //所有stub对象调用的rpc方法都走这个，我们再这里统一做数据序列化和网络发送
    void CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, 
                    const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done);
};

