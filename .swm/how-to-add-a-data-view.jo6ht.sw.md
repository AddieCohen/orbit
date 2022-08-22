---
id: jo6ht
name: How to Add a Data View
file_version: 1.0.2
app_version: 0.9.4-5
file_blobs:
  src/DataViews/TracepointsDataView.cpp: 9c09a53a99456fa9d239c0bf1cf8f56fea37ff18
  src/DataViews/include/DataViews/TracepointsDataView.h: 61fcd32445d179fd3b16912948b0969d6d398ff9
  src/DataViews/CMakeLists.txt: 32fe29b71aea449759d53fac80149717d919037a
  src/DataViews/include/DataViews/AppInterface.h: 5425487cc319e9ad0e0dc4c8dcca52ea4e018021
  src/OrbitGl/App.h: b5986a599717130929b163457c738522e86b2767
  src/OrbitGl/App.cpp: 9a4e64dcb4e429ccdedefd834ef0c26452c65715
  src/DataViews/SamplingReportDataView.cpp: d455378cc3988cb8acec08cee08d698ae38b1373
  src/OrbitQt/orbitmainwindow.ui: a92cec9e46d9523239a8f8bfbacd06051c5b4b10
  src/DataViews/include/DataViews/DataView.h: 73449262dc7913c09bdb69deaaea6af5cb870880
  src/DataViews/include/DataViews/SamplingReportDataView.h: 6f8e4fc0027ec5c9f6ebcee384232d26a0824436
  src/DataViews/include/DataViews/FunctionsDataView.h: 2be24f8fdd5c3555a13d9526f33beba1d53f05d4
  src/DataViews/include/DataViews/CallstackDataView.h: 4008c7bb017d0f71b341d980cead5fb29305a4b1
  src/DataViews/include/DataViews/LiveFunctionsDataView.h: 60fb023d21cc997328627137322b48bc767fecb0
---

Understanding Data Views, how they work, and how to add new ones, is important - and this document will describe just that.

A Data View is {Explain what a Data View is and its role in the system}

Some examples of `DataView`[<sup id="Z15hSoa">↓</sup>](#f-Z15hSoa)s are `SamplingReportDataView`[<sup id="2jk0yw">↓</sup>](#f-2jk0yw), `FunctionsDataView`[<sup id="FCAUW">↓</sup>](#f-FCAUW), `CallstackDataView`[<sup id="ZsaX7z">↓</sup>](#f-ZsaX7z), and `LiveFunctionsDataView`[<sup id="ZFcOq9">↓</sup>](#f-ZFcOq9).

## TL;DR - How to Add a `DataView`[<sup id="Z15hSoa">↓</sup>](#f-Z15hSoa)

1.  Create a new class inheriting from `DataView`[<sup id="Z15hSoa">↓</sup>](#f-Z15hSoa) 
    
    *   Place the file under `📄 src/DataViews/include/DataViews`, e.g. `TracepointsDataView`[<sup id="Z1MKmYn">↓</sup>](#f-Z1MKmYn) is defined in `📄 src/DataViews/include/DataViews/TracepointsDataView.h`.
        
2.  Implement `DoFilter`[<sup id="2vCK68">↓</sup>](#f-2vCK68), `GetActionStatus`[<sup id="2hCoJQ">↓</sup>](#f-2hCoJQ), `GetColumns`[<sup id="2k7k1E">↓</sup>](#f-2k7k1E), `GetDefaultSortingColumn`[<sup id="ZoAmEM">↓</sup>](#f-ZoAmEM), `GetValue`[<sup id="Z1oF9Ys">↓</sup>](#f-Z1oF9Ys), and `DoSort`[<sup id="ZeMKyn">↓</sup>](#f-ZeMKyn).
    
3.  Update relevant files with the new class (see below).
    
4.  **Profit** 💰
    

## Example Walkthrough - `TracepointsDataView`[<sup id="Z1MKmYn">↓</sup>](#f-Z1MKmYn)

We'll follow the implementation of `TracepointsDataView`[<sup id="Z1MKmYn">↓</sup>](#f-Z1MKmYn) for this example.

A `TracepointsDataView`[<sup id="Z1MKmYn">↓</sup>](#f-Z1MKmYn) is {Explain what TracepointsDataView is and how it works with the Data View interface}

<br/>

### `TracepointsDataView`[<sup id="Z1MKmYn">↓</sup>](#f-Z1MKmYn) Usage Example

For example, this is how `TracepointsDataView`[<sup id="Z1MKmYn">↓</sup>](#f-Z1MKmYn) can be used:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/TracepointsDataView.cpp
```c++
⬜ 20     
⬜ 21     namespace orbit_data_views {
⬜ 22     
🟩 23     TracepointsDataView::TracepointsDataView(AppInterface* app,
⬜ 24                                              orbit_metrics_uploader::MetricsUploader* metrics_uploader)
⬜ 25         : DataView(DataViewType::kTracepoints, app, metrics_uploader) {}
⬜ 26     
```

<br/>

## Steps to Adding a new `DataView`[<sup id="Z15hSoa">↓</sup>](#f-Z15hSoa)

### 1\. Inherit from `DataView`[<sup id="Z15hSoa">↓</sup>](#f-Z15hSoa).

All `DataView`[<sup id="Z15hSoa">↓</sup>](#f-Z15hSoa)s are defined in files under `📄 src/DataViews/include/DataViews`.

<br/>

We first need to define our class in the relevant file, and inherit from `DataView`[<sup id="Z15hSoa">↓</sup>](#f-Z15hSoa):
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/include/DataViews/TracepointsDataView.h
```c
⬜ 17     
⬜ 18     namespace orbit_data_views {
⬜ 19     
🟩 20     class TracepointsDataView : public DataView {
⬜ 21      public:
⬜ 22       explicit TracepointsDataView(AppInterface* app,
⬜ 23                                    orbit_metrics_uploader::MetricsUploader* metrics_uploader);
```

<br/>

> **Note**: the class name should end with "DataView".

### 2\. Implement `DoFilter`[<sup id="2vCK68">↓</sup>](#f-2vCK68), `GetActionStatus`[<sup id="2hCoJQ">↓</sup>](#f-2hCoJQ), `GetColumns`[<sup id="2k7k1E">↓</sup>](#f-2k7k1E), `GetDefaultSortingColumn`[<sup id="ZoAmEM">↓</sup>](#f-ZoAmEM), `GetValue`[<sup id="Z1oF9Ys">↓</sup>](#f-Z1oF9Ys), and `DoSort`[<sup id="ZeMKyn">↓</sup>](#f-ZeMKyn)

Here is how we do it for `TracepointsDataView`[<sup id="Z1MKmYn">↓</sup>](#f-Z1MKmYn):

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/TracepointsDataView.cpp
```c++
⬜ 77       }
⬜ 78     }
⬜ 79     
🟩 80     void TracepointsDataView::DoFilter() {
🟩 81       std::vector<uint64_t> indices;
🟩 82       std::vector<std::string> tokens = absl::StrSplit(absl::AsciiStrToLower(filter_), ' ');
🟩 83     
🟩 84       for (size_t i = 0; i < tracepoints_.size(); ++i) {
🟩 85         const TracepointInfo& tracepoint = tracepoints_[i];
🟩 86         const std::string& tracepoint_string = tracepoint.name();
🟩 87     
🟩 88         bool match = true;
🟩 89     
🟩 90         for (std::string& filter_token : tokens) {
🟩 91           if (tracepoint_string.find(filter_token) == std::string::npos) {
🟩 92             match = false;
🟩 93             break;
🟩 94           }
🟩 95         }
🟩 96     
🟩 97         if (match) {
🟩 98           indices.emplace_back(i);
🟩 99         }
🟩 100      }
🟩 101    
🟩 102      indices_ = std::move(indices);
🟩 103    }
⬜ 104    
⬜ 105    DataView::ActionStatus TracepointsDataView::GetActionStatus(
⬜ 106        std::string_view action, int clicked_index, const std::vector<int>& selected_indices) {
```

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/TracepointsDataView.cpp
```c++
⬜ 102      indices_ = std::move(indices);
⬜ 103    }
⬜ 104    
🟩 105    DataView::ActionStatus TracepointsDataView::GetActionStatus(
🟩 106        std::string_view action, int clicked_index, const std::vector<int>& selected_indices) {
🟩 107      std::function<bool(const TracepointInfo&)> is_visible_action_enabled;
🟩 108      if (action == kMenuActionSelect) {
🟩 109        is_visible_action_enabled = [this](const TracepointInfo& tracepoint) {
🟩 110          return !app_->IsTracepointSelected(tracepoint);
🟩 111        };
🟩 112    
🟩 113      } else if (action == kMenuActionUnselect) {
🟩 114        is_visible_action_enabled = [this](const TracepointInfo& tracepoint) {
🟩 115          return app_->IsTracepointSelected(tracepoint);
🟩 116        };
🟩 117    
🟩 118      } else {
🟩 119        return DataView::GetActionStatus(action, clicked_index, selected_indices);
🟩 120      }
🟩 121    
🟩 122      for (int index : selected_indices) {
🟩 123        const TracepointInfo& tracepoint = GetTracepoint(index);
🟩 124        if (is_visible_action_enabled(tracepoint)) return ActionStatus::kVisibleAndEnabled;
🟩 125      }
🟩 126      return ActionStatus::kVisibleButDisabled;
🟩 127    }
⬜ 128    
⬜ 129    void TracepointsDataView::OnSelectRequested(const std::vector<int>& selection) {
⬜ 130      for (int i : selection) {
```

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/TracepointsDataView.cpp
```c++
⬜ 24                                              orbit_metrics_uploader::MetricsUploader* metrics_uploader)
⬜ 25         : DataView(DataViewType::kTracepoints, app, metrics_uploader) {}
⬜ 26     
🟩 27     const std::vector<DataView::Column>& TracepointsDataView::GetColumns() {
🟩 28       static const std::vector<Column>& columns = [] {
🟩 29         std::vector<Column> columns;
🟩 30         columns.resize(kNumColumns);
🟩 31         columns[kColumnSelected] = {"Selected", .0f, SortingOrder::kDescending};
🟩 32         columns[kColumnCategory] = {"Category", .5f, SortingOrder::kAscending};
🟩 33         columns[kColumnName] = {"Name", .2f, SortingOrder::kAscending};
🟩 34         return columns;
🟩 35       }();
🟩 36       return columns;
🟩 37     }
⬜ 38     
⬜ 39     std::string TracepointsDataView::GetValue(int row, int col) {
⬜ 40       const TracepointInfo& tracepoint = GetTracepoint(row);
```

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/include/DataViews/TracepointsDataView.h
```c
⬜ 23                                    orbit_metrics_uploader::MetricsUploader* metrics_uploader);
⬜ 24     
⬜ 25       const std::vector<Column>& GetColumns() override;
🟩 26       int GetDefaultSortingColumn() override { return kColumnCategory; }
⬜ 27       std::string GetValue(int row, int column) override;
⬜ 28     
⬜ 29       void OnSelectRequested(const std::vector<int>& selection) override;
```

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/TracepointsDataView.cpp
```c++
⬜ 36       return columns;
⬜ 37     }
⬜ 38     
🟩 39     std::string TracepointsDataView::GetValue(int row, int col) {
🟩 40       const TracepointInfo& tracepoint = GetTracepoint(row);
🟩 41     
🟩 42       switch (col) {
🟩 43         case kColumnSelected:
🟩 44           return app_->IsTracepointSelected(tracepoint) ? "X" : "-";
🟩 45         case kColumnName:
🟩 46           return tracepoint.name();
🟩 47         case kColumnCategory:
🟩 48           return tracepoint.category();
🟩 49         default:
🟩 50           return "";
🟩 51       }
🟩 52     }
⬜ 53     
⬜ 54     #define ORBIT_PROC_SORT(Member)                                     \
⬜ 55       [&](int a, int b) {                                               \
```

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/TracepointsDataView.cpp
```c++
⬜ 57             tracepoints_[a].Member, tracepoints_[b].Member, ascending); \
⬜ 58       }
⬜ 59     
🟩 60     void TracepointsDataView::DoSort() {
🟩 61       bool ascending = sorting_orders_[sorting_column_] == SortingOrder::kAscending;
🟩 62       std::function<bool(int a, int b)> sorter = nullptr;
🟩 63     
🟩 64       switch (sorting_column_) {
🟩 65         case kColumnName:
🟩 66           sorter = ORBIT_PROC_SORT(name());
🟩 67           break;
🟩 68         case kColumnCategory:
🟩 69           sorter = ORBIT_PROC_SORT(category());
🟩 70           break;
🟩 71         default:
🟩 72           break;
🟩 73       }
🟩 74     
🟩 75       if (sorter) {
🟩 76         std::stable_sort(indices_.begin(), indices_.end(), sorter);
🟩 77       }
🟩 78     }
⬜ 79     
⬜ 80     void TracepointsDataView::DoFilter() {
⬜ 81       std::vector<uint64_t> indices;
```

<br/>

## Update additional files with the new class

Every time we add new `DataView`[<sup id="Z15hSoa">↓</sup>](#f-Z15hSoa)s, we reference them in a few locations.

We will still look at `TracepointsDataView`[<sup id="Z1MKmYn">↓</sup>](#f-Z1MKmYn) as our example.

<br/>

3\. We modify `📄 src/DataViews/CMakeLists.txt`, like so:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/CMakeLists.txt
```text
⬜ 16             ModulesDataView.cpp
⬜ 17             PresetsDataView.cpp
⬜ 18             SamplingReportDataView.cpp
🟩 19             TracepointsDataView.cpp)
⬜ 20     
⬜ 21     target_sources(DataViews PUBLIC
⬜ 22             include/DataViews/AppInterface.h
```

<br/>

Also in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/CMakeLists.txt
```text
⬜ 32             include/DataViews/PresetLoadState.h
⬜ 33             include/DataViews/SamplingReportDataView.h        
⬜ 34             include/DataViews/SamplingReportInterface.h
🟩 35             include/DataViews/TracepointsDataView.h)
⬜ 36     
⬜ 37     target_include_directories(DataViews PUBLIC include/)
⬜ 38     target_link_libraries(DataViews PUBLIC
```

<br/>

3\. Add the new class to `📄 src/DataViews/include/DataViews/AppInterface.h`, as seen here:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/include/DataViews/AppInterface.h
```c
⬜ 82           std::vector<const orbit_client_data::ModuleData*> modules_to_validate) = 0;
⬜ 83       virtual void UpdateProcessAndModuleList() = 0;
⬜ 84     
🟩 85       // Functions needed by TracepointsDataView
⬜ 86       virtual void SelectTracepoint(const orbit_grpc_protos::TracepointInfo& info) = 0;
⬜ 87       virtual void DeselectTracepoint(const orbit_grpc_protos::TracepointInfo& tracepoint) = 0;
⬜ 88       [[nodiscard]] virtual bool IsTracepointSelected(
```

<br/>

3\. Don't forget to add the new class to `📄 src/OrbitGl/App.h`, for instance:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/App.h
```c
⬜ 57     #include "DataViews/ModulesDataView.h"
⬜ 58     #include "DataViews/PresetLoadState.h"
⬜ 59     #include "DataViews/PresetsDataView.h"
🟩 60     #include "DataViews/TracepointsDataView.h"
⬜ 61     #include "FramePointerValidatorClient.h"
⬜ 62     #include "FrameTrackOnlineProcessor.h"
⬜ 63     #include "GlCanvas.h"
```

<br/>

Still in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/App.h
```c
⬜ 618      std::unique_ptr<orbit_data_views::CallstackDataView> callstack_data_view_;
⬜ 619      std::unique_ptr<orbit_data_views::CallstackDataView> selection_callstack_data_view_;
⬜ 620      std::unique_ptr<orbit_data_views::PresetsDataView> presets_data_view_;
🟩 621      std::unique_ptr<orbit_data_views::TracepointsDataView> tracepoints_data_view_;
⬜ 622    
⬜ 623      CaptureWindow* capture_window_ = nullptr;
⬜ 624      IntrospectionWindow* introspection_window_ = nullptr;
```

<br/>

3\. Update `📄 src/OrbitGl/App.cpp`, as we do with `TracepointsDataView`[<sup id="Z1MKmYn">↓</sup>](#f-Z1MKmYn) here:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/App.cpp
```c++
⬜ 128    using orbit_data_views::FunctionsDataView;
⬜ 129    using orbit_data_views::ModulesDataView;
⬜ 130    using orbit_data_views::PresetsDataView;
🟩 131    using orbit_data_views::TracepointsDataView;
⬜ 132    
⬜ 133    using orbit_gl::MainWindowInterface;
⬜ 134    
```

<br/>

Also notice in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGl/App.cpp
```c++
⬜ 2630       case DataViewType::kTracepoints:
⬜ 2631         if (!tracepoints_data_view_) {
⬜ 2632           tracepoints_data_view_ =
🟩 2633               DataView::CreateAndInit<TracepointsDataView>(this, metrics_uploader_);
⬜ 2634           panels_.push_back(tracepoints_data_view_.get());
⬜ 2635         }
⬜ 2636         return tracepoints_data_view_.get();
```

<br/>

## Optionally, these snippets may be helpful

<br/>

Update `📄 src/DataViews/SamplingReportDataView.cpp`
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/SamplingReportDataView.cpp
```c++
⬜ 26     #include "ClientData/ProcessData.h"
⬜ 27     #include "DataViews/CompareAscendingOrDescending.h"
⬜ 28     #include "DataViews/DataViewType.h"
🟩 29     #include "DataViews/FunctionsDataView.h"
⬜ 30     #include "OrbitBase/Append.h"
⬜ 31     #include "OrbitBase/Logging.h"
⬜ 32     #include "OrbitBase/Result.h"
```

<br/>

Update `📄 src/DataViews/SamplingReportDataView.cpp`
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/SamplingReportDataView.cpp
```c++
⬜ 78     
⬜ 79       switch (column) {
⬜ 80         case kColumnSelected:
🟩 81           return app_->IsFunctionSelected(func) ? FunctionsDataView::kSelectedFunctionString
⬜ 82                                                 : FunctionsDataView::kUnselectedFunctionString;
⬜ 83         case kColumnFunctionName:
⬜ 84           return func.name;
```

<br/>

Update `📄 src/DataViews/SamplingReportDataView.cpp`
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/DataViews/SamplingReportDataView.cpp
```c++
⬜ 79       switch (column) {
⬜ 80         case kColumnSelected:
⬜ 81           return app_->IsFunctionSelected(func) ? FunctionsDataView::kSelectedFunctionString
🟩 82                                                 : FunctionsDataView::kUnselectedFunctionString;
⬜ 83         case kColumnFunctionName:
⬜ 84           return func.name;
⬜ 85         case kColumnInclusive:
```

<br/>

Update `📄 src/OrbitQt/orbitmainwindow.ui`
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitQt/orbitmainwindow.ui
```xml
⬜ 237                </widget>
⬜ 238                <widget class="OrbitDataViewPanel" name="FunctionsList" native="true">
⬜ 239                 <property name="accessibleName">
🟩 240                  <string>FunctionsDataView</string>
⬜ 241                 </property>
⬜ 242                </widget>
⬜ 243               </widget>
```

<br/>

<!-- THIS IS AN AUTOGENERATED SECTION. DO NOT EDIT THIS SECTION DIRECTLY -->
### Swimm Note

<span id="f-ZsaX7z">CallstackDataView</span>[^](#ZsaX7z) - "src/DataViews/include/DataViews/CallstackDataView.h" L23
```c
class CallstackDataView : public DataView {
```

<span id="f-Z15hSoa">DataView</span>[^](#Z15hSoa) - "src/DataViews/include/DataViews/DataView.h" L67
```c
class DataView {
```

<span id="f-2vCK68">DoFilter</span>[^](#2vCK68) - "src/DataViews/TracepointsDataView.cpp" L80
```c++
void TracepointsDataView::DoFilter() {
```

<span id="f-ZeMKyn">DoSort</span>[^](#ZeMKyn) - "src/DataViews/TracepointsDataView.cpp" L60
```c++
void TracepointsDataView::DoSort() {
```

<span id="f-FCAUW">FunctionsDataView</span>[^](#FCAUW) - "src/DataViews/include/DataViews/FunctionsDataView.h" L16
```c
class FunctionsDataView : public DataView {
```

<span id="f-2hCoJQ">GetActionStatus</span>[^](#2hCoJQ) - "src/DataViews/TracepointsDataView.cpp" L105
```c++
DataView::ActionStatus TracepointsDataView::GetActionStatus(
```

<span id="f-2k7k1E">GetColumns</span>[^](#2k7k1E) - "src/DataViews/TracepointsDataView.cpp" L27
```c++
const std::vector<DataView::Column>& TracepointsDataView::GetColumns() {
```

<span id="f-ZoAmEM">GetDefaultSortingColumn</span>[^](#ZoAmEM) - "src/DataViews/include/DataViews/TracepointsDataView.h" L26
```c
  int GetDefaultSortingColumn() override { return kColumnCategory; }
```

<span id="f-Z1oF9Ys">GetValue</span>[^](#Z1oF9Ys) - "src/DataViews/TracepointsDataView.cpp" L39
```c++
std::string TracepointsDataView::GetValue(int row, int col) {
```

<span id="f-ZFcOq9">LiveFunctionsDataView</span>[^](#ZFcOq9) - "src/DataViews/include/DataViews/LiveFunctionsDataView.h" L26
```c
class LiveFunctionsDataView : public DataView {
```

<span id="f-2jk0yw">SamplingReportDataView</span>[^](#2jk0yw) - "src/DataViews/include/DataViews/SamplingReportDataView.h" L27
```c
class SamplingReportDataView : public DataView {
```

<span id="f-Z1MKmYn">TracepointsDataView</span>[^](#Z1MKmYn) - "src/DataViews/include/DataViews/TracepointsDataView.h" L20
```c
class TracepointsDataView : public DataView {
```

<br/>

This file was generated by Swimm. [Click here to view it in the app](http://localhost:5000/repos/Z2l0aHViJTNBJTNBb3JiaXQlM0ElM0FBZGRpZUNvaGVu/docs/jo6ht).