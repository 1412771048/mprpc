# mprpc
基于muduo库和protobuf，c++实现的一个rpc框架


* 一个大型工程我们一般把每个模块都编译成一个库(一般是动态库)，好处：1.各模块相对独立，便于维护；2. 可重用；3. 编译快
* 实际项目中用到第三方库的话都是编译到项目目录下，不会编译到系统目录下,本项目就以protobuf为例，muduo和boost也应该移出来
所以以后安装三方库都要注意，不管他是cmake编译的还是其他的，应该都能指定安装位置
* 包含源文件，库的源文件不需要包含


本地服务如何变成rpc服务，即服务器端如何编程：继承服务类UserServiceRpc，重写虚函数方法即可。
见userservice.cc: 写好后调用框架：1.加载配置文件，2.发布服务 3. 循环等待请求
加载配置文件我们就用一个方法把配置写到一个静态的map表里，后续直接查表拿配置也方便
用一个databank存放全局数据供大家读写；这里就没加锁了
