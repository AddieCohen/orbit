// Copyright (c) 2022 The Orbit Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ORBIT_CLIENT_DATA_API_SCOPE_ID_PROVIDER_H_
#define ORBIT_CLIENT_DATA_API_SCOPE_ID_PROVIDER_H_

#include <absl/container/flat_hash_map.h>

#include <cstdint>

#include "ClientData/TimerTrackDataIdManager.h"
#include "GrpcProtos/capture.pb.h"

namespace orbit_client_data {

// The inferface defines a map from `TimerInfo` to ids. When called twice on identical `TimerInfo`
// instances, it returns the same ids. This is used to allow for aggregation of manual
// instrumentation events in the same function as we aggregate the events for hooked functions (see
// go/stadia-orbit-manual-instrumentation-aggregation).
class ScopeIdProvider {
 public:
  virtual ~ScopeIdProvider() = default;

  [[nodiscard]] virtual uint64_t FunctionIdToScopeId(uint64_t function_id) const = 0;

  [[nodiscard]] virtual uint64_t ProvideId(const TimerInfo& timer_info) = 0;

  [[nodiscard]] virtual std::vector<uint64_t> GetAllProvidedScopeIds() const = 0;

  [[nodiscard]] virtual const std::string& GetScopeName(uint64_t scope_id) const = 0;
};

// This class, unless the timer does already have an id (`function_id`), it assigning an id for
// timers based on the equality of their names and types. Two timers are provided with the same ids
// if and only if they share the name and the type. Currently this is only implemented for manual
// instrumentation scopes, both sync and async.
class NameEqualityScopeIdProvider : public ScopeIdProvider {
 public:
  // Ids for instrumented functions are precomputed on capture start and we are using id range above
  // those.
  [[nodiscard]] static std::unique_ptr<NameEqualityScopeIdProvider> Create(
      const ::orbit_grpc_protos::CaptureOptions& capture_options);

  [[nodiscard]] uint64_t FunctionIdToScopeId(uint64_t function_id) const override;

  [[nodiscard]] uint64_t ProvideId(const TimerInfo& timer_info) override;

  [[nodiscard]] std::vector<uint64_t> GetAllProvidedScopeIds() const override;

  [[nodiscard]] const std::string& GetScopeName(uint64_t scope_id) const override;

 private:
  explicit NameEqualityScopeIdProvider(uint64_t start_id,
                                       absl::flat_hash_map<uint64_t, std::string> scope_id_to_name)
      : next_id_(start_id), scope_id_to_name_(std::move(scope_id_to_name)) {}

  absl::flat_hash_map<std::pair<orbit_client_protos::TimerInfo_Type, std::string>, uint64_t>
      name_to_id_;
  uint64_t next_id_{};

  absl::flat_hash_map<uint64_t, std::string> scope_id_to_name_;
};

}  // namespace orbit_client_data

#endif  // ORBIT_CLIENT_DATA_API_SCOPE_ID_PROVIDER_H_
