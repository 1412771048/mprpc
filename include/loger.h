#pragma once
#include <string>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <utility>

#include "lockqueue.h"

//mprpc框架的日志模块
class Loger {
public:
    typedef enum LogLevel {
        INFO,
        ERROR
    } LogLevel;
    
    static Loger& GetInstance();
    void SetLogLevel(const LogLevel level);
    void Log(const std::string& msg); //写日志到缓冲队列


private:
    int log_level; //记录日志级别
    LockQueue<std::string> lock_queue; //日志缓冲队列

    Loger();
    std::pair<std::string, std::string> GetTime(); 
};

//snprintf(buf, sizeof(buf), msg, ##__VA_ARGS__用于格式化字符串，把msg和##__VA_ARGS__可变参合体

//这个宏是抽象宏
#define LOG(level, msg, ...) \
    do { \
        Loger loger = Loger::GetInstance(); \
        loger.SetLogLevel(level); \
        char buf[1024] = {0}; \
        snprintf(buf, sizeof(buf), msg, ##__VA_ARGS__); \
        loger.Log(buf); \
    } while (0);

#define LOG_INFO(msg, ...) LOG(INFO, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) LOG(ERROR, msg, ##__VA_ARGS__)



