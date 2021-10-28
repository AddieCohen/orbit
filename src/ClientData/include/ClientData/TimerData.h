// Copyright (c) 2021 The Orbit Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CLIENT_DATA_TIMER_DATA_H_
#define CLIENT_DATA_TIMER_DATA_H_

#include <absl/base/thread_annotations.h>
#include <absl/synchronization/mutex.h>

#include "OrbitBase/ThreadConstants.h"
#include "TimerChain.h"
#include "TimerDataInterface.h"
#include "capture_data.pb.h"

namespace orbit_client_data {

class TimerData final : public TimerDataInterface {
 public:
  const orbit_client_protos::TimerInfo& AddTimer(orbit_client_protos::TimerInfo timer_info,
                                                 uint32_t depth = 0) override;

  [[nodiscard]] virtual std::vector<const orbit_client_protos::TimerInfo*> GetTimers(
      uint64_t /*min_tick*/ = std::numeric_limits<uint64_t>::min(),
      uint64_t /*max_tick*/ = std::numeric_limits<uint64_t>::max()) const override {
    // TODO(b/204173236): Implement GetTimers and use it in TimerTracks.
    return {};
  }

  // TODO(b/204173036): Test depth and process_id.
  TimerMetadata GetTimerMetadata() const override;

  [[nodiscard]] std::vector<const TimerChain*> GetChains() const override;
  [[nodiscard]] const TimerChain* GetChain(uint64_t depth) const;

  [[nodiscard]] const orbit_client_protos::TimerInfo* GetFirstAfterStartTime(uint64_t time,
                                                                             uint32_t depth) const;
  [[nodiscard]] const orbit_client_protos::TimerInfo* GetFirstBeforeStartTime(uint64_t time,
                                                                              uint32_t depth) const;

  const orbit_client_protos::TimerInfo* GetLeft(
      const orbit_client_protos::TimerInfo& timer_info) const override {
    return GetFirstBeforeStartTime(timer_info.start(), timer_info.depth());
  }

  const orbit_client_protos::TimerInfo* GetRight(
      const orbit_client_protos::TimerInfo& timer_info) const override {
    return GetFirstAfterStartTime(timer_info.start(), timer_info.depth());
  }

  const orbit_client_protos::TimerInfo* GetUp(
      const orbit_client_protos::TimerInfo& timer_info) const override {
    return GetFirstBeforeStartTime(timer_info.start(), timer_info.depth() - 1);
  }

  const orbit_client_protos::TimerInfo* GetDown(
      const orbit_client_protos::TimerInfo& timer_info) const override {
    return GetFirstAfterStartTime(timer_info.start(), timer_info.depth() + 1);
  }

  // Unused methods needed in TimerDataInterface
  [[nodiscard]] int64_t GetThreadId() const override { return -1; }
  virtual void OnCaptureComplete() override {}

 private:
  void UpdateMinTime(uint64_t min_time);
  void UpdateMaxTime(uint64_t max_time);
  void UpdateMaxDepth(uint32_t depth) { max_depth_ = std::max(max_depth_, depth); }
  [[nodiscard]] TimerChain* GetOrCreateTimerChain(uint64_t depth);

  uint32_t max_depth_ = 0;
  mutable absl::Mutex mutex_;
  std::map<uint32_t, std::unique_ptr<TimerChain>> timers_ ABSL_GUARDED_BY(mutex_);
  std::atomic<size_t> num_timers_{0};
  std::atomic<uint64_t> min_time_{std::numeric_limits<uint64_t>::max()};
  std::atomic<uint64_t> max_time_{std::numeric_limits<uint64_t>::min()};

  uint32_t process_id_ = orbit_base::kInvalidProcessId;
};

}  // namespace orbit_client_data

#endif  // CLIENT_DATA_TIMER_DATA_H_
