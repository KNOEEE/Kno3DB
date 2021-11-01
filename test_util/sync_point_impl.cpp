#include "test_util/sync_point_impl.h"

bool SyncPoint::Data::PredecessorsAllCleared(const std::string& point) {
  for (const auto& pred : predecessors_[point]) {
    if (cleared_points_.count(pred) == 0) {
      return false;
    }
  }
  return true;
}

void SyncPoint::Data::Process(const std::string& point_string, void* cb_arg) {
  if (!enabled_) {
    return;
  }
  // 如果没有就不要折腾了
  if (!point_filter_.MayContain(point_string)) {
    return;
  }
  std::unique_lock<std::mutex> lock(mutex_);
  auto thread_id = std::this_thread::get_id();

  auto marker_iter = markers_.find(point_string);
  if (marker_iter != markers_.end()) {
    for (auto& marked_point : marker_iter->second) {
      marked_thread_id_.emplace(marked_point, thread_id);
      point_filter_.Add(marked_point);
    }
  }

  if (DisabledByMarker(point_string, thread_id)) {
    return;
  }
  while (!PredecessorsAllCleared(point_string)) {
    /**
     * 阻塞 自动unlock 其他线程拿到锁就可以继续运行
     * 当notify的时候 就会唤醒wait 自动lock并往下执行
     * notify_all唤醒所有线程 但最终只能有一个线程得到锁
     * 其他没有得到控制权的线程会回到阻塞
     * 这个过程叫做虚假唤醒 解决办法就是像这样使用while循环
     * */
    cv_.wait(lock);
    if (DisabledByMarker(point_string, thread_id)) {
      return;
    }
  }

  auto callback_pair = callbacks_.find(point_string);
  if (callback_pair != callbacks_.end()) {
    num_callbacks_running_++;
    mutex_.unlock();
    callback_pair->second(cb_arg);
    mutex_.lock();
    num_callbacks_running_--;
  }
  cleared_points_.insert(point_string);
  cv_.notify_all();
}