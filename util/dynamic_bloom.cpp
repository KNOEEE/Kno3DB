#include "dynamic_bloom.h"

#include <algorithm>

#include "memory/allocator.h"
#include "port/port.h"
#include <string.h>
#include "util/hash.h"

#include <assert.h>

namespace {
// 比x大的第一个pow2
uint32_t roundUpToPow2(uint32_t x) {
  uint32_t rv = 1;
  while (rv < x) {
    rv <<= 1;
  }
  return rv;
}
}

DynamicBloom::DynamicBloom(Allocator* allocator, uint32_t total_bits,
                           uint32_t num_probes, size_t huge_page_tlb_size)
    : kNumDoubleProbes((num_probes + (num_probes == 1)) / 2) {
  assert(num_probes % 2 == 0);
  assert(num_probes <= 10);
  assert(kNumDoubleProbes > 0);

  uint32_t block_bytes = 8 *
                         std::max(1U, roundUpToPow2(kNumDoubleProbes));
  uint32_t block_bits = block_bytes * 8;
  uint32_t blocks = (total_bits + block_bits - 1) / block_bits;
  // sz是整个bf的大小 单位byte
  uint32_t sz = blocks * block_bytes;
  // 将整个空间按照64bits区分的话 就是kLen
  kLen = sz / 8;
  assert(kLen > 0);

  for (uint32_t i = 0; i < kNumDoubleProbes; i++) {
    assert(((kLen - 1) ^ i) < kLen);
  }

  sz += block_bytes - 1;
  assert(allocator);

  char* raw = allocator->AllocateAligned(sz, huge_page_tlb_size, 0);
  memset(raw, 0, sz);
  auto block_offset = reinterpret_cast<uintptr_t>(raw) % block_bytes;
  if (block_offset > 0) {
    raw += block_bytes - block_offset;
  }
  static_assert(sizeof(std::atomic<uint64_t>) == sizeof(uint64_t),
                "Expecting 0-space-overhead atomic");
  data_ = reinterpret_cast<std::atomic<uint64_t>*>(raw);
}