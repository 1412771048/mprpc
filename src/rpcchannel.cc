#include "rpcchannel.h"


//客户端的所有rpc请求都走这个，需要自己定义：我们在这里做数据序列化(填装proto)和网络发送
void MpRpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, 
                    const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done)
{   
    //序列化
    std::string send_str = GetSendstr(method, controller, request);
    if (controller->Failed()) {
        return;
    }

    //网络发送并接受响应，就使用简单的socket编程吧，不需要什么高性能
    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)RpcProvider::Lock("config_map", RpcProvider::READ);
    std::string ip = (*config_map_ptr)["rpc_server_ip"];
    uint16_t port = stoi((*config_map_ptr)["rpc_server_port"]);
    RpcProvider::Unlock(RpcProvider::READ);

    socket_send_res res = SocketSend(ip, port, send_str, controller);
    if (controller->Failed()) {
        return;
    }

    //拿到recv_buf，反序列化填到response,std::string response_str(recv_buf); 
    //出现问题：recv_buf有\0后面就截断了，如recv_buf = {'a', '\0', 'b'}
    if (!response->ParseFromArray(res.recv_buf, res.recv_size)) {
        controller->SetFailed("response parse error!");
        return;
    } 
    //至此rpc调用就结束了，response被反序列化填入数据
}

std::string MpRpcChannel::GetSendstr(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, const google::protobuf::Message* request) {
    std::string service_name = method->service()->name();
    std::string method_name = method->name();
    std::string args_str = "";
    if (!request->SerializeToString(&args_str)) {
        controller->SetFailed("request serialize error!");
        return "";
    }
    
    uint32_t args_size = args_str.size();
    //装填proto
    mprpc::RpcHeader rpc_header;
    rpc_header.set_service_name(service_name);
    rpc_header.set_method_name(method_name);
    rpc_header.set_args_size(args_size);
    std::string rpc_header_str = rpc_header.SerializeAsString();
    uint32_t header_size = rpc_header_str.size();
    
    //组织要发送的字符流: xxxxheaderstrargs_str
    std::string send_str = "";
    send_str.insert(0, std::string((char*)&header_size, 4)); //前4个字节放ehader_size的2进制比表示
    return send_str + rpc_header_str + args_str;
}

socket_send_res MpRpcChannel::SocketSend(const std::string& ip, const std::uint16_t port, const std::string& send_str, google::protobuf::RpcController* controller) {
    socket_send_res res;
    int fd = socket(AF_INET, SOCK_STREAM, 0);
        if (fd == -1) {
        controller->SetFailed("socketfd create error!");
        return res;
    } 
    sockaddr_in server_addr = {AF_INET, htons(port), inet_addr(ip.c_str())};
    if (connect(fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        controller->SetFailed("socket connect error!");
        close(fd);
        return res;
    }
    if (send(fd, send_str.c_str(), send_str.size(), 0) == -1) {
        controller->SetFailed("socket send error!");
        close(fd);
        return res; 
    }
    if (res.recv_size = recv(fd, res.recv_buf, sizeof(res.recv_buf), 0) == -1) {
        controller->SetFailed("socket recv error!");
        close(fd);
        return res;
    }
    close(fd);
    return res;
}