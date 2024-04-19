#include "databank.h"

std::shared_mutex DataBank::config_map_mutex; //读写锁
std::unordered_map<std::string, std::string> DataBank::config_map;

std::unordered_map<std::string, std::shared_mutex*> data_mutex_map = {
    {"config_map", &DataBank::config_map_mutex}
};

