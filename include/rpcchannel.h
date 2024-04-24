#pragma once
#include <arpa/inet.h> 
#include "rpcprovider.h"


typedef struct socket_send_res {
    char recv_buf[1024];
    int recv_size;
    socket_send_res(): recv_size(-1) {memset(recv_buf, 0, sizeof(recv_buf));}
} socket_send_res;


//RpcChannel是纯虚类，我们重写CallMethod方法
class MpRpcChannel: public google::protobuf::RpcChannel {
public:
    //所有stub对象调用的rpc方法都走这个，我们再这里统一做数据序列化和网络发送
    void CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, 
                    const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done);
private:
    //序列化数据，返回序列化后的字符串
    std::string GetSendstr(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, const google::protobuf::Message* request);

    //调用socket发送数据，返回recv_size，结果写入recv_buf
    socket_send_res SocketSend(const std::string& ip, const std::uint16_t port, const std::string& send_str, google::protobuf::RpcController* controller);
};

