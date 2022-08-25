---
id: 677dq
name: How to Add an Orbit Ssh Qt in Orbitsshqt
file_version: 1.0.2
app_version: 0.9.4-6
file_blobs:
  src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h: e6f8310854c9d2ad7392ac56b83fd5aa2f65664b
  src/OrbitSshQt/include/OrbitSshQt/SftpCopyToLocalOperation.h: 135e261d3585ea28f07c6dee87d20156ee911de4
  src/OrbitSshQt/include/OrbitSshQt/SftpCopyToRemoteOperation.h: 7690e675235ad9f3309fb7d1c3abd5251529fe91
  src/SessionSetup/ServiceDeployManager.cpp: 6403c2b9b41471be12ef973477ef51e562822e06
  src/OrbitSshQt/CMakeLists.txt: 1fab18063600c3da21dd0f6177c25a4ff685c6a1
  src/OrbitSshQt/IntegrationTests.cpp: 6f15a0c6f70b80dc7274072e1d152eb38256d2b0
  src/OrbitSshQt/include/OrbitSshQt/StateMachineHelper.h: c8f0bab3bcd6be5b023da417a21a563b8f42a709
---

In this document, we will learn how to add a new Orbit Ssh Qt to the system.

Some examples of `📄 src/OrbitSshQt/include/OrbitSshQt`s are `📄 src/OrbitSshQt/include/OrbitSshQt/ScopedConnection.h`, `📄 src/OrbitSshQt/include/OrbitSshQt/Session.h`, and `📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h`.

## TL;DR - How to add `📄 src/OrbitSshQt/include/OrbitSshQt`s

1.  Create a new file under `📄 src/OrbitSshQt/include/OrbitSshQt` 
    
    *   e.g. `📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h`
        
2.  Create `<OrbitSshQtName>State` (e.g. `SftpChannelState`[<sup id="6LLyq">↓</sup>](#f-6LLyq))
    
3.  Implement the Orbit Ssh Qt class (e.g. `SftpChannel`[<sup id="1ViGGG">↓</sup>](#f-1ViGGG))
    
    1.  Inherit from `StateMachineHelper`[<sup id="2eKR5k">↓</sup>](#f-2eKR5k)
        
    2.  Update `📄 src/OrbitSshQt/include/OrbitSshQt/SftpCopyToLocalOperation.h` with the new class
        
    3.  Update `📄 src/OrbitSshQt/include/OrbitSshQt/SftpCopyToRemoteOperation.h` with the new class
        
    4.  Update `📄 src/SessionSetup/ServiceDeployManager.cpp` with the new class
        
    5.  Update `📄 src/OrbitSshQt/CMakeLists.txt` with the new class
        
    6.  Update `📄 src/OrbitSshQt/IntegrationTests.cpp` with the new class
        
    7.  Update `📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h` with the new class
        
4.  **Profit** 💰
    

# Full walkthrough

Start by creating a new file under `📄 src/OrbitSshQt/include/OrbitSshQt`. We'll follow `📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h` as an example.




## Implement `<OrbitSshQtName>State`

Here is how we do it for `📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h`:

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h
```c
⬜ 20     
⬜ 21     namespace orbit_ssh_qt {
⬜ 22     namespace details {
🟩 23     enum class SftpChannelState {
🟩 24       kInitial,
🟩 25       kNoChannel,
🟩 26       kStarted,
🟩 27       kChannelInitialized,
🟩 28       kShutdown,
🟩 29       kClosingChannel,
🟩 30       kDone,
🟩 31       kError
🟩 32     };
⬜ 33     }  // namespace details
⬜ 34     
⬜ 35     /*
```

<br/>

## Implement the Orbit Ssh Qt class

Here is how we do it for `📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h`:

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h
```c
⬜ 41       SftpCopyToRemoteOperation. The user needs to keep the channel alive as long as
⬜ 42       SftpOperations are still running.
⬜ 43     */
🟩 44     class SftpChannel : public StateMachineHelper<SftpChannel, details::SftpChannelState> {
🟩 45       Q_OBJECT
🟩 46       friend StateMachineHelper;
🟩 47     
🟩 48      public:
🟩 49       explicit SftpChannel(Session* session);
🟩 50     
🟩 51       void Start();
🟩 52       void Stop();
🟩 53     
🟩 54       orbit_ssh::Sftp* GetRawSftp() { return sftp_ ? &sftp_.value() : nullptr; }
🟩 55     
🟩 56      signals:
🟩 57       void aboutToShutdown();
🟩 58       void started();
🟩 59       void stopped();
🟩 60       void dataEvent();
🟩 61       void errorOccurred(std::error_code);
🟩 62     
🟩 63      private:
🟩 64       QPointer<Session> session_;
🟩 65       std::optional<orbit_ssh::Sftp> sftp_;
🟩 66     
🟩 67       std::optional<ScopedConnection> data_event_connection_;
🟩 68       std::optional<ScopedConnection> about_to_shutdown_connection_;
🟩 69     
🟩 70       void HandleSessionShutdown();
🟩 71       void HandleEagain();
🟩 72     
🟩 73       using StateMachineHelper::SetError;
🟩 74       void SetError(std::error_code);
🟩 75     
🟩 76       outcome::result<void> startup();
🟩 77       outcome::result<void> shutdown();
🟩 78       outcome::result<void> run();
🟩 79     };
⬜ 80     
⬜ 81     }  // namespace orbit_ssh_qt
⬜ 82     
```

<br/>

### Update additional files with the new class

Every time we add new `📄 src/OrbitSshQt/include/OrbitSshQt`s, we reference them in a few locations.

We will still look at `📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h` as our example.

<br/>

Don't forget to add the new class to `📄 src/OrbitSshQt/include/OrbitSshQt/SftpCopyToLocalOperation.h`, like so:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/include/OrbitSshQt/SftpCopyToLocalOperation.h
```c
⬜ 38     
⬜ 39     /*
⬜ 40       SftpCopyToRemoteOperation represents a file operation in the SSH-SFTP
🟩 41       subsystem. It needs an established SftpChannel for operation.
⬜ 42     
⬜ 43       This operation implements remote -> local copying.
⬜ 44     */
```

<br/>

In addition, in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/include/OrbitSshQt/SftpCopyToLocalOperation.h
```c
⬜ 48       friend StateMachineHelper;
⬜ 49     
⬜ 50      public:
🟩 51       explicit SftpCopyToLocalOperation(Session* session, SftpChannel* channel);
⬜ 52     
⬜ 53       void CopyFileToLocal(std::filesystem::path source, std::filesystem::path destination);
⬜ 54     
```

<br/>

Also notice in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/include/OrbitSshQt/SftpCopyToLocalOperation.h
```c
⬜ 64       std::optional<ScopedConnection> data_event_connection_;
⬜ 65       std::optional<ScopedConnection> about_to_shutdown_connection_;
⬜ 66     
🟩 67       QPointer<SftpChannel> channel_;
⬜ 68       std::optional<orbit_ssh::SftpFile> sftp_file_;
⬜ 69       QFile local_file_;
⬜ 70     
```

<br/>

We modify `📄 src/OrbitSshQt/include/OrbitSshQt/SftpCopyToRemoteOperation.h`, as seen here:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/include/OrbitSshQt/SftpCopyToRemoteOperation.h
```c
⬜ 39     
⬜ 40     /*
⬜ 41       SftpCopyToRemoteOperation represents a file operation in the SSH-SFTP
🟩 42       subsystem. It needs an established SftpChannel for operation.
⬜ 43     
⬜ 44       This operation implements local -> remote copying, for remote to local use
⬜ 45       SftpCopyToLocalOperation.
```

<br/>

Additionally in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/include/OrbitSshQt/SftpCopyToRemoteOperation.h
```c
⬜ 53      public:
⬜ 54       enum class FileMode : int { kUserWritable = 0644, kUserWritableAllExecutable = 0755 };
⬜ 55     
🟩 56       explicit SftpCopyToRemoteOperation(Session* session, SftpChannel* channel);
⬜ 57     
⬜ 58       void CopyFileToRemote(std::filesystem::path source, std::filesystem::path destination,
⬜ 59                             FileMode destination_mode);
```

<br/>

Still in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/include/OrbitSshQt/SftpCopyToRemoteOperation.h
```c
⬜ 70       std::optional<ScopedConnection> data_event_connection_;
⬜ 71       std::optional<ScopedConnection> about_to_shutdown_connection_;
⬜ 72     
🟩 73       QPointer<SftpChannel> channel_;
⬜ 74       std::optional<orbit_ssh::SftpFile> sftp_file_;
⬜ 75       QFile local_file_;
⬜ 76       QByteArray write_buffer_;
```

<br/>

Update `📄 src/SessionSetup/ServiceDeployManager.cpp`, for example:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/SessionSetup/ServiceDeployManager.cpp
```c++
⬜ 234      return outcome::success(tunnel->value().GetListenPort());
⬜ 235    }
⬜ 236    
🟩 237    ErrorMessageOr<std::unique_ptr<orbit_ssh_qt::SftpChannel>>
⬜ 238    ServiceDeployManager::StartSftpChannel() {
⬜ 239      ORBIT_CHECK(QThread::currentThread() == thread());
🟩 240      auto sftp_channel = std::make_unique<orbit_ssh_qt::SftpChannel>(&session_.value());
⬜ 241    
⬜ 242      orbit_qt_utils::EventLoop loop{};
⬜ 243      auto quit_handler =
🟩 244          ConnectQuitHandler(&loop, sftp_channel.get(), &orbit_ssh_qt::SftpChannel::started);
⬜ 245    
⬜ 246      auto error_handler =
🟩 247          ConnectErrorHandler(&loop, sftp_channel.get(), &orbit_ssh_qt::SftpChannel::errorOccurred);
⬜ 248      auto cancel_handler = ConnectCancelHandler(&loop, this);
⬜ 249    
⬜ 250      sftp_channel->Start();
```

<br/>

Also in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/SessionSetup/ServiceDeployManager.cpp
```c++
⬜ 277    }
⬜ 278    
⬜ 279    ErrorMessageOr<void> ServiceDeployManager::ShutdownSftpChannel(
🟩 280        orbit_ssh_qt::SftpChannel* sftp_channel) {
⬜ 281      ORBIT_SCOPED_TIMED_LOG("ServiceDeployManager::ShutdownSftpChannel");
⬜ 282      ORBIT_CHECK(QThread::currentThread() == thread());
⬜ 283      ORBIT_CHECK(sftp_channel != nullptr);
⬜ 284    
⬜ 285      orbit_qt_utils::EventLoop loop{};
🟩 286      auto quit_handler = ConnectQuitHandler(&loop, sftp_channel, &orbit_ssh_qt::SftpChannel::stopped);
⬜ 287      auto error_handler =
🟩 288          ConnectErrorHandler(&loop, sftp_channel, &orbit_ssh_qt::SftpChannel::errorOccurred);
⬜ 289      auto cancel_handler = ConnectCancelHandler(&loop, this);
⬜ 290    
⬜ 291      sftp_channel->Stop();
```

<br/>

Add the new class to `📄 src/OrbitSshQt/CMakeLists.txt`, as we do with `📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h` here:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/CMakeLists.txt
```text
⬜ 28               Session.cpp
⬜ 29               Tunnel.cpp
⬜ 30               Task.cpp
🟩 31               SftpChannel.cpp
⬜ 32               SftpCopyToLocalOperation.cpp
⬜ 33               SftpCopyToRemoteOperation.cpp)
⬜ 34     
```

<br/>

Add the new class to `📄 src/OrbitSshQt/IntegrationTests.cpp`, for instance:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/IntegrationTests.cpp
```c++
⬜ 59       std::string data_sink_reverse;
⬜ 60       std::string greetings = "Hello World! I'm here!";
⬜ 61       std::string_view write_buffer{greetings};
🟩 62       orbit_ssh_qt::SftpChannel sftp_channel{&session};
⬜ 63       orbit_ssh_qt::SftpCopyToRemoteOperation sftp_op{&session, &sftp_channel};
⬜ 64       QEventLoop loop{};
⬜ 65     
```

<br/>

In addition, in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/IntegrationTests.cpp
```c++
⬜ 164      // SFTP Channel
⬜ 165      std::optional<QTemporaryFile> temp_file;
⬜ 166    
🟩 167      QObject::connect(&sftp_channel, &orbit_ssh_qt::SftpChannel::started, &loop, [&]() {
⬜ 168        temp_file.emplace();
⬜ 169        temp_file->setAutoRemove(true);
⬜ 170        ASSERT_TRUE(temp_file->open());
```

<br/>

Additionally in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/IntegrationTests.cpp
```c++
⬜ 177        CheckCheckpoint(Checkpoint::kSftpChannelStarted);
⬜ 178      });
⬜ 179    
🟩 180      QObject::connect(&sftp_channel, &orbit_ssh_qt::SftpChannel::errorOccurred, &loop,
⬜ 181                       [&](std::error_code e) {
⬜ 182                         loop.quit();
⬜ 183                         FAIL() << absl::StrFormat("SFTP channel error occurred: %s", e.message());
⬜ 184                       });
⬜ 185    
🟩 186      QObject::connect(&sftp_channel, &orbit_ssh_qt::SftpChannel::stopped, &loop, [&]() {
⬜ 187        ORBIT_LOG("Sftp channel closed!");
⬜ 188        loop.quit();
⬜ 189        CheckCheckpoint(Checkpoint::kSftpChannelStopped);
```

<br/>

Also in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/IntegrationTests.cpp
```c++
⬜ 235    
⬜ 236      orbit_ssh_qt::Session session{&context.value()};
⬜ 237    
🟩 238      orbit_ssh_qt::SftpChannel channel{&session};
⬜ 239    
⬜ 240      orbit_ssh_qt::SftpCopyToLocalOperation sftp_copy_to_local{&session, &channel};
⬜ 241    
```

<br/>

Still in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/IntegrationTests.cpp
```c++
⬜ 247      });
⬜ 248    
⬜ 249      std::string file_name;
🟩 250      QObject::connect(&channel, &orbit_ssh_qt::SftpChannel::started, &loop, [&]() {
⬜ 251        QTemporaryFile temp_file;
⬜ 252        ASSERT_TRUE(temp_file.open());
⬜ 253        file_name = temp_file.fileName().toStdString();
```

<br/>

Also notice in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/IntegrationTests.cpp
```c++
⬜ 257        sftp_copy_to_local.CopyFileToLocal("/proc/cpuinfo", file_name);
⬜ 258      });
⬜ 259    
🟩 260      QObject::connect(&channel, &orbit_ssh_qt::SftpChannel::stopped, &loop, [&]() {
⬜ 261        ORBIT_LOG("Sftp channel closed!");
⬜ 262        loop.quit();
⬜ 263      });
```

<br/>

We modify `📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h`, for instance:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h
```c
⬜ 33     }  // namespace details
⬜ 34     
⬜ 35     /*
🟩 36       SftpChannel is a standard SSH channel with the SFTP subsystem initialized.
⬜ 37     
⬜ 38       That's a requirement for issuing SFTP commands to the remote side.
⬜ 39     
```

<br/>

Also notice in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h
```c
⬜ 41       SftpCopyToRemoteOperation. The user needs to keep the channel alive as long as
⬜ 42       SftpOperations are still running.
⬜ 43     */
🟩 44     class SftpChannel : public StateMachineHelper<SftpChannel, details::SftpChannelState> {
⬜ 45       Q_OBJECT
⬜ 46       friend StateMachineHelper;
⬜ 47     
⬜ 48      public:
🟩 49       explicit SftpChannel(Session* session);
⬜ 50     
⬜ 51       void Start();
⬜ 52       void Stop();
```

<br/>

<!-- THIS IS AN AUTOGENERATED SECTION. DO NOT EDIT THIS SECTION DIRECTLY -->
### Swimm Note

<span id="f-1ViGGG">SftpChannel</span>[^](#1ViGGG) - "src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h" L44
```c
class SftpChannel : public StateMachineHelper<SftpChannel, details::SftpChannelState> {
```

<span id="f-6LLyq">SftpChannelState</span>[^](#6LLyq) - "src/OrbitSshQt/include/OrbitSshQt/SftpChannel.h" L23
```c
enum class SftpChannelState {
```

<span id="f-2eKR5k">StateMachineHelper</span>[^](#2eKR5k) - "src/OrbitSshQt/include/OrbitSshQt/StateMachineHelper.h" L145
```c
class StateMachineHelper : public QObject {
```

<br/>

This file was generated by Swimm. [Click here to view it in the app](http://localhost:5000/repos/Z2l0aHViJTNBJTNBb3JiaXQlM0ElM0FBZGRpZUNvaGVu/docs/677dq).