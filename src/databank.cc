#include "databank.h"

std::shared_mutex DataBank::rw_mutex; //读写锁
std::unordered_map<std::string, std::string> DataBank::config_map;