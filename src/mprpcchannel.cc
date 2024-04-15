#include "include/mprpcchannel.h"
#include <string>
#include <rpcheader.pb.h>

// header_size + service_name + method_name + args_size + args
void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, 
                    const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done)
{   
    std::string service_name = method->service()->name(), method_name = method->name();
    //获取参数的序列化字符串长度
    std::string args_str = request->SerializeAsString();
    uint32_t args_size = args_str.size();
    mprpc::RpcHeader* rpc_header;
    rpc_header->set_service_name(service_name);
    rpc_header->set_method_name(method_name);
    rpc_header->set_args_size(args_size);

    std::string rpc_header_str = rpc_header->SerializeAsString();
}