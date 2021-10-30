#include "util/string_util.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <cinttypes>
#include <cmath>
#include <sstream>
#include <string>
#include <utility>
#include <vector>
#include "port/port.h"

#include <string.h>
// 暂时忘记slice吧

const std::string kNullptrString = "nullptr";

std::vector<std::string> StringSplit(const std::string& arg,
                                     char delim) {
  std::vector<std::string> splits;
  // 用一个ss把string包裹起来
  std::stringstream ss(arg);
  std::string item;
  while (std::getline(ss, item, delim)) {
    splits.push_back(item);
  }
  return splits;
}

static std::string invoke_stderror_r(char* (*strerror_r)(int, char*, size_t),
                                     int err, char* buf, size_t buflen) {
  return strerror_r(err, buf, buflen);
}

std::string errnoStr(int err) {
  char buf[1024];
  buf[0] = '\0';
  std::string result;

  result.assign(invoke_stderror_r(strerror_r, err, buf, sizeof(buf)));
  return result;
}