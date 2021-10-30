// Encoding independent of machine byte order:
// * Fixed-length numbers are encoded with least-significant byte first
//   (little endian, native order on Intel and others)
//
// More functions in coding.h

#pragma once

#include <cstdint>
#include <cstring>

#include "port/port.h"  // for port::kLittleEndian

inline void EncodeFixed32(char* buf, uint32_t value) {
  if (port::kLittleEndian) {
    memcpy(buf, &value, sizeof(value));
  } else {
    buf[0] = value & 0xff;
    buf[1] = (value >> 8) & 0xff;
    buf[2] = (value >> 16) & 0xff;
    buf[3] = (value >> 24) & 0xff;
  }
}

inline uint32_t DecodeFixed32(const char* ptr) {
  if (port::kLittleEndian) {
    uint32_t result;
    memcpy(&result, ptr, sizeof(result));
    return result;
  } else {
    return ((static_cast<uint32_t>(static_cast<unsigned char>(ptr[0]))) |
            (static_cast<uint32_t>(static_cast<unsigned char>(ptr[1])) << 8) |
            (static_cast<uint32_t>(static_cast<unsigned char>(ptr[2])) << 16) |
            (static_cast<uint32_t>(static_cast<unsigned char>(ptr[3])) << 24));
  }
}