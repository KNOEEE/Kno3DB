#pragma once

#include <array>
#include <string>
#include "port/port.h"
#include "util/hash.h"

#include <atomic>
#include <memory>

// 为什么这里要再声明一次
// 为什么被实现的allocator类可以赋值给这里
class Allocator;

class DynamicBloom {
public:
  explicit DynamicBloom(Allocator* allocator, uint32_t total_bits,
                        uint32_t num_probes = 6, size_t huge_page_tlb_size = 0);
  ~DynamicBloom() {}

  void Add(const std::string& key);
  // Assuming single threaded access to this function.
  void AddHash(uint32_t hash);
  bool MayContain(const std::string& key) const;
  bool MayContainHash(uint32_t Hash) const;
private:
  uint32_t kLen;

  const uint32_t kNumDoubleProbes;
  std::atomic<uint64_t>* data_;

  template<typename OrFunc>
  void AddHash(uint32_t hash, const OrFunc& or_func);
  bool DoubleProbe(uint32_t h32, size_t a) const;
};

inline void DynamicBloom::Add(const std::string& key) {
  // 把key hash一次
  AddHash(BloomHash(key));
}

inline void DynamicBloom::AddHash(uint32_t hash) {
  // www.zhihu.com/question/24301047
  // 我人晕了
  AddHash(hash, [](std::atomic<uint64_t>* ptr, uint64_t mask) {
    ptr->store(ptr->load(std::memory_order_relaxed) | mask,
               std::memory_order_relaxed);
  });
}