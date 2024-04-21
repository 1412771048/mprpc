#include "rpcchannel.h"


//客户端的所有rpc请求都走这个，需要自己定义：我们在这里做数据序列化(填装proto)和网络发送
void MpRpcChannel::CallMethod(const google::protobuf::MethodDescriptor* method, google::protobuf::RpcController* controller, 
                    const google::protobuf::Message* request, google::protobuf::Message* response, google::protobuf::Closure* done)
{   
    std::string service_name = method->service()->name();
    std::string method_name = method->name();
    std::string args_str = request->SerializeAsString();
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
    send_str += rpc_header_str + args_str;

    //网络发送，就使用简单的socket编程吧，不需要什么高性能
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1) {
        std::cerr << "socketfd create error!" << std::endl;
        exit(EXIT_FAILURE);
    } 
    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)RpcProvider::Lock("config_map", RpcProvider::READ);
    std::string ip = (*config_map_ptr)["rpc_server_ip"];
    uint16_t port = stoi((*config_map_ptr)["rpc_server_port"]);
    RpcProvider::Unlock(RpcProvider::READ);

    sockaddr_in server_addr = {AF_INET, htons(port), inet_addr(ip.c_str())};
    if (connect(fd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        std::cerr << "socket connect error!" << std::endl;
        close(fd);
        exit(EXIT_FAILURE);
    }
    if (send(fd, send_str.c_str(), send_str.size(), 0) == -1) {
        close(fd);
        std::cerr << "socket send error!" << std::endl;
        return; //不要因为一个send而exit：结束这一次的rpc调用
    }
    char recv_buf[1024] = {0};
    int recv_size = recv(fd, recv_buf, sizeof(recv_buf), 0);
    if (recv_size == -1) {
        std::cerr << "socket recv error!" << std::endl;
        close(fd);
        return;
    }
    close(fd);

    //拿到recv_buf，反序列化填到response
    //std::string response_str(recv_buf); 出现问题：recv_buf有\0后面就截断了，如recv_buf = {'a', '\0', 'b'}
    if (!response->ParseFromArray(recv_buf, recv_size)) {
        std::cerr << "response parse error!" << std::endl;
        return;
    } 
    //至此rpc调用就结束了，response自动返回
}