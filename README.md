>本项目是基于muduo库+Protobuf开发的一个高效的RPC框架，主要是为RPC方法的调用提供数据处理、网络发送以及服务注册及发现功能，以简化分布式系统中的远程过程调用。

# 准备工作
启动zookeeper和nginx,已下载到vendor，参考以下链接开启即可：
[nginx](https://blog.csdn.net/qq_72642900/article/details/138380177?spm=1001.2014.3001.5502)

[zookeeper](https://blog.csdn.net/qq_72642900/article/details/138380411?spm=1001.2014.3001.5502)

# 编译
```bash
./autobuild.sh
```

# 运行
我们在nginx里注册了三个节点，这里就开那三个
```bash
cd output/
./service -i server.conf -r rpc1
./service -i server.conf -r rpc2
./service -i server.conf -r rpc3
./client -i client.conf
```

什么是分布式？
    分布式是一种系统架构，多个节点协作完成任务
    单机服务器的缺点：1.由于硬件，并发有限；2. 修改任何一个模块，整个工程要重新编译部署；3. 资源分配不合理
    (不同的模块对资源的需求是不一样的，如业务模块肯定需要更多的硬件资源，而像一些后台管理模块根据不需要高并发)
    集群呢：集群能提高并发，后面2点也解决不了
    故引入分布式：把一个工程分成多个模块，根据每个模块对资源的需求部署在合适的机器上，所有服务器共同提供服务。每个服务器就是一个节点
    好处：修改任何一个模块不影响其他模块；资源合理分配：可根据节点的并发需求再做集群部署
    
什么是RPC？
    RPC是一个技术：远程方法调用，它可以让一台服务器通过网络调用另一台服务器上的方法。
    它屏蔽了网络调用的细节，让调用远程方法像调用本地一样简单。让我们只需要关注业务逻辑。类似socket，屏蔽了底层的各层协议栈细节。
    而且RPC是跨语言的，只要双方协议一致即可。本项目就用的是protobuf
    比较火的有grpc,brpc,而我这个项目的就是也实现一个rpc框架，叫mprpc

RPC的工作原理？
    1.客户端调用本地代理(其实就是一个框架)去访问一个RPC方法
    2. 代理把请求封装好，发送给服务端
    3. 服务端解析请求，调用本地方法，把响应封装好，发回给客户端代理
    4. 客户端代理解析响应，返回给调用者

为什么选择用protobuf?
    常用的通信协议有json、xml、protobuf，各有优点，json简单易用，而protobuf的优点在于它把数据序列成2进制传输，数据量更小，传输也就更快。

protobuf是如何定义RPC方法？
    通过它的service模块
    假设现在服务端有一个login方法, 客户端想调用它，前提是login方法它得先变成rpc方法，怎么发布成rpc方法？靠rpotobuf
    在proto里的service模块里定义好rpc方法的请求和响应(具体就是把形参给定义好)，生成代码，就会生成2个类，假设我定义了一个服务类RpcService，里面有一个login()方法，请求和响应的形参都定义好了，则它生成代码后的2个类:RpcService和RpcService_Stub。
    客户端使用pcService_Stub类对象填充request的proto, 调RPC方法，等待响应即可。
    服务端重写RpcService类的query方法：1.调本地的query方法，传入客户端request,把返回值写到响应里发回去 


运行流程(架构)：简历里就有

对Zookeeper的了解？
    1. Zookeeper是一个分布式协调组件，主要功能有配置管理、分布式锁、服务注册、领导选举，我这里就让他充当一个服务注册中心
    2. 他内部的数据组织结构就与linux的目录一样都是树状，像我使用的时候一般/server_name/method_name/ip+port
    3. zk的客户端会向服务器定时发送心跳，若超时，服务器就认为他挂了，如果是临时节点就删掉。我们服务端节点注册到zk都设置为临时节点。
    4. zk还有一个watch机制，他可以监听节点数据的变化，并通知客户端，类似发布订阅

对muduo库的了解？
        muduo库是一个高性能的网络库，典型的多Reactor多线程模式
    什么是reactor和proactor:其实就是对I/O多路复用做了封装，让他面向对象。
    Ractor模型的流程：reactor对象通过内部的epoll监听事件，收到事件后通过分发器分发给accetor对象或者handler对象分别对应建立连接事件和处理已连接用户的具体事件。这是单reacor单线程。常用的是多reactor多线程：主线程中主reactor负责用户连接，然后把连接发给子线程，子线程中的子reactor负责具体事件的处理，而且正好映射了那句话：one loop pthread,一个事件循环对应一个线程(一个reactor即一个事件循环对应一个线程)。
        muduo的源码我也看过一点，大致有4大组件：event事件、Reactor反应堆、多路录用事件分发器，事件处理器
    当一个连接/读写事件来了，先到event事件这里，通过它注册到Reactor反应堆，反应堆就是一个事件集合，然后反应堆再像事件分发器
    注册事件即epoll add/mod, 启动反应堆会连带开启事件分布器的事件循环epoll_wait,事件发生返回事件集合给反应堆，反应堆调对应的事件处理器处理。
        提供简单的api实现对连接事件和读写事件设置回调。
    简单总结：muduo库就是非阻塞io+io多路复用+线程池

lockQueue，无所队列有了解吗？
        lockqueue就是把std::queue加了一把锁，使其充当一个线程安全的日志缓冲队列，本项目是一写多读，开一个写线程在后台pop，
    这样就磁盘IO时间就不再RPC调用里面。
        pop细节：进来加锁，判断队列是否为空则wait(lock)释放锁并等待。
        无锁队列有一点了解，是要用原子操作去保证线程安全，但我没有仔细研究过，比较复杂。

连接池设计？
        先说一下连接池的原理：大部分池化技术远离都是预分配资源+重复利用，避免资源的反复申请释放，浪费时间
    而数据库连接池也是基于此。我们知道一条sql从客户端到服务器的执行流程：tcp三次握手->mysql连接认证->执行sql->关闭mysql连接->tcp4次挥手
    而连接池直接把这条连接给缓存下来，后续直接用，5步变1步。
        连接池的核心参数：初始连接数、最大连接数、最大空闲时间(一般超过此时间就把空闲连接数恢复到初始连接数，避免浪费)、连接超时时间
        运行流程：创建单例、创建初始连接数的连接放入一个队列，并且开一个线程定时检查队列里的连接有无超过最大空闲时间(看队头即可)，超过就开始清理。

对Ningx的了解?
        Ningx是一个高性能的HTTP服务器和反向代理服务器(所谓反向代理就是客户端向代理服务器发请求，由他转发给服务器，正向代理就是客户端通过
    代理服务器去访问目标服务器，如翻墙梯子)。本项目就让他作为反向代理服务器，客户端的RPC请求本来是由框架直接发给对应的节点，现在改为发给nginx,nginx再根据它的负载均衡策略分发给各个服务器节点。
        负载均衡：把请求根据一定的策略发到各个服务器节点上，避免单个服务器过载
        策略：轮询(等权值)、按权值分配(1，2)、随机、一致性哈希
    一致性哈希：我们把0-2^32-1这些数在逻辑上形成一个环。把服务器映射到哈希环上，可以用ip地址对2^32取模。然后把请求也映射上去，这个请求顺时针走，发给第一个服务器。好处：增删节点方便，如：abc三个服务器，我要加一个d在ab之间，则只需要把a-d的数据从b转移到d上，其他的数据不动，开销比一般的哈希函数小很多。但是若节点分布不均，会导致负载不均。所以引入虚拟节点，如现在3个节点，一个节点对应5个虚拟节点，就有15个虚拟节点，然后把15个虚拟节点平均分布在环上，当请求到达虚拟节点，就放到实际节点这样，节点多了就相对均匀。也就相对负载均衡了。

RPC方法调用过程中若出现问题怎么办？
    有一个参数叫controller，从客户端发起的请求可以带上这个参数，过程中任何一步出问题，都可以用这个参数记录下来

-----------------------------------------------------------------------------------------------------------------------
代码层面细节分析：

在RPC调用过程中,我们定义了一个rpc_header的proto：severvice_name, method_name, arg_size(参数长度)，
当客户端发起连接后，框架先要组织好哪些数据需要序列化发送，rpc_header + args_str， 头部字符流+参数字符流着2个肯定要发
但若遇到粘包问题，就麻烦了，所以我们还需要把头部字符串的长度发过去。你不能直接to_string,那对方怎么知道要读几位啊
str.insert(0, std::string((cahr*)&header_size, 4));
把整数地址转成char*,访问4位，就把整数变成了4字节的字符串，int都只占4个字节

----------------------------------------------------------------------------------------------------------------------
项目背景（为什么做这个项目）？
    首先这是一个开源项目。为了学习分布式、RPC相关知识

项目难点：
    1. RPC框架的架构的理解以及工作原理的把握
    2. 各种组件的学习也是对我学习能力的训练。

为什么要用这些组件？


