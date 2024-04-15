#pragma once
#include "mprpcconfig.h"

//mprpc框架的基础类，单例
class MprpcApplication {
public:
    static MprpcApplication& GetInstance();
    static void Init(int argc, char** argv);
    static MprpcConfig& GetConfig();
private:
    static MprpcConfig config_; 

    MprpcApplication() = default;
};
