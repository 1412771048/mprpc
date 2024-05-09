# mprpc
基于muduo库和protobuf，c++实现的一个rpc框架


* 一个大型工程我们一般把每个模块都编译成一个库(一般是动态库)，好处：1.各模块相对独立，便于维护；2. 可重用；3. 编译快
* 实际项目中用到第三方库的话都是编译到项目目录下，不会编译到系统目录下,本项目就以protobuf为例，muduo和boost也应该移出来
所以以后安装三方库都要注意，不管他是cmake编译的还是其他的，应该都能指定安装位置
* 包含源文件，库的源文件不需要包含
* 函数形参若是void*，则可接受任意类型的指针，但函数内部必须转换为实际类型才能使用
函数返回值若是void*,则可以返回任意类型的指针，但返回后必须转换才能使用
一个主要函数最好不要超过多少行，否则考虑拆分


本地服务如何变成rpc服务，即服务器端如何编程：继承服务类UserServiceRpc，重写虚函数方法即可。
见userservice.cc: 写好后调用框架：1.加载配置文件，2.发布服务 3. 启动网络
1. 加载配置文件我们就用一个方法把配置写到一个静态的map表里，后续直接查表拿配置也方便
用一个databank存放全局数据供大家读写；用一个读写锁控制整个databank
2. 发布服务就是把服务放到一个服务表里，后续请求来了查它
3. 启动网络就是muduo库编程，基本死的,主要是OnMessage如何实现.

客户端如何发起rpc请求：
1. 加载配置文件
2. 生成stub对象，装填proto，调用

生成静态库，所有的文件都不用给别人了，包括第三方库

要发布一个方法，先要在proto里定义(user.proto)

现在有一个问题，当前的rpc调用状态？若调用失败，像客户端你还搁那读取errcode,msg没有意义
我们需要知道rpc调用过程中的状态，通过controller实现

我们把头文件都放到rpcprovider.h里，客户端只要包含这个就行了

日志模块：写日志是磁盘io很慢，一般是用一个消息队列，写日志知识写到队列里，后续专门用一个线程异步写入磁盘
这样就不会把磁盘io的时间放在rpc调用里
加一把锁保证线程安全，且队列无数据时，写线程(消费者)就不要去抢锁了，让生产者去拿锁：信号量/条件变量
这个日志模块以后可以移植到其他项目里

zookeeper：分布式协调服务，可以充当服务注册中心，提供分布式锁(不同主机上的进程如何安全的访问分布式服务)
功能很多，我们这里就用的是服务注册中心这个功能
(项目表达很重要，你为什么用这个？你知道什么就聊什么，我这个项目里需要xxx，所以我用到了它的xx功能，虽然他还有其他的功能，但我没用到，不太了解，我只知道xxx)

下载：好多新版本没有configure了...
https://archive.apache.org/dist/zookeeper/zookeeper-3.4.9/zookeeper-3.4.9.tar.gz
cd src/c
./configure
打开makefile 548行，AM_CFLAGS = -Wall -Werror  去掉-Werror
make -j4
make install
至此就获取c api编程接口，但原生api有缺点：1. 设置监听watch是一次性的，触发后要重新设置；
2. znode节点只存储字节数组，若要存储对象，需手动转换成字节数组(protobuf)
他会定期发送心跳


打开conf，改名为zoo.cfg，dataDir=/home/gyl/workspace/mprpc/output/zk_data
cd bin, ./zkServer.sh start
./zkCli.sh 客户端连接；
zk的数据组织结构是树状
ls / 列出当前目录节点; get /zookeeper获取节点数据; 
set /zookeeper 10 设置节点数据; 
create /test 创建节点; delete /test 删除节点; deleteall /test 删除节点及子节点;
我们主要关注2个数据：第一行设置的数据，以及ephmeralOwner = 0x0 永久节点/临时节点
每个rpc服务提供者都对应一个节点，要定时发送ping心跳，若超时，临时节点就被删掉了
我们采用/UserServiceRpc为一个节点名，子节点是它的方法如/Login,里面的数据就是ip port


zk有一个watch机制，类似发布订阅，事件触发，通知客户端

如何移植到项目里：服务端在run的时候，注册到zk即可，注意要开启zkserver

客户端数据发过去都能正常显示，但返回的response为什么没有填入数据呢，都是默认值.


函数不要随便exit,可以给返回值，让调用方自己选择


启动provider，若没挂就已经注册到zk里了

create table user(
    name varchar(20) not null,
    password varchar(20) not null,
    state enum('offline', 'online') default 'offline' not null
);

create table offlinemsg(
    name varchar(20) not null,
    msg varchar(200)
);


response为空，客户端就收不到send,所以proto信息要带多一点