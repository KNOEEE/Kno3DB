#pragma once

#include <assert.h>

#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>


class SyncPoint {
public:
  static SyncPoint* GetInstance();
  SyncPoint(const SyncPoint&) = delete;
  SyncPoint& operator=(const SyncPoint&) = delete;
  ~SyncPoint();

  struct SyncPointPair {
    std::string predecessor;
    std::string successor;
  };

  void Process(const std::string& point, void* cb_arg = nullptr);
  struct Data;
private:
  // singleton
  SyncPoint();
  Data* impl_;
};

#define TEST_SYNC_POINT(x) \
  SyncPoint::GetInstance()->Process(x)
#define TEST_SYNC_POINT_CALLBACK(x, y) \
  SyncPoint::GetInstance()->Process(x, y)
#define INIT_SYNC_POINT_SINGLETON() \
  (void)SyncPoint::GetInstance()