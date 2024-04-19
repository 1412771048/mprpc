#include "rpcprovider.h"
void RpcProvider::LoadConfig() {
    CSimpleIniA ini;
    SI_Error rc = ini.LoadFile("../../config/test.ini");
    if (rc < 0) {
        std::cerr << "Failed to open config file!" << std::endl;
        exit(EXIT_FAILURE);
    }
    std::string rpc_server_ip = ini.GetValue("rpc", "server_ip", "");
    std::string rpc_server_port = ini.GetValue("rpc", "server_port", "");
    std::string zookeeper_server_ip = ini.GetValue("zookeeper", "server_ip", "");
    std::string zookeeper_server_port = ini.GetValue("zookeeper", "server_port", "");
    if (rpc_server_ip.empty() || rpc_server_port.empty() || zookeeper_server_ip.empty() || zookeeper_server_port.empty()) {
        std::cerr << "config file format errno!" << std::endl;
        exit(EXIT_FAILURE);
    }

    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)DataBank::Lock("config_map", DataBank::WRITE);
    config_map_ptr->insert({"rpc_server_ip", rpc_server_ip});
    config_map_ptr->insert({"rpc_server_port", rpc_server_port});
    config_map_ptr->insert({"zookeeper_server_ip", zookeeper_server_ip});
    config_map_ptr->insert({"zookeeper_server_port", zookeeper_server_port});
    DataBank::Unlock(DataBank::WRITE);
}

void RpcProvider::NotifyService(google::protobuf::Service *service) {
    //获取服务的具体描述：里面有服务名，方法数，每个方法名，都通过这个拿
    auto pserverdesc = service->GetDescriptor(); 
    std::string service_name = pserverdesc->name(); //获取服务名,即类名
    int method_count = pserverdesc->method_count(); //获取方法数量，因为一个服务对象可以有多个方法

    ServiceInfo service_info; 
    service_info.service = service; 
    for (int i = 0; i < method_count; i++) {
        auto pmethoddesc = pserverdesc->method(i); //获取一个方法描述
        std::string method_name = pmethoddesc->name(); //获取方法名，即函数名
        service_info.methodMap[method_name] = pmethoddesc;
    }   
    m_serviceMap.insert({service_name, service_info}); //插入服务信息到服务map中
    

    //服务名-服务
    auto service_map_ptr = (std::unordered_map<std::string, google::protobuf::Service*>*)DataBank::Lock("service_map", DataBank::WRITE);
    service_map_ptr->insert({service->GetDescriptor()->name(), service});
    DataBank::Unlock(DataBank::WRITE);
}
  
//启动网络服务，即muduo库编程
void RpcProvider::Run() {
    //先拿ip, port
    auto config_map_ptr = (std::unordered_map<std::string, std::string>*)DataBank::Lock("config_map", DataBank::READ);
    const std::string ip = (*config_map_ptr)["rpc_server_ip"];
    const uint16_t port = stoi((*config_map_ptr)["rpc_server_port"]);
    DataBank::Unlock(DataBank::READ);

    //muduo库编程，基本死的
    muduo::net::TcpServer server(&event_loop, muduo::net::InetAddress(ip, port), "RpcProvider");
    // 绑定连接回调和消息读写回调，分离网络代码和业务代码
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
    //设置线程数
    server.setThreadNum(4);//一个io线程，三个工作线程
    //启动网络服务,后续有连接请求时，会调用connectioncallback，有消息读写请求时，会调用messagecallback
    server.start();
    event_loop.loop();
}

//对于新连接我们要干嘛
void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr& conn) {
    if (!conn->connected()) {//断开连接了
        conn->shutdown(); //关闭文件描述符，对应socket的close
    }
}

//对于读写事件我们要干嘛，不同的场景不一样，这里就是反序列化，执行函数
void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr& conn, muduo::net::Buffer* buf, muduo::Timestamp time) {
    // std::string msg = buf->retrieveAllAsString(); 
    // //把msg反序列化得到：service_name、method_name、args_size,
    // //反序列化的结果类似： 16UserServiceLoginzhang san123456    这个16把他变成2进制存，就不会超过4个字节

    // //从msg中读前4个字节，反序列化它
    // uint32_t header_size = 0;
    // msg.copy((char*)&header_size, 4, 0);//msg从0位置拷贝4字节内容到header_size
    // std::string rpc_header_str = msg.substr(4, header_size);  //对应这段："UserServiceLogin"
    // mprpc::RpcHeader rpc_header;
    // if (!rpc_header.ParseFromString(rpc_header_str)) {//反序列化为"UserServiceLogin"
    //     std::cerr << rpc_header_str << " error!" << std::endl;
    //     return;
    // }
    // //反序列化成功, 得到service_name、method_name、args
    // std::string service_name = rpc_header.service_name();
    // std::string method_name = rpc_header.method_name();
    // uint32_t args_size = rpc_header.args_size();
    // std::string args = msg.substr(4 + header_size, args_size); //对应："zhang san123456" 

    // //查表判断服务和方法存不存在
    // auto it = m_serviceMap.find(service_name);
    // if (it == m_serviceMap.end()) {
    //     std::cerr << service_name << "not exist!" << std::endl;
    //     return;
    // }
    // auto mit = it->second.methodMap.find(method_name);
    // if (mit == it->second.methodMap.end()) {
    //     std::cerr << service_name << ": " << method_name << "not exist!" << std::endl;
    //     return;
    // }
    
    // //存在，获取服务和方法
    // auto service = it->second.service;
    // auto method = mit->second;
    // //生成request和response，以便下面的rpc方法回调
    // auto request = service->GetRequestPrototype(method).New();
    // if (!request->ParseFromString(args)) {//反序列化为"zhang san123456" 
    //     std::cerr << "request parse error: " << args << std::endl;
    //     return;
    // }
    // auto response = service->GetResponsePrototype(method).New();

    // //生成done,给CallMethod提供最后一个参数
    // //NewNewCallback有很多重载版本，这里传入对象，方法，形参，这里显式指定了类型：
    // //     inline Closure* NewCallback(Class* object, void (Class::*method)(Arg1, Arg2),
    // //                             Arg1 arg1, Arg2 arg2) {
    // //   return new internal::MethodClosure2<Class, Arg1, Arg2>(
    // //     object, method, true, arg1, arg2);
    // // }
    // auto done = google::protobuf::NewCallback<RpcProvider, const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>
    //                                             (this, &RpcProvider::SendRpcResponse, conn, response);


    // //执行回调，这是基类的方法，才是框架吗
    // service->CallMethod(method, nullptr, request, response, done);
}   

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr& conn, google::protobuf::Message* response) {
    // //把序列化后的响应发送出去
    // conn->send(response->SerializeAsString());
    // //发送完断开，模拟http短连接，给更多的客户端提供请求
    // conn->shutdown(); 
}

