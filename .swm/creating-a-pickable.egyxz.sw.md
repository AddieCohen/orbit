---
id: egyxz
name: Creating a Pickable
file_version: 1.0.2
app_version: 0.9.4-5
file_blobs:
  src/OrbitGl/TimeGraph.h: 6c6930cbcac7aa0c3a15ab693424581f8c7b69c7
  src/OrbitGl/TrackContainer.h: 3fd2504052ce1022ad1bedecb2ce764e92757cc2
  src/OrbitGl/TrackContainer.cpp: 1b1faf934019ed6db45408b0cf848fc63b5d028c
  src/OrbitGl/PickingManager.h: 9d8a448058ca78a1262a029eb1e9c2f68804940a
  src/OrbitGl/CaptureViewElement.h: 412f8063f976040de4c684657adf1cb88067e255
  src/OrbitGl/CallstackThreadBar.h: a110b63f9947cc6ad87efc65713bfe1707282383
  src/OrbitGl/TimelineUi.h: 5e559bc6d0f4f4b267b85939ee0d690536605bcd
  src/OrbitGl/ThreadStateBar.h: 11d30a02afcf50f1b584104bcb2ac7bd0f2bc930
  src/OrbitGl/ThreadBar.h: 6bb781e59876fe8a4f35fd692a53c61b00a8f737
---

Understanding Pickables, how they work, and how to add new ones, is important - and this document will describe just that.

A Pickable is {Explain what a Pickable is and its role in the system}

Some examples of `Pickable`[<sup id="1N2J29">↓</sup>](#f-1N2J29)s are `TrackContainer`[<sup id="Zah29l">↓</sup>](#f-Zah29l), `CallstackThreadBar`[<sup id="1boo5u">↓</sup>](#f-1boo5u), `TimelineUi`[<sup id="1UgGOW">↓</sup>](#f-1UgGOW), and `ThreadStateBar`[<sup id="1cQgRO">↓</sup>](#f-1cQgRO). Note: some of these examples inherit indirectly from `Pickable`[<sup id="1N2J29">↓</sup>](#f-1N2J29).

## Inherit from `CaptureViewElement`[<sup id="14fDYA">↓</sup>](#f-14fDYA) or `ThreadBar`[<sup id="1Y6Adb">↓</sup>](#f-1Y6Adb)? 🤔

Instances of `Pickable`[<sup id="1N2J29">↓</sup>](#f-1N2J29) usually don't inherit from it directly. Rather, they inherit from `ThreadBar`[<sup id="1Y6Adb">↓</sup>](#f-1Y6Adb) when {When to inherit from ThreadBar?} or from `CaptureViewElement`[<sup id="14fDYA">↓</sup>](#f-14fDYA) when {When to inherit from CaptureViewElement?}.

In this document we demonstrate inheriting from `CaptureViewElement`[<sup id="14fDYA">↓</sup>](#f-14fDYA) as it is more common.

## TL;DR - How to Add a `CaptureViewElement`[<sup id="14fDYA">↓</sup>](#f-14fDYA)

1.  Create a new class inheriting from `CaptureViewElement`[<sup id="14fDYA">↓</sup>](#f-14fDYA) 
    
    *   Place the file under `📄 src/OrbitGl`, e.g. `TrackContainer`[<sup id="Zah29l">↓</sup>](#f-Zah29l) is defined in `📄 src/OrbitGl/TrackContainer.h`.
        
2.  Implement `CreateAccessibleInterface`[<sup id="Z1W6FTF">↓</sup>](#f-Z1W6FTF) and `GetHeight`[<sup id="Z275aRz">↓</sup>](#f-Z275aRz).
    
3.  **Profit** 💰
    

## Example Walkthrough - `TrackContainer`[<sup id="Zah29l">↓</sup>](#f-Zah29l)

We'll follow the implementation of `TrackContainer`[<sup id="Zah29l">↓</sup>](#f-Zah29l) for this example.

A `TrackContainer`[<sup id="Zah29l">↓</sup>](#f-Zah29l) is {Explain what TrackContainer is and how it works with the Pickable interface}

<br/>

### `TrackContainer`[<sup id="Zah29l">↓</sup>](#f-Zah29l) Usage Example

For example, this is how `TrackContainer`[<sup id="Zah29l">↓</sup>](#f-Zah29l) can be used:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/TimeGraph.h
```c
⬜ 52       [[nodiscard]] const orbit_client_data::CaptureData* GetCaptureData() const {
⬜ 53         return capture_data_;
⬜ 54       }
🟩 55       [[nodiscard]] orbit_gl::TrackContainer* GetTrackContainer() const {
⬜ 56         return track_container_.get();
⬜ 57       }
⬜ 58       [[nodiscard]] orbit_gl::TimelineUi* GetTimelineUi() const { return timeline_ui_.get(); }
```

<br/>

## Steps to Adding a new `CaptureViewElement`[<sup id="14fDYA">↓</sup>](#f-14fDYA)

### 1\. Inherit from `CaptureViewElement`[<sup id="14fDYA">↓</sup>](#f-14fDYA).

All `CaptureViewElement`[<sup id="14fDYA">↓</sup>](#f-14fDYA)s are defined in files under `📄 src/OrbitGl`.

<br/>

We first need to define our class in the relevant file, and inherit from `CaptureViewElement`[<sup id="14fDYA">↓</sup>](#f-14fDYA):
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/TrackContainer.h
```c
⬜ 23     namespace orbit_gl {
⬜ 24     
⬜ 25     // Represent the space where Tracks will be drawn.
🟩 26     class TrackContainer final : public CaptureViewElement {
⬜ 27      public:
⬜ 28       explicit TrackContainer(CaptureViewElement* parent, TimelineInfoInterface* timeline_info,
⬜ 29                               Viewport* viewport, TimeGraphLayout* layout, OrbitApp* app,
```

<br/>

### 2\. Implement `CreateAccessibleInterface`[<sup id="Z1W6FTF">↓</sup>](#f-Z1W6FTF) and `GetHeight`[<sup id="Z275aRz">↓</sup>](#f-Z275aRz)

Here is how we do it for `TrackContainer`[<sup id="Zah29l">↓</sup>](#f-Zah29l):

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/TrackContainer.cpp
```c++
⬜ 411    }
⬜ 412    
⬜ 413    std::unique_ptr<orbit_accessibility::AccessibleInterface>
🟩 414    TrackContainer::CreateAccessibleInterface() {
🟩 415      return std::make_unique<AccessibleCaptureViewElement>(
🟩 416          this, "TrackContainer", orbit_accessibility::AccessibilityRole::Pane,
🟩 417          orbit_accessibility::AccessibilityState::Focusable);
🟩 418    }
⬜ 419    
⬜ 420    }  // namespace orbit_gl
⬜ 421    
```

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/TrackContainer.h
```c
⬜ 30                               const orbit_client_data::ModuleManager* module_manager,
⬜ 31                               orbit_client_data::CaptureData* capture_data);
⬜ 32     
🟩 33       [[nodiscard]] float GetHeight() const override { return height_; };
⬜ 34       void SetHeight(float height) { height_ = height; }
⬜ 35       [[nodiscard]] float GetVisibleTracksTotalHeight() const;
⬜ 36     
```

<br/>

<!-- THIS IS AN AUTOGENERATED SECTION. DO NOT EDIT THIS SECTION DIRECTLY -->
### Swimm Note

<span id="f-1boo5u">CallstackThreadBar</span>[^](#1boo5u) - "src/OrbitGl/CallstackThreadBar.h" L26
```c
class CallstackThreadBar : public ThreadBar {
```

<span id="f-14fDYA">CaptureViewElement</span>[^](#14fDYA) - "src/OrbitGl/CaptureViewElement.h" L26
```c
class CaptureViewElement : public Pickable, public AccessibleInterfaceProvider {
```

<span id="f-Z1W6FTF">CreateAccessibleInterface</span>[^](#Z1W6FTF) - "src/OrbitGl/TrackContainer.cpp" L414
```c++
TrackContainer::CreateAccessibleInterface() {
```

<span id="f-Z275aRz">GetHeight</span>[^](#Z275aRz) - "src/OrbitGl/TrackContainer.h" L33
```c
  [[nodiscard]] float GetHeight() const override { return height_; };
```

<span id="f-1N2J29">Pickable</span>[^](#1N2J29) - "src/OrbitGl/PickingManager.h" L26
```c
class Pickable {
```

<span id="f-1Y6Adb">ThreadBar</span>[^](#1Y6Adb) - "src/OrbitGl/ThreadBar.h" L22
```c
class ThreadBar : public CaptureViewElement, public std::enable_shared_from_this<ThreadBar> {
```

<span id="f-1cQgRO">ThreadStateBar</span>[^](#1cQgRO) - "src/OrbitGl/ThreadStateBar.h" L25
```c
class ThreadStateBar final : public ThreadBar {
```

<span id="f-1UgGOW">TimelineUi</span>[^](#1UgGOW) - "src/OrbitGl/TimelineUi.h" L15
```c
class TimelineUi : public CaptureViewElement {
```

<span id="f-Zah29l">TrackContainer</span>[^](#Zah29l) - "src/OrbitGl/TrackContainer.h" L26
```c
class TrackContainer final : public CaptureViewElement {
```

<br/>

This file was generated by Swimm. [Click here to view it in the app](http://localhost:5000/repos/Z2l0aHViJTNBJTNBb3JiaXQlM0ElM0FBZGRpZUNvaGVu/docs/egyxz).