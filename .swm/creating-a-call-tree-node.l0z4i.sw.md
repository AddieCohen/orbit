---
id: l0z4i
name: Creating a Call Tree Node
file_version: 1.0.2
app_version: 0.9.4-1
file_blobs:
  src/OrbitGl/CallTreeView.h: 0b9c0aa9ce8895844c9c25c0db22011962728b70
  src/OrbitGl/CallTreeView.cpp: a0c9bb9ef103d3f029e6b8df5160b0d008b8f361
  src/OrbitQt/CallTreeViewItemModel.cpp: 4552a18b43af00ac225f336bfbf532b95af015f1
---

In this document, we will learn how to add a new Call Tree Node to the system.

A Call Tree Node is {Explain what a Call Tree Node is and its role in the system}

Some examples of `CallTreeNode`[<sup id="2nY3OS">↓</sup>](#f-2nY3OS)s are `CallTreeView`[<sup id="aJPGT">↓</sup>](#f-aJPGT), `CallTreeFunction`[<sup id="z1u9v">↓</sup>](#f-z1u9v), `CallTreeUnwindErrorType`[<sup id="ZcdvR8">↓</sup>](#f-ZcdvR8), and `CallTreeThread`[<sup id="ZxsgRt">↓</sup>](#f-ZxsgRt).

## TL;DR - How to Add a `CallTreeNode`[<sup id="2nY3OS">↓</sup>](#f-2nY3OS)

1.  Create a new class inheriting from `CallTreeNode`[<sup id="2nY3OS">↓</sup>](#f-2nY3OS)  in `📄 src/OrbitGl/CallTreeView.h`.
    
2.  Update `📄 src/OrbitGl/CallTreeView.cpp`.
    
3.  Update `📄 src/OrbitGl/CallTreeView.h`.
    
4.  Update `📄 src/OrbitQt/CallTreeViewItemModel.cpp`.
    
5.  **Profit** 💰
    

## Example Walkthrough - `CallTreeFunction`[<sup id="z1u9v">↓</sup>](#f-z1u9v)

We'll follow the implementation of `CallTreeFunction`[<sup id="z1u9v">↓</sup>](#f-z1u9v) for this example.

A `CallTreeFunction`[<sup id="z1u9v">↓</sup>](#f-z1u9v) is {Explain what CallTreeFunction is and how it works with the Call Tree Node interface}

## Steps to Adding a new `CallTreeNode`[<sup id="2nY3OS">↓</sup>](#f-2nY3OS)

### 1\. Inherit from `CallTreeNode`[<sup id="2nY3OS">↓</sup>](#f-2nY3OS).

All `CallTreeNode`[<sup id="2nY3OS">↓</sup>](#f-2nY3OS)s are defined in `📄 src/OrbitGl/CallTreeView.h`.

<br/>

We first need to define our class in the relevant file, and inherit from `CallTreeNode`[<sup id="2nY3OS">↓</sup>](#f-2nY3OS):
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.h
```c
⬜ 115      mutable std::optional<std::vector<const CallTreeNode*>> children_cache_;
⬜ 116    };
⬜ 117    
🟩 118    class CallTreeFunction : public CallTreeNode {
⬜ 119     public:
⬜ 120      explicit CallTreeFunction(uint64_t function_absolute_address, std::string function_name,
⬜ 121                                std::string module_path, std::string module_build_id,
```

<br/>

> **Note**: the class name should start with "CallTree".

## Update additional files with the new class

Every time we add new `CallTreeNode`[<sup id="2nY3OS">↓</sup>](#f-2nY3OS)s, we reference them in a few locations.

We will still look at `CallTreeFunction`[<sup id="z1u9v">↓</sup>](#f-z1u9v) as our example.

<br/>

2\. Add the new class to `📄 src/OrbitGl/CallTreeView.cpp`, for instance:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.cpp
```c++
⬜ 62       return &it->second;
⬜ 63     }
⬜ 64     
🟩 65     CallTreeFunction* CallTreeNode::GetFunctionOrNull(uint64_t function_absolute_address) {
⬜ 66       auto function_it = function_children_.find(function_absolute_address);
⬜ 67       if (function_it == function_children_.end()) {
⬜ 68         return nullptr;
```

<br/>

In addition, in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.cpp
```c++
⬜ 70       return &function_it->second;
⬜ 71     }
⬜ 72     
🟩 73     CallTreeFunction* CallTreeNode::AddAndGetFunction(uint64_t function_absolute_address,
⬜ 74                                                       std::string function_name,
⬜ 75                                                       std::string module_path,
⬜ 76                                                       std::string module_build_id) {
⬜ 77       const auto& [it, inserted] = function_children_.try_emplace(
⬜ 78           function_absolute_address,
🟩 79           CallTreeFunction{function_absolute_address, std::move(function_name), std::move(module_path),
⬜ 80                            std::move(module_build_id), this});
⬜ 81       ORBIT_CHECK(inserted);
⬜ 82       children_cache_.reset();
```

<br/>

Also in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.cpp
```c++
⬜ 108      return unwind_errors_child_.get();
⬜ 109    }
⬜ 110    
🟩 111    [[nodiscard]] static CallTreeFunction* GetOrCreateFunctionNode(CallTreeNode* current_node,
⬜ 112                                                                   uint64_t frame,
⬜ 113                                                                   const std::string& function_name,
⬜ 114                                                                   const std::string& module_path,
⬜ 115                                                                   const std::string& module_build_id) {
🟩 116      CallTreeFunction* function_node = current_node->GetFunctionOrNull(frame);
⬜ 117      if (function_node == nullptr) {
⬜ 118        std::string formatted_function_name;
⬜ 119        if (function_name != orbit_client_data::kUnknownFunctionOrModuleName) {
```

<br/>

Still in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.cpp
```c++
⬜ 151        const auto& [module_path, module_build_id] =
⬜ 152            orbit_client_data::FindModulePathAndBuildIdByAddress(module_manager, capture_data, frame);
⬜ 153    
🟩 154        CallTreeFunction* function_node =
⬜ 155            GetOrCreateFunctionNode(current_thread_or_function, frame, function_name, module_path,
⬜ 156                                    module_build_id.value_or(""));
⬜ 157        function_node->IncreaseSampleCount(callstack_sample_count);
```

<br/>

Also notice in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.cpp
```c++
⬜ 183      const auto& [module_path, module_build_id] =
⬜ 184          orbit_client_data::FindModulePathAndBuildIdByAddress(module_manager, capture_data, frame);
⬜ 185    
🟩 186      CallTreeFunction* function_node = GetOrCreateFunctionNode(
⬜ 187          unwind_error_type_node, frame, function_name, module_path, module_build_id.value_or(""));
⬜ 188      function_node->IncreaseSampleCount(callstack_sample_count);
⬜ 189      function_node->AddExclusiveCallstackEvents(callstack_events);
```

<br/>

Additionally in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.cpp
```c++
⬜ 257        const auto& [module_path, module_build_id] =
⬜ 258            orbit_client_data::FindModulePathAndBuildIdByAddress(module_manager, capture_data, frame);
⬜ 259    
🟩 260        CallTreeFunction* function_node = GetOrCreateFunctionNode(
⬜ 261            current_node, frame, function_name, module_path, module_build_id.value_or(""));
⬜ 262        function_node->IncreaseSampleCount(callstack_sample_count);
⬜ 263        current_node = function_node;
```

<br/>

In addition, in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.cpp
```c++
⬜ 278          orbit_client_data::GetFunctionNameByAddress(module_manager, capture_data, frame);
⬜ 279      const auto& [module_path, module_build_id] =
⬜ 280          orbit_client_data::FindModulePathAndBuildIdByAddress(module_manager, capture_data, frame);
🟩 281      CallTreeFunction* function_node = GetOrCreateFunctionNode(
⬜ 282          bottom_up_view, frame, function_name, module_path, module_build_id.value_or(""));
⬜ 283      function_node->IncreaseSampleCount(callstack_sample_count);
⬜ 284    
```

<br/>

2\. Update `📄 src/OrbitGl/CallTreeView.h`, as we do with `CallTreeFunction`[<sup id="z1u9v">↓</sup>](#f-z1u9v) here:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.h
```c
⬜ 19     #include "ClientData/PostProcessedSamplingData.h"
⬜ 20     
⬜ 21     class CallTreeThread;
🟩 22     class CallTreeFunction;
⬜ 23     class CallTreeUnwindErrors;
⬜ 24     class CallTreeUnwindErrorType;
⬜ 25     
```

<br/>

Additionally in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.h
```c
⬜ 42     
⬜ 43       [[nodiscard]] CallTreeThread* AddAndGetThread(uint32_t thread_id, std::string thread_name);
⬜ 44     
🟩 45       [[nodiscard]] CallTreeFunction* GetFunctionOrNull(uint64_t function_absolute_address);
⬜ 46     
🟩 47       [[nodiscard]] CallTreeFunction* AddAndGetFunction(uint64_t function_absolute_address,
⬜ 48                                                         std::string function_name,
⬜ 49                                                         std::string module_path,
⬜ 50                                                         std::string module_build_id);
```

<br/>

Also notice in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.h
```c
⬜ 99       // absl::node_hash_map instead of absl::flat_hash_map as pointer stability is
⬜ 100      // needed for the CallTreeNode::parent_ field.
⬜ 101      absl::node_hash_map<uint32_t, CallTreeThread> thread_children_;
🟩 102      absl::node_hash_map<uint64_t, CallTreeFunction> function_children_;
⬜ 103      absl::node_hash_map<orbit_client_data::CallstackType, CallTreeUnwindErrorType>
⬜ 104          unwind_error_type_children_;
⬜ 105      // std::shared_ptr instead of std::unique_ptr because absl::node_hash_map
```

<br/>

Still in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/CallTreeView.h
```c
⬜ 115      mutable std::optional<std::vector<const CallTreeNode*>> children_cache_;
⬜ 116    };
⬜ 117    
🟩 118    class CallTreeFunction : public CallTreeNode {
⬜ 119     public:
🟩 120      explicit CallTreeFunction(uint64_t function_absolute_address, std::string function_name,
⬜ 121                                std::string module_path, std::string module_build_id,
⬜ 122                                CallTreeNode* parent)
⬜ 123          : CallTreeNode{parent},
```

<br/>

2\. Don't forget to add the new class to `📄 src/OrbitQt/CallTreeViewItemModel.cpp`, for example:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitQt/CallTreeViewItemModel.cpp
```c++
⬜ 27       ORBIT_CHECK(index.isValid());
⬜ 28       auto* item = static_cast<CallTreeNode*>(index.internalPointer());
⬜ 29       auto* thread_item = dynamic_cast<CallTreeThread*>(item);
🟩 30       auto* function_item = dynamic_cast<CallTreeFunction*>(item);
⬜ 31       auto* unwind_errors_item = dynamic_cast<CallTreeUnwindErrors*>(item);
⬜ 32       auto* unwind_error_type_item = dynamic_cast<CallTreeUnwindErrorType*>(item);
⬜ 33     
```

<br/>

Also in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitQt/CallTreeViewItemModel.cpp
```c++
⬜ 116      ORBIT_CHECK(index.isValid());
⬜ 117      auto* item = static_cast<CallTreeNode*>(index.internalPointer());
⬜ 118      auto* thread_item = dynamic_cast<CallTreeThread*>(item);
🟩 119      auto* function_item = dynamic_cast<CallTreeFunction*>(item);
⬜ 120      auto* unwind_errors_item = dynamic_cast<CallTreeUnwindErrors*>(item);
⬜ 121      auto* unwind_error_type_item = dynamic_cast<CallTreeUnwindErrorType*>(item);
⬜ 122    
```

<br/>

In addition, in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitQt/CallTreeViewItemModel.cpp
```c++
⬜ 173    QVariant CallTreeViewItemModel::GetToolTipRoleData(const QModelIndex& index) const {
⬜ 174      ORBIT_CHECK(index.isValid());
⬜ 175      auto* item = static_cast<CallTreeNode*>(index.internalPointer());
🟩 176      auto* function_item = dynamic_cast<CallTreeFunction*>(item);
⬜ 177      auto* unwind_error_type_item = dynamic_cast<CallTreeUnwindErrorType*>(item);
⬜ 178      if (function_item != nullptr) {
⬜ 179        switch (index.column()) {
```

<br/>

Still in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitQt/CallTreeViewItemModel.cpp
```c++
⬜ 216    QVariant CallTreeViewItemModel::GetModulePathRoleData(const QModelIndex& index) const {
⬜ 217      ORBIT_CHECK(index.isValid());
⬜ 218      auto* item = static_cast<CallTreeNode*>(index.internalPointer());
🟩 219      auto* function_item = dynamic_cast<CallTreeFunction*>(item);
⬜ 220      if (function_item != nullptr) {
⬜ 221        return QString::fromStdString(function_item->module_path());
⬜ 222      }
```

<br/>

Also notice in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitQt/CallTreeViewItemModel.cpp
```c++
⬜ 226    QVariant CallTreeViewItemModel::GetModuleBuildIdRoleData(const QModelIndex& index) const {
⬜ 227      ORBIT_CHECK(index.isValid());
⬜ 228      auto* item = static_cast<CallTreeNode*>(index.internalPointer());
🟩 229      auto* function_item = dynamic_cast<CallTreeFunction*>(item);
⬜ 230      if (function_item != nullptr) {
⬜ 231        return QString::fromStdString(function_item->module_build_id());
⬜ 232      }
```

<br/>

Also in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitQt/CallTreeViewItemModel.cpp
```c++
⬜ 239      ORBIT_CHECK(index.isValid());
⬜ 240      auto* item = static_cast<CallTreeNode*>(index.internalPointer());
⬜ 241      auto* thread_item = dynamic_cast<CallTreeThread*>(item);
🟩 242      auto* function_item = dynamic_cast<CallTreeFunction*>(item);
⬜ 243      auto* unwind_errors_item = dynamic_cast<CallTreeUnwindErrors*>(item);
⬜ 244    
⬜ 245      if (thread_item != nullptr) {
```

<br/>

<!-- THIS IS AN AUTOGENERATED SECTION. DO NOT EDIT THIS SECTION DIRECTLY -->
### Swimm Note

<span id="f-z1u9v">CallTreeFunction</span>[^](#z1u9v) - "src/OrbitGl/CallTreeView.h" L118
```c
class CallTreeFunction : public CallTreeNode {
```

<span id="f-2nY3OS">CallTreeNode</span>[^](#2nY3OS) - "src/OrbitGl/CallTreeView.h" L26
```c
class CallTreeNode {
```

<span id="f-ZxsgRt">CallTreeThread</span>[^](#ZxsgRt) - "src/OrbitGl/CallTreeView.h" L148
```c
class CallTreeThread : public CallTreeNode {
```

<span id="f-ZcdvR8">CallTreeUnwindErrorType</span>[^](#ZcdvR8) - "src/OrbitGl/CallTreeView.h" L167
```c
class CallTreeUnwindErrorType : public CallTreeNode {
```

<span id="f-aJPGT">CallTreeView</span>[^](#aJPGT) - "src/OrbitGl/CallTreeView.h" L181
```c
class CallTreeView : public CallTreeNode {
```

<br/>

This file was generated by Swimm. [Click here to view it in the app](http://localhost:5000/repos/Z2l0aHViJTNBJTNBb3JiaXQlM0ElM0FBZGRpZUNvaGVu/docs/l0z4i).