#include "test_util/sync_point.h"

#include <fcntl.h>

#include "test_util/sync_point_impl.h"

SyncPoint* SyncPoint::GetInstance() {
  static SyncPoint sync_point;
  return &sync_point;
}

SyncPoint::SyncPoint() : impl_(new Data) {}

SyncPoint::~SyncPoint() {
  delete impl_;
}

void SyncPoint::Process(const std::string& point, void* cb_arg) {
  impl_->Process(point, cb_arg);
}