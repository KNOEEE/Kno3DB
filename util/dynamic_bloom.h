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
/** 
 * 这个实现对于准确度和速度的权衡大概是 0.9xBF op latency 1.1x【假阳率FP rate】
 * 考虑如下情况 FP rate上升1% 对于每个key的查询开销上升0.01xlook-up
 * 因此假阳率上升导致的开销的增加应该小于 bf优化减少的开销
 * */
// For simplicity and performance, the current implementation requires
// num_probes to be a multiple of two and <= 10.
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
  
  // 探测次数除以2 因为每次读写64bits探测2位
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

template <typename OrFunc>
inline void DynamicBloom::AddHash(uint32_t h32, const OrFunc& or_func) {
  // 姑且把这里当成是小于h32的一次映射
  // 这里a也必定小于kLen 因为乘积后右移32位
  size_t a = FastRange32(kLen, h32);
  // data_+a必定取到 因为第一次取a^0
  PREFETCH(data_ + a, 0, 3);
  // Expand/remix with 64-bit golden ratio
  // sh*t this is real ULLONG_MAX * 0.618
  uint64_t h = 0x9e3779b97f4a7c13ULL * h32;
  for (unsigned i = 0;; i++) {
    // Two bit probes per uint64_t probe
    // mask是64位中的随机2位为1
    uint64_t mask = 
        ((uint64_t)1 << (h & 63)) | ((uint64_t)1 << ((h >> 6) & 63));
    // 每次添加一个新的key就在原来的位置上添加2位
    or_func(&data_[a ^ i], mask);
    // 最大探测次数
    if (i + 1 >= kNumDoubleProbes) {
      return;
    }
    // 其实这里不就是把h旋转12位吗
    h = (h >> 12) | (h << 52);
  }
} 

inline bool DynamicBloom::MayContain(const std::string& key) const {
  return MayContainHash(BloomHash(key));
}

// Speed hacks in this implementation:
// * Uses fastrange instead of %
// * Minimum logic to determine first (and all) probed memory addresses.
//   (Uses constant bit-xor offsets from the starting probe address.)
// * (Major) Two probes per 64-bit memory fetch/write.
//   Code simplification / optimization: only allow even number of probes.
// * Very fast and effective (murmur-like) hash expansion/re-mixing. (At
// least on recent CPUs, integer multiplication is very cheap. Each 64-bit
// remix provides five pairs of bit addresses within a uint64_t.)
//   Code simplification / optimization: only allow up to 10 probes, from a
//   single 64-bit remix.
//
// The FP rate penalty for this implementation, vs. standard Bloom filter, is
// roughly 1.12x on top of the 1.15x penalty for a 512-bit cache-local Bloom.
// This implementation does not explicitly use the cache line size, but is
// effectively cache-local (up to 16 probes) because of the bit-xor offsetting.
inline bool DynamicBloom::MayContainHash(uint32_t h32) const {
  size_t a = FastRange32(kLen, h32);
  // 0表示读 3表示强局部性
  PREFETCH(data_ + a, 0, 3);
  return DoubleProbe(h32, a);
}

inline bool DynamicBloom::DoubleProbe(uint32_t h32, size_t byte_offset) const {
  // Expand/remix with 64-bit golden ratio
  uint64_t h = 0x9e3779b97f4a7c13ULL * h32;
  for (unsigned i = 0;; i++) {
    uint64_t mask = 
        ((uint64_t)1 << (h & 63)) | ((uint64_t)1 << ((h >> 6) & 63));
    // offset是固定的 但是每次探测的data的位置却不一样
    // 看起来与addhash过程中放置的位置一致
    uint64_t val = data_[byte_offset ^ i].load(std::memory_order_relaxed);
    if (i + 1 >= kNumDoubleProbes) {
      // last time
      return (val & mask) == mask;
    } else if ((val & mask != mask)) {
      // 在每次探测过程中val 在mask所在的位上都必须是1
      // 因为这是放置过程中与mask按位或的结果
      // 有任何一次不符合 都说明a不存在 这也符合bloom filter的定义
      return false;
    }
    h = (h >> 12) | (h << 52);
  }
}