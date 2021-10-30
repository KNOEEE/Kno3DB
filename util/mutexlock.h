#pragma once
#include <assert.h>
#include <atomic>
#include <mutex>
#include <thread>
#include "port/port.h"

// Helper class that locks a mutex on construction and unlocks the mutex when
// the destructor of the MutexLock object is invoked.
//
// Typical usage:
//
//   void MyClass::MyMethod() {
//     MutexLock l(&mu_);       // mu_ is an instance variable
//     ... some complex code, possibly with multiple return paths ...
//   }

class MutexLock {
public:
  explicit MutexLock(port::Mutex* mu) : mu_(mu) {
    this->mu_->Lock();
  }
  // No copying allowed
  MutexLock(const MutexLock &) = delete;
  void operator=(const MutexLock &) = delete;
  ~MutexLock() { this->mu_->Unlock(); }
private:
  port::Mutex *const mu_;
};