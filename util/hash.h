// Common hash functions with convenient interfaces. If hashing a
// statically-sized input in a performance-critical context, consider
// calling a specific hash implementation directly, such as
// XXH3_64bits from xxhash.h.
//
// Since this is a very common header, implementation details are kept
// out-of-line. Out-of-lining also aids in tracking the time spent in
// hashing functions. Inlining is of limited benefit for runtime-sized
// hash inputs.

#pragma once

#include <cstddef>
#include <cstdint>

#include <string>
#include "util/fastrange.h"

// hash32
extern uint32_t Hash(const char* data, size_t n, uint32_t seed);

inline uint32_t BloomHash(const std::string& key) {
  return Hash(key.c_str(), key.size(), 0xbc9f1d34);
}