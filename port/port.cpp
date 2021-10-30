#include "port/port.h"
#include <assert.h>
#include <cpuid.h>
#include <errno.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

#include <cstdlib>
#include <fstream>
#include <string>

#include "util/string_util.h"

extern const bool kDefaultToAdaptiveMutex = false;

namespace port {

static int PthreadCall(const char* label, int result) {
  // 超时和busy在允许的范围内
  // 其他情况则会generate core
  if (result != 0 && result != ETIMEDOUT && result != EBUSY) {
    fprintf(stderr, "pthread %s: %s\n", label, errnoStr(result).c_str());
    abort();
  }
  return result;
}

Mutex::Mutex(bool adaptive) {
  (void) adaptive;
  // 这里有复杂逻辑来生成adaptive的mu
  // 省略 来普通的构建
  PthreadCall("init mutex", pthread_mutex_init(&mu_, nullptr));
}

Mutex::~Mutex() {
  PthreadCall("destroy mutex", pthread_mutex_destroy(&mu_));
}

void Mutex::Lock() {
  PthreadCall("lock", pthread_mutex_lock(&mu_));
  locked_ = true;
}

void Mutex::Unlock() {
  locked_ = false;
  PthreadCall("unlock", pthread_mutex_unlock(&mu_));
}

bool Mutex::TryLock() {
  bool ret = PthreadCall("trylock", pthread_mutex_trylock(&mu_)) == 0;
  if (ret) {
    locked_ = true;
  }
  return ret;
}

void Mutex::AssertHeld() {
  assert(locked_);
}

CondVar::CondVar(Mutex* mu) : mu_(mu) {
  PthreadCall("init cv", pthread_cond_init(&cv_, nullptr));
}

CondVar::~CondVar() {
  PthreadCall("destroy cv", pthread_cond_destroy(&cv_));
}

void CondVar::Wait() {
  mu_->locked_ = false;
  // 这里传了第二个参数是什么意思呢
  PthreadCall("wait", pthread_cond_wait(&cv_, &mu_->mu_));
  mu_->locked_ = true;
}

bool CondVar::TimedWait(uint64_t abs_time_us) {
  struct timespec ts;
  ts.tv_sec = static_cast<time_t>(abs_time_us / 1e6);
  ts.tv_nsec = static_cast<suseconds_t>((abs_time_us % 1000000) * 1000);

  mu_->locked_ = false;
  int err = pthread_cond_timedwait(&cv_, &mu_->mu_, &ts);
  mu_->locked_ = true;
  if (err == ETIMEDOUT) {
    return true;
  }
  if (err != 0) {
    PthreadCall("timewait", err);
  }
  return false;
}

void CondVar::Signal() {
  PthreadCall("signal", pthread_cond_signal(&cv_));
}
void CondVar::SignalAll() {
  PthreadCall("broadcast", pthread_cond_broadcast(&cv_));
}

RWMutex::RWMutex() {
  PthreadCall("init rw mutex", pthread_rwlock_init(&mu_, nullptr));
}
RWMutex::~RWMutex() { PthreadCall("destroy mutex", pthread_rwlock_destroy(&mu_)); }

void RWMutex::ReadLock() { PthreadCall("read lock", pthread_rwlock_rdlock(&mu_)); }

void RWMutex::WriteLock() { PthreadCall("write lock", pthread_rwlock_wrlock(&mu_)); }

void RWMutex::ReadUnlock() { PthreadCall("read unlock", pthread_rwlock_unlock(&mu_)); }

void RWMutex::WriteUnlock() { PthreadCall("write unlock", pthread_rwlock_unlock(&mu_)); }

int PhysicalCoreID() {
  unsigned eax, ebx = 0, ecx, edx;
  if (!__get_cpuid(1, &eax, &ebx, &ecx, &edx)) {
    return -1;
  }
  return ebx >> 24;
}

void InitOnce(OnceType* once, void (*initializer)()) {
  PthreadCall("once", pthread_once(once, initializer));
}

void Crash(const std::string& srcfile, int srcline) {
  fprintf(stdout, "Crashing at %s:%d\n", srcfile.c_str(), srcline);
  fflush(stdout);
  // 给当前进程发信号 kill -15
  kill(getpid(), SIGTERM);
}

int GetMaxOpenFiles() {
#if defined(RLIMIT_NOFILE)
  struct rlimit no_files_limit;
  if (getrlimit(RLIMIT_NOFILE, &no_files_limit) != 0) {
    return -1;
  }
  if (static_cast<uintmax_t>(no_files_limit.rlim_cur) >=
      static_cast<uintmax_t>(std::numeric_limits<int>::max())) {
    return std::numeric_limits<int>::max();
  }
  return static_cast<int>(no_files_limit.rlim_cur);
#endif
  return -1;
}

int64_t GetProcessID() { return getpid(); }
} // namespace port