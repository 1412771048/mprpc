#pragma once

typedef enum LockMode {
    READ,
    WRITE
} LockMode;

typedef enum LogLevel {
    INFO,
    ERROR
} LogLevel;


#define LOG_INFO(msg, ...)\
    do {\
        Loger& loger = Loger::GetInstance();\
        loger.SetLogLevel(INFO);\
        char buf[1024] = {0};\
        snprintf(buf, sizeof(buf), (std::string("[INFO] ") + msg + "\n").c_str(), ##__VA_ARGS__);\
        loger.Log(buf);\
    } while (0);

#define LOG_ERROR(msg, ...)\
    do {\
        Loger& loger = Loger::GetInstance();\
        loger.SetLogLevel(ERROR);\
        char buf[1024] = {0};\
        snprintf(buf, sizeof(buf), (std::string("[ERROR] ") + msg + "\n").c_str(), ##__VA_ARGS__);\
        loger.Log(buf);\
    } while (0);