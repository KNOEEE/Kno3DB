#include <assert.h>

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>

// #include "memory/concurrent_arena.h"
#include "memory/allocator.h"
#include "port/port.h"
#include "test_util/sync_point.h"
#include "util/dynamic_bloom.h"

#pragma once

// arena和syncPoint造成循环依赖 所以这里额外实现一个其他的allocator
class SingleAllocator : public Allocator {
public:
  char* Allocate(size_t) override {
    assert(false);
    return nullptr;
  }
  /** 
   * 难道是这样 在syncPOint这个场景中 并没有使用arena
   * 而是用了一个简单的resize再转位char*的方法分配内存
   * 这么一说 我也没必要复现arena了 就用这个简陋版好了
   * */
  char* AllocateAligned(size_t bytes, size_t, int) override {
    buf_.resize(bytes);
    return const_cast<char*>(buf_.data());
  }
  size_t BlockSize() const override {
    assert(false);
    return 0;
  }
private:
  std::string buf_;
};

struct SyncPoint::Data {
  Data() : point_filter_(&alloc_, 8192), enabled_(false) {}
  virtual ~Data() {}
  // successor/predecessor map loaded from LoadDependency
  std::unordered_map<std::string, std::vector<std::string>> successors_;
  std::unordered_map<std::string, std::vector<std::string>> predecessors_;
  std::unordered_map<std::string, std::function<void(void*)> > callbacks_;
  std::unordered_map<std::string, std::vector<std::string> > markers_;
  std::unordered_map<std::string, std::thread::id> marked_thread_id_;

  std::mutex              mutex_;
  std::condition_variable cv_;
  // sync points that have been passed through
  std::unordered_set<std::string> cleared_points_;
  SingleAllocator alloc_;
  // A filter before holding mutex to speed up process.
  DynamicBloom point_filter_;
  std::atomic<bool> enabled_;
  int num_callbacks_running_ = 0;

  bool PredecessorsAllCleared(const std::string& point);
  bool DisabledByMarker(const std::string& point,
                        std::thread::id thread_id) {
    auto marked_point_iter = marked_thread_id_.find(point);
    return marked_point_iter != marked_thread_id_.end() &&
           thread_id != marked_point_iter->second;
  }
  void Process(const std::string& point, void* cb_arg);
};