---
id: 6sb8h
name: Orbitgl Overview
file_version: 1.0.2
app_version: 0.9.4-6
file_blobs:
  src/OrbitGl/TextRenderer.h: e5210d2a4d26dda28f8c1efbbaba6200d711b958
  src/OrbitGl/Viewport.h: 2a352f02b45d98e8184728bce3565525fe7f11ea
  src/OrbitGl/TimeGraphLayout.h: 65e2821a0a5857e269a14311144ac657fffb8757
  src/OrbitGl/TimelineInfoInterface.h: 65fcd4d040b43f72fb1ca41d381f451da0764abc
  src/OrbitGl/PickingManager.h: 9d8a448058ca78a1262a029eb1e9c2f68804940a
  src/OrbitGl/CaptureViewElement.h: 412f8063f976040de4c684657adf1cb88067e255
  src/OrbitGl/PrimitiveAssembler.h: db95c7edf748ef747acb08ac8ae4cc6f0ef7b2c5
  src/OrbitGl/App.h: b5986a599717130929b163457c738522e86b2767
  src/ApiUtils/include/ApiUtils/Event.h: 30e68db345b1d1e312b97f94d57e39c4f8ad1bb9
---

This document describes Orbitgl, which is a major component in our system.

{Describe Orbitgl in a short paragraph.}

It is located under `📄 src/OrbitGl`

# Main Types

<br/>

## `TextRenderer`[<sup id="1LGIz2">↓</sup>](#f-1LGIz2)

A `TextRenderer`[<sup id="1LGIz2">↓</sup>](#f-1LGIz2) is {Explain this type and what it represents}
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/TextRenderer.h
```c
⬜ 28     struct texture_font_t;
⬜ 29     }  // namespace ftgl
⬜ 30     
🟩 31     class TextRenderer {
🟩 32      public:
🟩 33       enum class HAlign { Left, Right };
🟩 34       enum class VAlign { Top, Middle, Bottom };
🟩 35     
🟩 36       struct TextFormatting {
🟩 37         uint32_t font_size = 14;
🟩 38         Color color = Color(255, 255, 255, 255);
🟩 39         float max_size = -1.f;
🟩 40         HAlign halign = HAlign::Left;
🟩 41         VAlign valign = VAlign::Top;
🟩 42       };
🟩 43     
🟩 44       explicit TextRenderer();
🟩 45       ~TextRenderer();
🟩 46     
🟩 47       void Init();
🟩 48       void Clear();
🟩 49       void SetViewport(orbit_gl::Viewport* viewport) { viewport_ = viewport; }
🟩 50     
⬜ 51       void RenderLayer(float layer);
⬜ 52       void RenderDebug(orbit_gl::PrimitiveAssembler* primitive_assembler);
⬜ 53       [[nodiscard]] std::vector<float> GetLayers() const;
```

<br/>

## `Viewport`[<sup id="Z23yq9H">↓</sup>](#f-Z23yq9H)

A `Viewport`[<sup id="Z23yq9H">↓</sup>](#f-Z23yq9H) is {Explain this type and what it represents}
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/Viewport.h
```c
⬜ 28     // Viewport will indicate if any changes happened that require redraw of the contents in between
⬜ 29     // frames. See Viewport::IsDirty() for usage.
⬜ 30     
🟩 31     class Viewport {
⬜ 32      public:
⬜ 33       explicit Viewport(int width, int height);
⬜ 34       Viewport() = delete;
```

<br/>

## `TimeGraphLayout`[<sup id="zcsVb">↓</sup>](#f-zcsVb)

A `TimeGraphLayout`[<sup id="zcsVb">↓</sup>](#f-zcsVb) is {Explain this type and what it represents}
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/TimeGraphLayout.h
```c
⬜ 12     
⬜ 13     #include "OrbitBase/ThreadUtils.h"
⬜ 14     
🟩 15     class TimeGraphLayout {
🟩 16      public:
🟩 17       TimeGraphLayout();
🟩 18     
🟩 19       const float kMinScale = 0.333f;
🟩 20       const float kMaxScale = 3.f;
⬜ 21     
⬜ 22       float GetTextBoxHeight() const { return text_box_height_ * scale_; }
⬜ 23       float GetTextCoresHeight() const { return core_height_ * scale_; }
```

<br/>

## `TimelineInfoInterface`[<sup id="Z1T48Oo">↓</sup>](#f-Z1T48Oo)

A `TimelineInfoInterface`[<sup id="Z1T48Oo">↓</sup>](#f-Z1T48Oo) is {Explain this type and what it represents}
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/TimelineInfoInterface.h
```c
⬜ 9      
⬜ 10     namespace orbit_gl {
⬜ 11     
🟩 12     class TimelineInfoInterface {
⬜ 13      public:
⬜ 14       virtual ~TimelineInfoInterface() = default;
⬜ 15     
```

<br/>

## `PickingId`[<sup id="Ziaaga">↓</sup>](#f-Ziaaga)

A `PickingId`[<sup id="Ziaaga">↓</sup>](#f-Ziaaga) is {Explain this type and what it represents}

*   `element_id`[<sup id="N7v3X">↓</sup>](#f-N7v3X)
    
*   `type`[<sup id="14nHAI">↓</sup>](#f-14nHAI)
    
*   `batcher_id`[<sup id="Z12Gkn1">↓</sup>](#f-Z12Gkn1)
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/PickingManager.h
```c
⬜ 47     // considered.
⬜ 48     enum class BatcherId { kTimeGraph, kUi, kCount };
⬜ 49     
🟩 50     struct PickingId {
⬜ 51       static constexpr uint32_t kElementIDBitSize = 28;
⬜ 52       static constexpr uint32_t kPickingTypeBitSize = 3;
⬜ 53       static constexpr uint32_t kBatcherIDBitSize = 1;
```

<br/>

## `CaptureViewElement`[<sup id="14fDYA">↓</sup>](#f-14fDYA)

A `CaptureViewElement`[<sup id="14fDYA">↓</sup>](#f-14fDYA) is {Explain this type and what it represents}
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CaptureViewElement.h
```c
⬜ 23     };
⬜ 24     
⬜ 25     /* Base class for UI elements drawn underneath the capture window. */
🟩 26     class CaptureViewElement : public Pickable, public AccessibleInterfaceProvider {
⬜ 27      public:
⬜ 28       explicit CaptureViewElement(CaptureViewElement* parent, const Viewport* viewport,
⬜ 29                                   const TimeGraphLayout* layout);
```

<br/>

## `PrimitiveAssembler`[<sup id="Z1GVzn2">↓</sup>](#f-Z1GVzn2)

A `PrimitiveAssembler`[<sup id="Z1GVzn2">↓</sup>](#f-Z1GVzn2) is {Explain this type and what it represents}
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/PrimitiveAssembler.h
```c
⬜ 49     NOTE: PrimitiveAssembler has a few pure virtual functions that have to be implemented: A few
⬜ 50     AddInternalMethods, ResetElements(), GetLayers() and DrawLayers().
⬜ 51     **/
🟩 52     class PrimitiveAssembler {
⬜ 53      public:
⬜ 54       explicit PrimitiveAssembler(BatcherId batcher_id, PickingManager* picking_manager = nullptr)
⬜ 55           : batcher_id_(batcher_id), picking_manager_(picking_manager) {
```

<br/>

# Useful utilities

Here are a few utility types we use in Orbitgl

<br/>

# `ApiStringEvent`[<sup id="wLOrj">↓</sup>](#f-wLOrj)

Used for {Describe what this utility class is useful for}
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/App.h
```c
⬜ 180      void OnModuleUpdate(uint64_t timestamp_ns, orbit_grpc_protos::ModuleInfo module_info) override;
⬜ 181      void OnModulesSnapshot(uint64_t timestamp_ns,
⬜ 182                             std::vector<orbit_grpc_protos::ModuleInfo> module_infos) override;
🟩 183      void OnApiStringEvent(const orbit_client_data::ApiStringEvent& api_string_event) override;
⬜ 184      void OnApiTrackValue(const orbit_client_data::ApiTrackValue& api_track_value) override;
⬜ 185      void OnWarningEvent(orbit_grpc_protos::WarningEvent warning_event) override;
⬜ 186      void OnClockResolutionEvent(
```

<br/>

<!-- THIS IS AN AUTOGENERATED SECTION. DO NOT EDIT THIS SECTION DIRECTLY -->
### Swimm Note

<span id="f-wLOrj">ApiStringEvent</span>[^](#wLOrj) - "src/ApiUtils/include/ApiUtils/Event.h" L110
```c
struct ApiStringEvent {
```

<span id="f-Z12Gkn1">batcher_id</span>[^](#Z12Gkn1) - "src/OrbitGl/PickingManager.h" L111
```c
  BatcherId batcher_id;
```

<span id="f-14fDYA">CaptureViewElement</span>[^](#14fDYA) - "src/OrbitGl/CaptureViewElement.h" L26
```c
class CaptureViewElement : public Pickable, public AccessibleInterfaceProvider {
```

<span id="f-N7v3X">element_id</span>[^](#N7v3X) - "src/OrbitGl/PickingManager.h" L109
```c
  uint32_t element_id;
```

<span id="f-Ziaaga">PickingId</span>[^](#Ziaaga) - "src/OrbitGl/PickingManager.h" L50
```c
struct PickingId {
```

<span id="f-Z1GVzn2">PrimitiveAssembler</span>[^](#Z1GVzn2) - "src/OrbitGl/PrimitiveAssembler.h" L52
```c
class PrimitiveAssembler {
```

<span id="f-1LGIz2">TextRenderer</span>[^](#1LGIz2) - "src/OrbitGl/TextRenderer.h" L31
```c
class TextRenderer {
```

<span id="f-zcsVb">TimeGraphLayout</span>[^](#zcsVb) - "src/OrbitGl/TimeGraphLayout.h" L15
```c
class TimeGraphLayout {
```

<span id="f-Z1T48Oo">TimelineInfoInterface</span>[^](#Z1T48Oo) - "src/OrbitGl/TimelineInfoInterface.h" L12
```c
class TimelineInfoInterface {
```

<span id="f-14nHAI">type</span>[^](#14nHAI) - "src/OrbitGl/PickingManager.h" L110
```c
  PickingType type;
```

<span id="f-Z23yq9H">Viewport</span>[^](#Z23yq9H) - "src/OrbitGl/Viewport.h" L31
```c
class Viewport {
```

<br/>

This file was generated by Swimm. [Click here to view it in the app](http://localhost:5000/repos/Z2l0aHViJTNBJTNBb3JiaXQlM0ElM0FBZGRpZUNvaGVu/docs/6sb8h).