#include "rpccontroller.h"
MprpcControlleer::MprpcControlleer(): failed_(false), err_text_("") {}
void MprpcControlleer::Reset() {
    failed_ = false;
    err_text_ = "";
}
bool MprpcControlleer::Failed() const {
    return failed_;
}
std::string MprpcControlleer::ErrorText() const {
    return err_text_;
}
void MprpcControlleer::SetFailed(const std::string& reason) {
    failed_ = true;
    err_text_ = reason;
}

void MprpcControlleer::StartCancel() {}
bool MprpcControlleer::IsCanceled() const {return false;}
void MprpcControlleer::NotifyOnCancel(google::protobuf::Closure* callback) {}