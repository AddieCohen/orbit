// Copyright (c) 2020 The Orbit Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ORBIT_GL_THREAD_TRACK_H_
#define ORBIT_GL_THREAD_TRACK_H_

#include <stdint.h>

#include <map>
#include <memory>
#include <string>

#include "CallstackThreadBar.h"
#include "CoreMath.h"
#include "PickingManager.h"
#include "ScopeTree.h"
#include "TextBox.h"
#include "ThreadStateBar.h"
#include "TimerTrack.h"
#include "TracepointThreadBar.h"
#include "Track.h"
#include "Viewport.h"
#include "capture_data.pb.h"

class OrbitApp;

class ThreadTrack final : public TimerTrack {
 public:
  explicit ThreadTrack(CaptureViewElement* parent, TimeGraph* time_graph,
                       orbit_gl::Viewport* viewport, TimeGraphLayout* layout, int32_t thread_id,
                       OrbitApp* app, const orbit_client_model::CaptureData* capture_data,
                       uint32_t indentation_level = 0);

  void InitializeNameAndLabel(int32_t thread_id);

  [[nodiscard]] int32_t GetThreadId() const { return thread_id_; }

  [[nodiscard]] Type GetType() const override { return Type::kThreadTrack; }
  [[nodiscard]] std::string GetTooltip() const override;

  [[nodiscard]] const TextBox* GetLeft(const TextBox* textbox) const override;
  [[nodiscard]] const TextBox* GetRight(const TextBox* textbox) const override;

  void Draw(Batcher& batcher, TextRenderer& text_renderer, uint64_t current_mouse_time_ns,
            PickingMode picking_mode, float z_offset = 0) override;
  void UpdatePrimitives(Batcher* batcher, uint64_t min_tick, uint64_t max_tick,
                        PickingMode picking_mode, float z_offset = 0) override;
  void OnTimer(const orbit_client_protos::TimerInfo& timer_info) override;
  [[nodiscard]] virtual float GetYFromDepth(uint32_t depth) const override;

  void OnPick(int x, int y) override;

  void UpdateBoxHeight() override;
  void SetTrackColor(Color color);
  [[nodiscard]] bool IsEmpty() const override;

  [[nodiscard]] std::vector<CaptureViewElement*> GetVisibleChildren() override;

 protected:
  [[nodiscard]] std::string GetThreadNameFromTid(uint32_t tid);
  [[nodiscard]] bool IsTimerActive(const orbit_client_protos::TimerInfo& timer) const override;
  [[nodiscard]] bool IsTrackSelected() const override;

  [[nodiscard]] Color GetTimerColor(const orbit_client_protos::TimerInfo& timer, bool is_selected,
                                    bool is_highlighted) const override;
  [[nodiscard]] Color GetTimerColor(const TextBox& text_box, const internal::DrawData& draw_data);
  void SetTimesliceText(const orbit_client_protos::TimerInfo& timer, float min_x, float z_offset,
                        TextBox* text_box) override;
  [[nodiscard]] std::string GetBoxTooltip(const Batcher& batcher, PickingId id) const override;

  [[nodiscard]] float GetHeight() const override;
  [[nodiscard]] float GetHeaderHeight() const override;

  void UpdatePositionOfSubtracks();
  void UpdatePrimitivesOfSubtracks(Batcher* batcher, uint64_t min_tick, uint64_t max_tick,
                                   PickingMode picking_mode, float z_offset);
  void UpdateMinMaxTimestamps();

  std::shared_ptr<orbit_gl::ThreadStateBar> thread_state_bar_;
  std::shared_ptr<orbit_gl::CallstackThreadBar> event_bar_;
  std::shared_ptr<orbit_gl::TracepointThreadBar> tracepoint_bar_;

  absl::Mutex scope_tree_mutex_;
  ScopeTree<TextBox> scope_tree_;
  TimerChain* timer_chain_;
};

#endif  // ORBIT_GL_THREAD_TRACK_H_
