// Arena is an implementation of Allocator class. For a request of small size,
// it allocates a block with pre-defined block size. For a request of big
// size, it uses malloc to directly get the requested size.

#pragma once
#include <sys/mman.h>
#include <assert.h>
#include <stdint.h>
#include <cerrno>
#include <cstddef>
#include <vector>
#include "memory/allocator.h"
#include "util/mutexlock.h"

class Arena : public Allocator {
public:
  Arena(const Arena&) = delete;
  void operator=(const Arena&) = delete;

  static const size_t kInlineSize = 2048;
  static const size_t kMinBlockSize;
  static const size_t kMaxBlockSize;

  // 甚至还有失败的情况 这时会fall back to normal case
  explicit Arena(size_t block_size = kMinBlockSize,
                 size_t huge_page_size = 0, void* reserved = nullptr);
  ~Arena();
};