#include <unordered_map>
#include <string>
#include <shared_mutex>
#include "google/protobuf/service.h"

//这个类专门存放公共的数据供大家读写
class DataBank {
public:
    static std::unordered_map<std::string, std::string> config_map;

    static std::unordered_map<std::string, google::protobuf::Service*> service_map;
private:
};


