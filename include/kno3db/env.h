#pragma once

#include <stdint.h>

#include <cstdarg>
#include <functional>
#include <limits>
#include <memory>
#include <string>
#include <vector>

const size_t kDefaultPageSize = 4 * 1024;
enum class CpuPriority {
  kIdle = 0,
  kLow = 1,
  kNormal = 2,
  kHigh = 3,
};