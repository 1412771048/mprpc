#pragma once
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <utility>
#include "lockqueue.h"
#include "typedef.h"


//mprpc框架的日志模块
class Loger {
public:
    static Loger& GetInstance();
    void SetLogLevel(const LogLevel level);
    void Log(const std::string& msg); //写日志到缓冲队列
private:
    int log_level; //记录日志级别
    LockQueue<std::string> lock_queue; //日志缓冲队列

    Loger();

    std::pair<std::string, std::string> GetTime(); 
};






