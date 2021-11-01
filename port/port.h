// 仅能用于Linux

#pragma once

#include <thread>

#include "kno3db/options.h"

#include <endian.h>
#include <pthread.h>
#include <stdint.h>
#include <limits>
#include <string>

#define PLATFORM_IS_LITTLE_ENDIAN (__BYTE_ORDER == __LITTLE_ENDIAN)

extern const bool kDefaultToAdaptiveMutex;

namespace port {
const uint32_t kMaxUint32 = std::numeric_limits<uint32_t>::max();
const int kMaxInt32 = std::numeric_limits<int32_t>::max();
const int kMinInt32 = std::numeric_limits<int32_t>::min();
const uint64_t kMaxUint64 = std::numeric_limits<uint64_t>::max();
const int64_t kMaxInt64 = std::numeric_limits<int64_t>::max();
const int64_t kMinInt64 = std::numeric_limits<int64_t>::min();
const size_t kMaxSizet = std::numeric_limits<size_t>::max();

constexpr bool kLittleEndian = PLATFORM_IS_LITTLE_ENDIAN;
#undef PLATFORM_IS_LITTLE_ENDIAN

class CondVar;

class Mutex {
public:
  explicit Mutex(bool adaptive = kDefaultToAdaptiveMutex);
  Mutex(const Mutex&) = delete;
  void operator=(const Mutex&) = delete;
  ~Mutex();

  void Lock();
  void Unlock();
  bool TryLock();
  // 判断mutex是否被held住了
  void AssertHeld();
private:
  friend class CondVar;
  pthread_mutex_t mu_;
  bool locked_ = false;
};

class RWMutex {
// 读写锁
public:
  RWMutex();
  // No copying allowed
  RWMutex(const RWMutex&) = delete;
  void operator=(const RWMutex&) = delete;

  ~RWMutex();

  void ReadLock();
  void WriteLock();
  void ReadUnlock();
  void WriteUnlock();
  void AssertHeld() { }
private:
  // 底层mutex
  pthread_rwlock_t mu_;
};

class CondVar {
public:
  explicit CondVar(Mutex* mu);
  ~CondVar();
  void Wait();
  // Timed condition wait.  Returns true if timeout occurred.
  bool TimedWait(uint64_t abs_time_us);
  void Signal();
  void SignalAll();
private:
  pthread_cond_t cv_;
  Mutex* mu_;
};

using Thread = std::thread;

static inline void AsmVolatilePause() {
  // A volatile specifier is a hint to a compiler that an object may change 
  // its value in ways not specified by the language so that 
  // aggressive optimizations must be avoided.
  // 编译器对访问该变量的代码不可进行优化
  asm volatile("pause");
}

// returns -1 if not available on the os
extern int PhysicalCoreID();

using OnceType = pthread_once_t;
#define LEVELDB_ONCE_INIT PTHREAD_ONCE_INIT;
extern void InitOnce(OnceType* once, void (*initializer)());

#define CACHE_LINE_SIZE 64U

// https://www.cnblogs.com/dongzhiquan/p/3694858.html
#define PREFETCH(addr, rw, locality) __builtin_prefetch(addr, rw, locality)

extern void Crash(const std::string& srcfile, int srcline);

extern int GetMaxOpenFiles();

extern const size_t kPageSize;

using ThreadId = pid_t;

extern void SetCpuPriority(ThreadId id, CpuPriority priority);

int64_t GetProcessID();

// Uses platform APIs to generate a 36-character RFC-4122 UUID. Returns
// true on success or false on failure.
bool GenerateRfcUuid(std::string* output);
}