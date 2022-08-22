---
id: r5r02
name: Creating a Linux Tracing in Linuxtracing
file_version: 1.0.2
app_version: 0.9.4-5
file_blobs:
  src/LinuxTracing/include/LinuxTracing/Tracer.h: 02913a00b84d32758c7a3991f3be7398c9dc3c97
  src/LinuxTracing/Tracer.cpp: c0eba026c989cfb180a8b0a0ce045123901521b9
  src/LinuxTracing/TracerImpl.cpp: 0d9641e464f50d57d2caa70f51406cb3a465de5a
  src/LinuxTracing/TracerImpl.h: af72d59ebb25a60d5af341cbe15a981be3a9d184
---

Understanding Linux Tracings, how they work, and how to add new ones, is important - and this document will describe just that.

Some examples of `📄 src/LinuxTracing/include/LinuxTracing`s are `📄 src/LinuxTracing/include/LinuxTracing/Tracer.h`, `📄 src/LinuxTracing/include/LinuxTracing/TracerListener.h`, and `📄 src/LinuxTracing/include/LinuxTracing/UserSpaceInstrumentationAddresses.h`.

## TL;DR - How to add `📄 src/LinuxTracing/include/LinuxTracing`s

1.  Create a new file under `📄 src/LinuxTracing/include/LinuxTracing` 
    
    *   e.g. `📄 src/LinuxTracing/include/LinuxTracing/Tracer.h`
        
2.  Implement the Linux Tracing class (e.g. `Tracer`[<sup id="Z2IMlh">↓</sup>](#f-Z2IMlh))
    
    1.  Update `📄 src/LinuxTracing/Tracer.cpp` with the new class
        
    2.  Update `📄 src/LinuxTracing/TracerImpl.cpp` with the new class
        
    3.  Update `📄 src/LinuxTracing/TracerImpl.h` with the new class
        
    4.  Update `📄 src/LinuxTracing/include/LinuxTracing/Tracer.h` with the new class
        
3.  **Profit** 💰
    

# Full walkthrough

Start by creating a new file under `📄 src/LinuxTracing/include/LinuxTracing`. We'll follow `📄 src/LinuxTracing/include/LinuxTracing/Tracer.h` as an example.

## Implement the Linux Tracing class

Here is how we do it for `📄 src/LinuxTracing/include/LinuxTracing/Tracer.h`:

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/LinuxTracing/include/LinuxTracing/Tracer.h
```c
⬜ 16     
⬜ 17     namespace orbit_linux_tracing {
⬜ 18     
🟩 19     class Tracer {
🟩 20      public:
🟩 21       virtual void Start() = 0;
🟩 22       virtual void Stop() = 0;
🟩 23     
🟩 24       // The `FunctionEntry` and `FunctionExit` events are received from user space instrumentation and
🟩 25       // piped back into `LinuxTracing` using these two methods. This way they can be processed together
🟩 26       // with stack samples, allowing us to correctly unwind the samples that fall inside one or more
🟩 27       // dynamically instrumented functions, just like we do with u(ret)probes.
🟩 28       virtual void ProcessFunctionEntry(const orbit_grpc_protos::FunctionEntry& function_entry) = 0;
🟩 29       virtual void ProcessFunctionExit(const orbit_grpc_protos::FunctionExit& function_exit) = 0;
🟩 30     
🟩 31       virtual ~Tracer() = default;
🟩 32     
🟩 33       [[nodiscard]] static std::unique_ptr<Tracer> Create(
🟩 34           const orbit_grpc_protos::CaptureOptions& capture_options,
🟩 35           std::unique_ptr<UserSpaceInstrumentationAddresses> user_space_instrumentation_addresses,
🟩 36           TracerListener* listener);
🟩 37     };
⬜ 38     
⬜ 39     }  // namespace orbit_linux_tracing
⬜ 40     
```

<br/>

### Update additional files with the new class

Every time we add new `📄 src/LinuxTracing/include/LinuxTracing`s, we reference them in a few locations.

We will still look at `📄 src/LinuxTracing/include/LinuxTracing/Tracer.h` as our example.

<br/>

Update `📄 src/LinuxTracing/Tracer.cpp`, like so:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/LinuxTracing/Tracer.cpp
```c++
⬜ 11     
⬜ 12     namespace orbit_linux_tracing {
⬜ 13     
🟩 14     std::unique_ptr<Tracer> Tracer::Create(
⬜ 15         const orbit_grpc_protos::CaptureOptions& capture_options,
⬜ 16         std::unique_ptr<UserSpaceInstrumentationAddresses> user_space_instrumentation_addresses,
⬜ 17         TracerListener* listener) {
```

<br/>

Add the new class to `📄 src/LinuxTracing/TracerImpl.cpp`, for example:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/LinuxTracing/TracerImpl.cpp
```c++
⬜ 841    }
⬜ 842    
⬜ 843    void TracerImpl::Run() {
🟩 844      orbit_base::SetCurrentThreadName("Tracer::Run");
⬜ 845    
⬜ 846      Startup();
⬜ 847    
```

<br/>

Don't forget to add the new class to `📄 src/LinuxTracing/TracerImpl.h`, for instance:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/LinuxTracing/TracerImpl.h
```c
⬜ 43     
⬜ 44     namespace orbit_linux_tracing {
⬜ 45     
🟩 46     class TracerImpl : public Tracer {
⬜ 47      public:
⬜ 48       explicit TracerImpl(
⬜ 49           const orbit_grpc_protos::CaptureOptions& capture_options,
```

<br/>

We modify `📄 src/LinuxTracing/include/LinuxTracing/Tracer.h`, as seen here:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/LinuxTracing/include/LinuxTracing/Tracer.h
```c
⬜ 16     
⬜ 17     namespace orbit_linux_tracing {
⬜ 18     
🟩 19     class Tracer {
⬜ 20      public:
⬜ 21       virtual void Start() = 0;
⬜ 22       virtual void Stop() = 0;
```

<br/>

Also in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/LinuxTracing/include/LinuxTracing/Tracer.h
```c
⬜ 28       virtual void ProcessFunctionEntry(const orbit_grpc_protos::FunctionEntry& function_entry) = 0;
⬜ 29       virtual void ProcessFunctionExit(const orbit_grpc_protos::FunctionExit& function_exit) = 0;
⬜ 30     
🟩 31       virtual ~Tracer() = default;
⬜ 32     
🟩 33       [[nodiscard]] static std::unique_ptr<Tracer> Create(
⬜ 34           const orbit_grpc_protos::CaptureOptions& capture_options,
⬜ 35           std::unique_ptr<UserSpaceInstrumentationAddresses> user_space_instrumentation_addresses,
⬜ 36           TracerListener* listener);
```

<br/>

<!-- THIS IS AN AUTOGENERATED SECTION. DO NOT EDIT THIS SECTION DIRECTLY -->
### Swimm Note

<span id="f-Z2IMlh">Tracer</span>[^](#Z2IMlh) - "src/LinuxTracing/include/LinuxTracing/Tracer.h" L19
```c
class Tracer {
```

<br/>

This file was generated by Swimm. [Click here to view it in the app](http://localhost:5000/repos/Z2l0aHViJTNBJTNBb3JiaXQlM0ElM0FBZGRpZUNvaGVu/docs/r5r02).