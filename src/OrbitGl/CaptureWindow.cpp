// Copyright (c) 2020 The Orbit Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "CaptureWindow.h"

#include <absl/time/time.h>
#include <glad/glad.h>
#include <imgui.h>
#include <string.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <functional>
#include <iterator>
#include <ostream>
#include <string_view>
#include <tuple>

#include "AccessibleTimeGraph.h"
#include "App.h"
#include "CaptureViewElement.h"
#include "ClientData/CallstackData.h"
#include "ClientData/CaptureData.h"
#include "ClientProtos/capture_data.pb.h"
#include "CoreMath.h"
#include "DisplayFormats/DisplayFormats.h"
#include "Geometry.h"
#include "GlUtils.h"
#include "ImGuiOrbit.h"
#include "Introspection/Introspection.h"
#include "OrbitAccessibility/AccessibleInterface.h"
#include "OrbitBase/Append.h"
#include "OrbitBase/Logging.h"
#include "OrbitBase/Profiling.h"
#include "OrbitBase/ThreadConstants.h"
#include "TextRenderer.h"
#include "TimeGraphLayout.h"
#include "absl/base/casts.h"

using orbit_accessibility::AccessibleInterface;
using orbit_accessibility::AccessibleWidgetBridge;

using orbit_client_data::CaptureData;
using orbit_gl::PickingUserData;
using orbit_gl::PrimitiveAssembler;

constexpr const char* kTimingDraw = "Draw";
constexpr const char* kTimingDrawAndUpdatePrimitives = "Draw & Update Primitives";
constexpr const char* kTimingFrame = "Complete Frame";

class AccessibleCaptureWindow : public AccessibleWidgetBridge {
 public:
  explicit AccessibleCaptureWindow(CaptureWindow* window) : window_(window) {}

  [[nodiscard]] int AccessibleChildCount() const override {
    if (window_->GetTimeGraph() == nullptr) {
      return 0;
    }
    return 1;
  }

  [[nodiscard]] const AccessibleInterface* AccessibleChild(int /*index*/) const override {
    if (window_->GetTimeGraph() == nullptr) {
      return nullptr;
    }
    return window_->GetTimeGraph()->GetOrCreateAccessibleInterface();
  }

 private:
  CaptureWindow* window_;
};

using orbit_client_protos::TimerInfo;

CaptureWindow::CaptureWindow(OrbitApp* app) : GlCanvas(), app_{app}, capture_client_app_{app} {
  draw_help_ = true;

  scoped_frame_times_[kTimingDraw] = std::make_unique<orbit_gl::SimpleTimings>(30);
  scoped_frame_times_[kTimingDrawAndUpdatePrimitives] =
      std::make_unique<orbit_gl::SimpleTimings>(30);
  scoped_frame_times_[kTimingFrame] = std::make_unique<orbit_gl::SimpleTimings>(30);

  slider_ = std::make_shared<orbit_gl::GlHorizontalSlider>(viewport_);
  vertical_slider_ = std::make_shared<orbit_gl::GlVerticalSlider>(viewport_);

  slider_->SetDragCallback([&](float ratio) {
    this->UpdateHorizontalScroll(ratio);
    RequestUpdatePrimitives();
  });
  slider_->SetResizeCallback([&](float normalized_start, float normalized_end) {
    this->UpdateHorizontalZoom(normalized_start, normalized_end);
    RequestUpdatePrimitives();
  });

  vertical_slider_->SetDragCallback([&](float ratio) {
    this->UpdateVerticalScroll(ratio);
    RequestUpdatePrimitives();
  });

  vertical_slider_->SetOrthogonalSliderPixelHeight(slider_->GetPixelHeight());
  slider_->SetOrthogonalSliderPixelHeight(vertical_slider_->GetPixelHeight());
}

void CaptureWindow::PreRender() {
  GlCanvas::PreRender();

  if (ShouldAutoZoom()) {
    ZoomAll();
  }

  if (time_graph_ != nullptr) {
    time_graph_->UpdateLayout();
    UpdateChildrenPosAndSize();
  }
}

void CaptureWindow::ZoomAll() {
  ResetHoverTimer();
  RequestUpdatePrimitives();
  if (time_graph_ == nullptr) return;
  time_graph_->ZoomAll();
}

void CaptureWindow::MouseMoved(int x, int y, bool left, bool right, bool middle) {
  GlCanvas::MouseMoved(x, y, left, right, middle);

  if (!(left || right || middle)) {
    ProcessSliderMouseMoveEvents(x, y);
  }

  if (time_graph_ == nullptr) return;

  // Pan
  if (left && !picking_manager_.IsDragging() && !capture_client_app_->IsCapturing()) {
    Vec2i mouse_click_screen = viewport_.WorldToScreen(mouse_click_pos_world_);
    Vec2 mouse_pos_world = viewport_.ScreenToWorld({x, y});
    time_graph_->GetTrackContainer()->SetVerticalScrollingOffset(
        track_container_click_scrolling_offset_ + mouse_click_pos_world_[1] - mouse_pos_world[1]);
    time_graph_->PanTime(mouse_click_screen[0], x, viewport_.GetScreenWidth(), ref_time_click_);

    click_was_drag_ = true;
  }

  // Update selection timestamps
  if (is_selecting_) {
    select_stop_time_ = time_graph_->GetTickFromWorld(select_stop_pos_world_[0]);
  }
}

void CaptureWindow::LeftDown(int x, int y) {
  GlCanvas::LeftDown(x, y);

  click_was_drag_ = false;

  if (time_graph_ == nullptr) return;
  ref_time_click_ = time_graph_->GetTime(static_cast<double>(x) / viewport_.GetScreenWidth());
  track_container_click_scrolling_offset_ =
      time_graph_->GetTrackContainer()->GetVerticalScrollingOffset();
}

void CaptureWindow::LeftUp() {
  GlCanvas::LeftUp();

  if (!click_was_drag_ && background_clicked_) {
    app_->SelectTimer(nullptr);
    app_->set_selected_thread_id(orbit_base::kAllProcessThreadsTid);
    RequestUpdatePrimitives();
  }

  ProcessSliderMouseMoveEvents(mouse_move_pos_screen_[0], mouse_move_pos_screen_[1]);
}

void CaptureWindow::HandlePickedElement(PickingMode picking_mode, PickingId picking_id, int x,
                                        int y) {
  // Early-out: This makes sure the timegraph was not deleted in between redraw and mouse click
  if (time_graph_ == nullptr) return;
  PickingType type = picking_id.type;

  PrimitiveAssembler& primitive_assembler = GetBatcherById(picking_id.batcher_id);

  if (picking_mode == PickingMode::kClick) {
    background_clicked_ = false;
    const orbit_client_protos::TimerInfo* timer_info = primitive_assembler.GetTimerInfo(picking_id);
    if (timer_info != nullptr) {
      SelectTimer(timer_info);
    } else if (type == PickingType::kPickable) {
      picking_manager_.Pick(picking_id, x, y);
    } else {
      // If the background is clicked: The selection should only be cleared
      // if the user doesn't drag around the capture window.
      // This is handled later in CaptureWindow::LeftUp()
      background_clicked_ = true;
    }
  } else if (picking_mode == PickingMode::kHover) {
    std::string tooltip;

    if (type == PickingType::kPickable) {
      auto pickable = GetPickingManager().GetPickableFromId(picking_id);
      if (pickable) {
        tooltip = pickable->GetTooltip();
      }
    } else {
      PickingUserData* user_data = primitive_assembler.GetUserData(picking_id);

      if (user_data && user_data->generate_tooltip_) {
        tooltip = user_data->generate_tooltip_(picking_id);
      }
    }

    app_->SendTooltipToUi(tooltip);
  }
}

void CaptureWindow::SelectTimer(const TimerInfo* timer_info) {
  ORBIT_CHECK(time_graph_ != nullptr);
  if (timer_info == nullptr) return;

  app_->SelectTimer(timer_info);
  app_->set_selected_thread_id(timer_info->thread_id());

  if (double_clicking_) {
    // Zoom and center the text_box into the screen.
    time_graph_->Zoom(*timer_info);
  }
}

void CaptureWindow::PostRender() {
  if (picking_mode_ != PickingMode::kNone) {
    RequestUpdatePrimitives();
  }

  GlCanvas::PostRender();
}

void CaptureWindow::RightDown(int x, int y) {
  GlCanvas::RightDown(x, y);
  if (time_graph_ != nullptr) {
    select_start_time_ = time_graph_->GetTickFromWorld(select_start_pos_world_[0]);
  }
}

bool CaptureWindow::RightUp() {
  if (time_graph_ != nullptr && is_selecting_ &&
      (select_start_pos_world_[0] != select_stop_pos_world_[0]) && ControlPressed()) {
    float min_world = std::min(select_start_pos_world_[0], select_stop_pos_world_[0]);
    float max_world = std::max(select_start_pos_world_[0], select_stop_pos_world_[0]);

    double new_min =
        TicksToMicroseconds(time_graph_->GetCaptureMin(), time_graph_->GetTickFromWorld(min_world));
    double new_max =
        TicksToMicroseconds(time_graph_->GetCaptureMin(), time_graph_->GetTickFromWorld(max_world));

    time_graph_->SetMinMax(new_min, new_max);

    // Clear the selection display
    select_stop_pos_world_ = select_start_pos_world_;
  }

  if (app_->IsDevMode()) {
    auto result = selection_stats_.Generate(this, select_start_time_, select_stop_time_);
    if (result.has_error()) {
      ORBIT_ERROR("%s", result.error().message());
    }
  }

  ProcessSliderMouseMoveEvents(mouse_move_pos_screen_[0], mouse_move_pos_screen_[1]);

  return GlCanvas::RightUp();
}

void CaptureWindow::ZoomHorizontally(int delta, int mouse_x) {
  if (delta == 0) return;

  auto delta_float = static_cast<float>(delta);

  if (time_graph_ != nullptr) {
    double mouse_ratio = static_cast<double>(mouse_x) / time_graph_->GetWidth();
    time_graph_->ZoomTime(delta_float, mouse_ratio);
  }
}

void CaptureWindow::Pan(float ratio) {
  if (time_graph_ == nullptr) return;
  double ref_time = time_graph_->GetTime(static_cast<double>(mouse_move_pos_screen_[0]) /
                                         viewport_.GetScreenWidth());
  time_graph_->PanTime(mouse_move_pos_screen_[0],
                       mouse_move_pos_screen_[0] +
                           static_cast<int>(ratio * static_cast<float>(viewport_.GetScreenWidth())),
                       viewport_.GetScreenWidth(), ref_time);
  RequestUpdatePrimitives();
}

void CaptureWindow::MouseWheelMoved(int x, int y, int delta, bool ctrl) {
  GlCanvas::MouseWheelMoved(x, y, delta, ctrl);

  if (time_graph_ != nullptr) {
    orbit_gl::ModifierKeys modifiers;
    modifiers.ctrl = ctrl;
    std::ignore =
        time_graph_->HandleMouseWheelEvent(viewport_.ScreenToWorld(Vec2i(x, y)), delta, modifiers);
  }
}

void CaptureWindow::MouseWheelMovedHorizontally(int x, int y, int delta, bool ctrl) {
  GlCanvas::MouseWheelMovedHorizontally(x, y, delta, ctrl);

  if (delta == 0) return;

  if (delta > 0) {
    Pan(0.1f);
  } else {
    Pan(-0.1f);
  }
}

void CaptureWindow::KeyPressed(unsigned int key_code, bool ctrl, bool shift, bool alt) {
  GlCanvas::KeyPressed(key_code, ctrl, shift, alt);

  switch (key_code) {
    case ' ':
      if (!shift) {
        ZoomAll();
      }
      break;
    case 'A':
      Pan(0.1f);
      break;
    case 'D':
      Pan(-0.1f);
      break;
    case 'W':
      ZoomHorizontally(1, mouse_move_pos_screen_[0]);
      break;
    case 'S':
      ZoomHorizontally(-1, mouse_move_pos_screen_[0]);
      break;
    case 'X':
      ToggleRecording();
      break;
    case 18:  // Left
      if (time_graph_ == nullptr) return;
      if (shift) {
        time_graph_->JumpToNeighborTimer(app_->selected_timer(),
                                         TimeGraph::JumpDirection::kPrevious,
                                         TimeGraph::JumpScope::kSameFunction);
      } else if (alt) {
        time_graph_->JumpToNeighborTimer(app_->selected_timer(),
                                         TimeGraph::JumpDirection::kPrevious,
                                         TimeGraph::JumpScope::kSameThreadSameFunction);
      } else {
        time_graph_->JumpToNeighborTimer(app_->selected_timer(),
                                         TimeGraph::JumpDirection::kPrevious,
                                         TimeGraph::JumpScope::kSameDepth);
      }
      break;
    case 20:  // Right
      if (time_graph_ == nullptr) return;
      if (shift) {
        time_graph_->JumpToNeighborTimer(app_->selected_timer(), TimeGraph::JumpDirection::kNext,
                                         TimeGraph::JumpScope::kSameFunction);
      } else if (alt) {
        time_graph_->JumpToNeighborTimer(app_->selected_timer(), TimeGraph::JumpDirection::kNext,
                                         TimeGraph::JumpScope::kSameThreadSameFunction);
      } else {
        time_graph_->JumpToNeighborTimer(app_->selected_timer(), TimeGraph::JumpDirection::kNext,
                                         TimeGraph::JumpScope::kSameDepth);
      }
      break;
    case 19:  // Up

      if (time_graph_ == nullptr) return;
      time_graph_->JumpToNeighborTimer(app_->selected_timer(), TimeGraph::JumpDirection::kTop,
                                       TimeGraph::JumpScope::kSameThread);
      break;
    case 21:  // Down
      if (time_graph_ == nullptr) return;
      time_graph_->JumpToNeighborTimer(app_->selected_timer(), TimeGraph::JumpDirection::kDown,
                                       TimeGraph::JumpScope::kSameThread);
      break;
  }
}

void CaptureWindow::SetIsMouseOver(bool value) {
  GlCanvas::SetIsMouseOver(value);

  if (!value && last_mouseover_slider_ != nullptr) {
    last_mouseover_slider_->OnMouseLeave();
    last_mouseover_slider_ = nullptr;
    RequestRedraw();
  }
}

bool CaptureWindow::ShouldAutoZoom() const { return capture_client_app_->IsCapturing(); }

std::unique_ptr<AccessibleInterface> CaptureWindow::CreateAccessibleInterface() {
  return std::make_unique<AccessibleCaptureWindow>(this);
}

void CaptureWindow::Draw() {
  ORBIT_SCOPE("CaptureWindow::Draw");
  uint64_t start_time_ns = orbit_base::CaptureTimestampNs();
  bool time_graph_was_redrawn = time_graph_ != nullptr && time_graph_->IsRedrawNeeded();

  text_renderer_.Init();

  if (ShouldSkipRendering()) {
    return;
  }

  if (ShouldAutoZoom()) {
    ZoomAll();
  }

  if (time_graph_ != nullptr) {
    uint64_t timegraph_current_mouse_time_ns =
        time_graph_->GetTickFromWorld(viewport_.ScreenToWorld(GetMouseScreenPos())[0]);
    time_graph_->DrawAllElements(GetPrimitiveAssembler(), GetTextRenderer(), picking_mode_,
                                 timegraph_current_mouse_time_ns);
  }

  RenderSelectionOverlay();

  if (picking_mode_ == PickingMode::kNone) {
    Vec2 pos = viewport_.ScreenToWorld(Vec2i(mouse_move_pos_screen_[0], 0));
    // Vertical green line at mouse x position
    ui_batcher_.AddVerticalLine(pos, viewport_.GetWorldHeight(), kZValueOverlay,
                                Color(0, 255, 0, 127));

    if (draw_help_) {
      RenderHelpUi();
    }
  }

  DrawScreenSpace();

  if (picking_mode_ == PickingMode::kNone) {
    text_renderer_.RenderDebug(&ui_batcher_);
  }

  if (picking_mode_ == PickingMode::kNone) {
    double update_duration_in_ms = (orbit_base::CaptureTimestampNs() - start_time_ns) / 1000000.0;
    if (time_graph_was_redrawn) {
      scoped_frame_times_[kTimingDrawAndUpdatePrimitives]->PushTimeMs(update_duration_in_ms);
    } else {
      scoped_frame_times_[kTimingDraw]->PushTimeMs(update_duration_in_ms);
    }
  }

  RenderAllLayers();

  if (picking_mode_ == PickingMode::kNone) {
    if (last_frame_start_time_ != 0) {
      double frame_duration_in_ms =
          (orbit_base::CaptureTimestampNs() - last_frame_start_time_) / 1000000.0;
      scoped_frame_times_[kTimingFrame]->PushTimeMs(frame_duration_in_ms);
    }
  }

  last_frame_start_time_ = orbit_base::CaptureTimestampNs();
}

void CaptureWindow::UpdateChildrenPosAndSize() {
  UpdateVerticalSliderFromWorld();
  // Horizontal slider width depends on the visibility of vertical slider.
  UpdateHorizontalSliderFromWorld();

  const TimeGraphLayout& layout = time_graph_->GetLayout();
  float right_margin = layout.GetRightMargin();
  if (vertical_slider_->IsVisible()) {
    int slider_width = static_cast<int>(time_graph_->GetLayout().GetSliderWidth());
    right_margin += slider_width;
  }

  UpdateRightMargin(right_margin);

  time_graph_->SetWidth(viewport_.GetWorldWidth() - right_margin);
}

void CaptureWindow::DrawScreenSpace() {
  ORBIT_SCOPE("CaptureWindow::DrawScreenSpace");
  if (time_graph_ == nullptr) return;
  uint64_t time_span = time_graph_->GetCaptureTimeSpanNs();

  Color col = slider_->GetBarColor();
  auto canvas_height = static_cast<float>(viewport_.GetScreenHeight());

  if (time_span > 0) {
    slider_->Draw(ui_batcher_, picking_manager_.IsThisElementPicked(slider_.get()));
    if (vertical_slider_->IsVisible()) {
      vertical_slider_->Draw(ui_batcher_,
                             picking_manager_.IsThisElementPicked(vertical_slider_.get()));
    }
  }

  // Right vertical margin.
  auto margin_x1 = static_cast<float>(viewport_.GetScreenWidth());
  float margin_x0 = margin_x1 - GetRightMargin();

  Tetragon box = MakeBox(Vec2(margin_x0, 0), Vec2(margin_x1 - margin_x0, canvas_height),
                         GlCanvas::kZValueMargin);
  ui_batcher_.AddBox(box, kBackgroundColor);
}

void CaptureWindow::RenderAllLayers() {
  std::vector<float> all_layers{};
  if (time_graph_ != nullptr) {
    all_layers = time_graph_->GetPrimitiveAssembler().GetLayers();
    orbit_base::Append(all_layers, time_graph_->GetTextRenderer()->GetLayers());
  }
  orbit_base::Append(all_layers, ui_batcher_.GetLayers());
  orbit_base::Append(all_layers, text_renderer_.GetLayers());

  // Sort and remove duplicates.
  std::sort(all_layers.begin(), all_layers.end());
  auto it = std::unique(all_layers.begin(), all_layers.end());
  all_layers.resize(std::distance(all_layers.begin(), it));
  if (all_layers.size() > GlCanvas::kMaxNumberRealZLayers) {
    ORBIT_ERROR("Too many z-layers. The current number is %d", all_layers.size());
  }

  for (float layer : all_layers) {
    if (time_graph_ != nullptr) {
      time_graph_->GetPrimitiveAssembler().DrawLayer(layer, picking_mode_ != PickingMode::kNone);
    }
    ui_batcher_.DrawLayer(layer, picking_mode_ != PickingMode::kNone);

    if (picking_mode_ == PickingMode::kNone) {
      text_renderer_.RenderLayer(layer);
      RenderText(layer);
    }
  }
}

void CaptureWindow::UpdateHorizontalScroll(float ratio) {
  if (time_graph_ == nullptr) return;
  time_graph_->UpdateHorizontalScroll(ratio);
}

void CaptureWindow::UpdateVerticalScroll(float ratio) {
  if (time_graph_ == nullptr) return;
  float range = std::max(0.f, time_graph_->GetTrackContainer()->GetVisibleTracksTotalHeight() -
                                  viewport_.GetWorldHeight());
  float new_scrolling_offset = ratio * range;
  time_graph_->GetTrackContainer()->SetVerticalScrollingOffset(new_scrolling_offset);
}

void CaptureWindow::UpdateHorizontalZoom(float normalized_start, float normalized_end) {
  if (time_graph_ == nullptr) return;
  double time_span = time_graph_->GetCaptureTimeSpanUs();
  time_graph_->SetMinMax(normalized_start * time_span, normalized_end * time_span);
}

void CaptureWindow::UpdateHorizontalSliderFromWorld() {
  if (time_graph_ == nullptr) return;
  double time_span = time_graph_->GetCaptureTimeSpanUs();
  double start = time_graph_->GetMinTimeUs();
  double stop = time_graph_->GetMaxTimeUs();
  double width = stop - start;
  double max_start = time_span - width;

  constexpr double kEpsilon = 1e-8;
  double ratio =
      capture_client_app_->IsCapturing() ? 1 : (max_start > kEpsilon ? start / max_start : 0);
  int slider_width = static_cast<int>(time_graph_->GetLayout().GetSliderWidth());
  slider_->SetPixelHeight(slider_width);
  slider_->SetNormalizedLength(static_cast<float>(width / time_span));
  slider_->SetNormalizedPosition(static_cast<float>(ratio));

  slider_->SetOrthogonalSliderPixelHeight(vertical_slider_->IsVisible() ? slider_width : 0);
}

void CaptureWindow::ProcessSliderMouseMoveEvents(int x, int y) {
  orbit_gl::GlSlider* slider = FindSliderUnderMouseCursor(x, y);
  if (slider != last_mouseover_slider_) {
    if (last_mouseover_slider_ != nullptr) {
      last_mouseover_slider_->OnMouseLeave();
    }
    last_mouseover_slider_ = slider;
    if (slider != nullptr) {
      slider->OnMouseEnter();
    }
  }

  if (slider != nullptr) {
    slider->OnMouseMove(x, y);
  }
}

void CaptureWindow::UpdateVerticalSliderFromWorld() {
  if (time_graph_ == nullptr) return;
  float visible_tracks_height = time_graph_->GetTrackContainer()->GetVisibleTracksTotalHeight();
  float max = std::max(0.f, visible_tracks_height - viewport_.GetWorldHeight());
  float pos_ratio =
      max > 0 ? time_graph_->GetTrackContainer()->GetVerticalScrollingOffset() / max : 0.f;
  float size_ratio =
      visible_tracks_height > 0 ? viewport_.GetWorldHeight() / visible_tracks_height : 1.f;
  int slider_width = static_cast<int>(time_graph_->GetLayout().GetSliderWidth());
  vertical_slider_->SetPixelHeight(slider_width);
  vertical_slider_->SetNormalizedPosition(pos_ratio);
  vertical_slider_->SetNormalizedLength(size_ratio);
  vertical_slider_->SetOrthogonalSliderPixelHeight(slider_width);
}

void CaptureWindow::ToggleRecording() {
  capture_client_app_->ToggleCapture();
  draw_help_ = false;
#ifdef __linux__
  ZoomAll();
#endif
}

orbit_gl::GlSlider* CaptureWindow::FindSliderUnderMouseCursor(int x, int y) {
  for (orbit_gl::GlSlider* slider : {vertical_slider_.get(), slider_.get()}) {
    if (slider->ContainsScreenSpacePoint(x, y)) {
      return slider;
    }
  }

  return nullptr;
}

bool CaptureWindow::ShouldSkipRendering() const {
  // Don't render when loading a capture to avoid contention with the loading thread.
  return app_->IsLoadingCapture();
}

void CaptureWindow::set_draw_help(bool draw_help) {
  draw_help_ = draw_help;
  RequestRedraw();
}

void CaptureWindow::CreateTimeGraph(CaptureData* capture_data) {
  time_graph_ =
      std::make_unique<TimeGraph>(this, app_, &viewport_, capture_data, &GetPickingManager());
}

PrimitiveAssembler& CaptureWindow::GetBatcherById(BatcherId batcher_id) {
  switch (batcher_id) {
    case BatcherId::kTimeGraph:
      ORBIT_CHECK(time_graph_ != nullptr);
      return time_graph_->GetPrimitiveAssembler();
    case BatcherId::kUi:
      return ui_batcher_;
    default:
      ORBIT_UNREACHABLE();
  }
}

void CaptureWindow::RequestUpdatePrimitives() {
  redraw_requested_ = true;
  if (time_graph_ == nullptr) return;
  time_graph_->RequestUpdate();
}

[[nodiscard]] bool CaptureWindow::IsRedrawNeeded() const {
  return GlCanvas::IsRedrawNeeded() || (time_graph_ != nullptr && time_graph_->IsRedrawNeeded());
}

void CaptureWindow::RenderImGuiDebugUI() {
  if (ImGui::CollapsingHeader("Layout Properties")) {
    if (time_graph_ != nullptr && time_graph_->GetLayout().DrawProperties()) {
      RequestUpdatePrimitives();
    }

    static bool draw_text_outline = false;
    if (ImGui::Checkbox("Draw Text Outline", &draw_text_outline)) {
      TextRenderer::SetDrawOutline(draw_text_outline);
      RequestUpdatePrimitives();
    }
  }

  if (ImGui::CollapsingHeader("Capture Info")) {
    IMGUI_VAR_TO_TEXT(viewport_.GetScreenWidth());
    IMGUI_VAR_TO_TEXT(viewport_.GetScreenHeight());
    IMGUI_VAR_TO_TEXT(viewport_.GetWorldWidth());
    IMGUI_VAR_TO_TEXT(viewport_.GetWorldHeight());
    IMGUI_VAR_TO_TEXT(mouse_move_pos_screen_[0]);
    IMGUI_VAR_TO_TEXT(mouse_move_pos_screen_[1]);
    if (time_graph_ != nullptr) {
      IMGUI_VAR_TO_TEXT(time_graph_->GetTrackContainer()->GetNumVisiblePrimitives());
      IMGUI_VAR_TO_TEXT(time_graph_->GetTrackManager()->GetAllTracks().size());
      IMGUI_VAR_TO_TEXT(time_graph_->GetMinTimeUs());
      IMGUI_VAR_TO_TEXT(time_graph_->GetMaxTimeUs());
      IMGUI_VAR_TO_TEXT(time_graph_->GetCaptureMin());
      IMGUI_VAR_TO_TEXT(time_graph_->GetCaptureMax());
      IMGUI_VAR_TO_TEXT(time_graph_->GetTimeWindowUs());
      const CaptureData* capture_data = time_graph_->GetCaptureData();
      if (capture_data != nullptr) {
        IMGUI_VAR_TO_TEXT(capture_data->GetCallstackData().GetCallstackEventsCount());
      }
    }
  }

  if (ImGui::CollapsingHeader("Performance")) {
    for (auto& item : scoped_frame_times_) {
      IMGUI_VARN_TO_TEXT(item.second->GetAverageTimeMs(),
                         (std::string("Avg time in ms: ") + item.first));
      IMGUI_VARN_TO_TEXT(item.second->GetMinTimeMs(),
                         (std::string("Min time in ms: ") + item.first));
      IMGUI_VARN_TO_TEXT(item.second->GetMaxTimeMs(),
                         (std::string("Max time in ms: ") + item.first));
    }
  }

  if (ImGui::CollapsingHeader("Selection Summary")) {
    const std::string& selection_summary = selection_stats_.GetSummary();

    if (ImGui::Button("Copy to clipboard")) {
      app_->SetClipboard(selection_summary);
    }

    ImGui::TextUnformatted(selection_summary.c_str(),
                           selection_summary.c_str() + selection_summary.size());
  }
}

void CaptureWindow::RenderText(float layer) {
  ORBIT_SCOPE_FUNCTION;
  if (time_graph_ == nullptr) return;
  if (picking_mode_ == PickingMode::kNone) {
    time_graph_->DrawText(layer);
  }
}

void CaptureWindow::RenderHelpUi() {
  constexpr int kOffset = 30;
  Vec2 world_pos = viewport_.ScreenToWorld(Vec2i(kOffset, kOffset));

  Vec2 text_bounding_box_pos;
  Vec2 text_bounding_box_size;
  // TODO(b/180312795): Use TimeGraphLayout's font size again.
  text_renderer_.AddText(GetHelpText(), world_pos[0], world_pos[1], GlCanvas::kZValueUi,
                         {14, Color(255, 255, 255, 255), -1.f /*max_size*/}, &text_bounding_box_pos,
                         &text_bounding_box_size);

  const Color kBoxColor(50, 50, 50, 230);
  const float kMargin = 15.f;
  const float kRoundingRadius = 20.f;
  ui_batcher_.AddRoundedBox(text_bounding_box_pos, text_bounding_box_size, GlCanvas::kZValueUi,
                            kRoundingRadius, kBoxColor, kMargin);
}

const char* CaptureWindow::GetHelpText() const {
  const char* help_message =
      "Start/Stop Capture: 'F5'\n"
      "Pan: 'A','D' or \"Left Click + Drag\"\n"
      "Zoom: 'W', 'S', Scroll or \"Ctrl + Right Click + Drag\"\n"
      "Vertical Zoom: \"Ctrl + Scroll\"\n"
      "Select: Left Click\n"
      "Measure: \"Right Click + Drag\"\n"
      "Toggle Help: Ctrl + 'H'";
  return help_message;
}

inline double GetIncrementMs(double milli_seconds) {
  constexpr double kDay = 24 * 60 * 60 * 1000;
  constexpr double kHour = 60 * 60 * 1000;
  constexpr double kMinute = 60 * 1000;
  constexpr double kSecond = 1000;
  constexpr double kMilli = 1;
  constexpr double kMicro = 0.001;
  constexpr double kNano = 0.000001;

  std::string res;

  if (milli_seconds < kMicro) {
    return kNano;
  }
  if (milli_seconds < kMilli) {
    return kMicro;
  }
  if (milli_seconds < kSecond) {
    return kMilli;
  }
  if (milli_seconds < kMinute) {
    return kSecond;
  }
  if (milli_seconds < kHour) {
    return kMinute;
  }
  if (milli_seconds < kDay) {
    return kHour;
  }
  return kDay;
}

void CaptureWindow::RenderSelectionOverlay() {
  if (time_graph_ == nullptr) return;
  if (picking_mode_ != PickingMode::kNone) return;
  if (select_start_pos_world_[0] == select_stop_pos_world_[0]) return;

  uint64_t min_time = std::min(select_start_time_, select_stop_time_);
  uint64_t max_time = std::max(select_start_time_, select_stop_time_);

  float from_world = time_graph_->GetWorldFromTick(min_time);
  float to_world = time_graph_->GetWorldFromTick(max_time);
  float stop_pos_world = time_graph_->GetWorldFromTick(select_stop_time_);

  float size_x = to_world - from_world;
  // TODO(http://b/226401787): Allow green selection overlay to be on top of the Timeline after
  // modifying its design and how the overlay is drawn
  float initial_y_position = time_graph_->GetLayout().GetTimeBarHeight();
  Vec2 pos(from_world, initial_y_position);
  Vec2 size(size_x, viewport_.GetWorldHeight() - initial_y_position);

  std::string text = orbit_display_formats::GetDisplayTime(TicksToDuration(min_time, max_time));
  const Color color(0, 128, 0, 128);

  Tetragon box = MakeBox(pos, size, GlCanvas::kZValueOverlay);
  ui_batcher_.AddBox(box, color);

  TextRenderer::HAlign alignment = select_stop_pos_world_[0] < select_start_pos_world_[0]
                                       ? TextRenderer::HAlign::Left
                                       : TextRenderer::HAlign::Right;
  TextRenderer::TextFormatting formatting;
  formatting.font_size = time_graph_->GetLayout().CalculateZoomedFontSize();
  formatting.color = Color(255, 255, 255, 255);
  formatting.halign = alignment;

  text_renderer_.AddText(text.c_str(), stop_pos_world, select_stop_pos_world_[1],
                         GlCanvas::kZValueOverlay, formatting);

  const unsigned char g = 100;
  Color grey(g, g, g, 255);
  ui_batcher_.AddVerticalLine(pos, size[1], GlCanvas::kZValueOverlay, grey);
}
