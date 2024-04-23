#pragma once
#include <string>
#include "google/protobuf/service.h"

//RpcController是抽象类，我们重写它的常用四个方法
class MprpcControlleer: public google::protobuf::RpcController {
public:
    MprpcControlleer();
    void Reset();
    bool Failed() const;
    std::string ErrorText() const;
    void SetFailed(const std::string& reason);
    //下面3个方法暂未实现
    void StartCancel();
    bool IsCanceled() const;
    void NotifyOnCancel(google::protobuf::Closure* callback);
private:    
    bool failed_; //rpc执行过程中的状态
    std::string err_text_; //执行过程中的错误信息
};