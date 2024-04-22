#pragma once
#include <arpa/inet.h> 
#include "google/protobuf/service.h"
#include "rpcprovider.h"
#include "rpcheader.pb.h"

//RpcChannel是纯虚类，我们重写CallMethod方法
class MpRpcChannel: public google::protobuf::RpcChannel {
public:
    //所有stub对象调用的rpc方法都走这个，我们再这里统一做数据序列化和网络发送
    void CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, 
                    const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done);
};
