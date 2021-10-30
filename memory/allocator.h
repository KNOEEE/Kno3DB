/**
 * 用block来分配内存的抽象接口 当allocator对象被销毁后
 * 内存也自动释放 这是最厉害的功能 详细实现见arena
 * */

#pragma once
#include <cerrno>
#include <cstddef>

// 这里涉及到另外两个类 logger和alloctracker
// 不过这里先放弃他们 不然从writeBufferManager开始追溯就太长了

class Allocator {
public:
  virtual ~Allocator() {}
  virtual char* Allocate(size_t bytes) = 0;
  virtual char* AllocateAligned(size_t bytes, size_t huge_page_size = 0,
                                int logger = 0) = 0;
  virtual size_t BlockSize() const = 0;
};