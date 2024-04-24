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
