#pragma once

typedef enum LockMode {
    READ,
    WRITE
} LockMode;

typedef enum LogLevel {
    INFO,
    ERROR
} LogLevel;

#define LOG(level, msg, ...)\
    do {\
        Loger& loger = Loger::GetInstance();\
        loger.SetLogLevel(level);\
        char buf[1024] = {0};\
        snprintf(buf, sizeof(buf), (std::string("[") + #level + "] " + msg + "\n").c_str(), ##__VA_ARGS__);\
        loger.Log(buf);\
    } while (0);
#define LOG_INFO(msg, ...) LOG(INFO, msg, ##__VA_ARGS__)
#define LOG_ERROR(msg, ...) LOG(ERROR, msg, ##__VA_ARGS__)

