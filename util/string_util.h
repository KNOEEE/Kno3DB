#pragma once

#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

// 非常实用
extern std::vector<std::string> StringSplit(
    const std::string& arg, char delim);

// errnoStr() function returns a string that describes the error code passed in
// the argument err
extern std::string errnoStr(int err);

extern const std::string kNullptrString;
