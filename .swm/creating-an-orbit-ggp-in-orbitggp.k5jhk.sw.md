---
id: k5jhk
name: Creating an Orbit Ggp in Orbitggp
file_version: 1.0.2
app_version: 0.9.4-5
file_blobs:
  src/OrbitGgp/include/OrbitGgp/Account.h: fffc2fe061416ca52ce4092b787604d615952ff4
  src/OrbitGgp/CMakeLists.txt: 3fa74b45b05f6842688b842f2023d3bf96ac4618
  src/OrbitGgp/Client.cpp: 8ab24e63275a62b5004e54e5e2b1af2834dedebe
  src/OrbitGgp/ClientTest.cpp: 932dd3a36ff7db537262d32090b470fb523aab16
  src/OrbitGgp/include/OrbitGgp/Client.h: a2474fd89eccbbfb5c1e32d2bf7b74793c48f00a
  src/SessionSetup/ConnectToStadiaWidget.cpp: 2c07a0be6b351fcc0a246463ab79e7358ddc9a83
  src/SessionSetup/RetrieveInstancesTest.cpp: cee267c6e85604c0e0112df33552dacc783905a7
  src/SessionSetup/include/SessionSetup/ConnectToStadiaWidget.h: 9de5ae8c14fdcf9508b684aef9b773d9499c63ec
---

An Orbit Ggp is a major component in our system. This document will describe what it is and how to add a new one.

Some examples of `📄 src/OrbitGgp/include/OrbitGgp`s are `📄 src/OrbitGgp/include/OrbitGgp/Account.h`, `📄 src/OrbitGgp/include/OrbitGgp/Instance.h`, and `📄 src/OrbitGgp/include/OrbitGgp/Project.h`.

## TL;DR - How to add `📄 src/OrbitGgp/include/OrbitGgp`s

1.  Create a new file under `📄 src/OrbitGgp/include/OrbitGgp` 
    
    *   e.g. `📄 src/OrbitGgp/include/OrbitGgp/Account.h`
        
2.  Implement the Orbit Ggp struct (e.g. `Account`[<sup id="2c72kk">↓</sup>](#f-2c72kk))
    
    1.  Update `📄 src/OrbitGgp/CMakeLists.txt` with the new struct
        
    2.  Update `📄 src/OrbitGgp/Client.cpp` with the new struct
        
    3.  Update `📄 src/OrbitGgp/ClientTest.cpp` with the new struct
        
    4.  Update `📄 src/OrbitGgp/include/OrbitGgp/Client.h` with the new struct
        
    5.  Update `📄 src/SessionSetup/ConnectToStadiaWidget.cpp` with the new struct
        
    6.  Update `📄 src/SessionSetup/RetrieveInstancesTest.cpp` with the new struct
        
    7.  Update `📄 src/SessionSetup/include/SessionSetup/ConnectToStadiaWidget.h` with the new struct
        
3.  **Profit** 💰
    

# Full walkthrough

Start by creating a new file under `📄 src/OrbitGgp/include/OrbitGgp`. We'll follow `📄 src/OrbitGgp/include/OrbitGgp/Account.h` as an example.

## Implement the Orbit Ggp struct

Here is how we do it for `📄 src/OrbitGgp/include/OrbitGgp/Account.h`:

<br/>



<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGgp/include/OrbitGgp/Account.h
```c
⬜ 11     
⬜ 12     namespace orbit_ggp {
⬜ 13     
🟩 14     struct Account {
🟩 15       QString email;
🟩 16     
🟩 17       static ErrorMessageOr<Account> GetDefaultAccountFromJson(const QByteArray& json);
🟩 18     };
⬜ 19     
⬜ 20     }  // namespace orbit_ggp
⬜ 21     
```

<br/>

### Update additional files with the new struct

Every time we add new `📄 src/OrbitGgp/include/OrbitGgp`s, we reference them in a few locations.

We will still look at `📄 src/OrbitGgp/include/OrbitGgp/Account.h` as our example.

<br/>

We modify `📄 src/OrbitGgp/CMakeLists.txt`, for example:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGgp/CMakeLists.txt
```text
⬜ 19               include/OrbitGgp/SshInfo.h)
⬜ 20     
⬜ 21     target_sources(OrbitGgp PRIVATE
🟩 22               Account.cpp
⬜ 23               Client.cpp
⬜ 24               Error.cpp
⬜ 25               Instance.cpp
```

<br/>

Update `📄 src/OrbitGgp/Client.cpp`, like so:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGgp/Client.cpp
```c++
⬜ 50       Future<ErrorMessageOr<QVector<Project>>> GetProjectsAsync() override;
⬜ 51       Future<ErrorMessageOr<Project>> GetDefaultProjectAsync() override;
⬜ 52       Future<ErrorMessageOr<Instance>> DescribeInstanceAsync(const QString& instance_id) override;
🟩 53       Future<ErrorMessageOr<Account>> GetDefaultAccountAsync() override;
⬜ 54     
⬜ 55      private:
⬜ 56       const QString ggp_program_;
```

<br/>

Also notice in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGgp/Client.cpp
```c++
⬜ 168          });
⬜ 169    }
⬜ 170    
🟩 171    Future<ErrorMessageOr<Account>> ClientImpl::GetDefaultAccountAsync() {
⬜ 172      QStringList arguments{"auth", "list", "-s"};
⬜ 173    
⬜ 174      orbit_base::ImmediateExecutor executor;
⬜ 175      return orbit_qt_utils::ExecuteProcess(ggp_program_, arguments, this, absl::FromChrono(timeout_))
🟩 176          .ThenIfSuccess(&executor, [](const QByteArray& json) -> ErrorMessageOr<Account> {
🟩 177            return Account::GetDefaultAccountFromJson(json);
⬜ 178          });
⬜ 179    }
⬜ 180    
```

<br/>

Don't forget to add the new struct to `📄 src/OrbitGgp/ClientTest.cpp`, as seen here:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGgp/ClientTest.cpp
```c++
⬜ 483      bool future_is_resolved = false;
⬜ 484    
⬜ 485      client.value()->GetDefaultAccountAsync().Then(
🟩 486          main_thread_executor_.get(), [&future_is_resolved](ErrorMessageOr<Account> account) {
⬜ 487            EXPECT_FALSE(future_is_resolved);
⬜ 488            future_is_resolved = true;
⬜ 489            ASSERT_THAT(account, HasValue());
```

<br/>

Add the new struct to `📄 src/OrbitGgp/include/OrbitGgp/Client.h`, for instance:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGgp/include/OrbitGgp/Client.h
```c
⬜ 14     #include <optional>
⬜ 15     #include <string>
⬜ 16     
🟩 17     #include "Account.h"
⬜ 18     #include "Instance.h"
⬜ 19     #include "OrbitBase/Future.h"
⬜ 20     #include "OrbitBase/Result.h"
```

<br/>

Also in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/OrbitGgp/include/OrbitGgp/Client.h
```c
⬜ 46       [[nodiscard]] virtual orbit_base::Future<ErrorMessageOr<Project>> GetDefaultProjectAsync() = 0;
⬜ 47       [[nodiscard]] virtual orbit_base::Future<ErrorMessageOr<Instance>> DescribeInstanceAsync(
⬜ 48           const QString& instance_id) = 0;
🟩 49       [[nodiscard]] virtual orbit_base::Future<ErrorMessageOr<Account>> GetDefaultAccountAsync() = 0;
⬜ 50     };
⬜ 51     
⬜ 52     [[nodiscard]] std::chrono::milliseconds GetClientDefaultTimeoutInMs();
```

<br/>

We modify `📄 src/SessionSetup/ConnectToStadiaWidget.cpp`, as we do with `📄 src/OrbitGgp/include/OrbitGgp/Account.h` here:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/SessionSetup/ConnectToStadiaWidget.cpp
```c++
⬜ 58     namespace orbit_session_setup {
⬜ 59     
⬜ 60     using orbit_base::Future;
🟩 61     using orbit_ggp::Account;
⬜ 62     using orbit_ggp::Instance;
⬜ 63     using orbit_ggp::Project;
⬜ 64     using orbit_ggp::SshInfo;
```

<br/>

Still in the same file:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/SessionSetup/ConnectToStadiaWidget.cpp
```c++
⬜ 446      emit Connecting();
⬜ 447    }
⬜ 448    
🟩 449    ErrorMessageOr<Account> ConnectToStadiaWidget::GetAccountSync() {
⬜ 450      if (cached_account_ != std::nullopt) return cached_account_.value();
⬜ 451    
⬜ 452      // This async call is not doing network calls, only reads local config files, hence it is fast and
⬜ 453      // used in a syncronous manner here. A timeout of 3 is used anyways
🟩 454      ErrorMessageOr<Account> future_result = ErrorMessage{"Call to \"ggp auth list\" timed out."};
⬜ 455      auto account_future = ggp_client_->GetDefaultAccountAsync().Then(
🟩 456          main_thread_executor_.get(), [&future_result](ErrorMessageOr<Account> result) -> void {
⬜ 457            future_result = std::move(result);
⬜ 458          });
⬜ 459    
```

<br/>

Update `📄 src/SessionSetup/RetrieveInstancesTest.cpp`, as we do with `📄 src/OrbitGgp/include/OrbitGgp/Account.h` here:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/SessionSetup/RetrieveInstancesTest.cpp
```c++
⬜ 46       MOCK_METHOD(Future<ErrorMessageOr<Project>>, GetDefaultProjectAsync, (), (override));
⬜ 47       MOCK_METHOD(Future<ErrorMessageOr<Instance>>, DescribeInstanceAsync,
⬜ 48                   (const QString& /*instance_id*/), (override));
🟩 49       MOCK_METHOD(Future<ErrorMessageOr<orbit_ggp::Account>>, GetDefaultAccountAsync, (), (override));
⬜ 50     };
⬜ 51     
⬜ 52     namespace {
```

<br/>

Don't forget to add the new struct to `📄 src/SessionSetup/include/SessionSetup/ConnectToStadiaWidget.h`, for instance:
<!-- NOTE-swimm-snippet: the lines below link your snippet to Swimm -->
### 📄 src/SessionSetup/include/SessionSetup/ConnectToStadiaWidget.h
```c
⬜ 116      QState s_connected_;
⬜ 117    
⬜ 118      absl::flat_hash_map<std::string, orbit_ssh::Credentials> instance_credentials_;
🟩 119      std::optional<orbit_ggp::Account> cached_account_;
⬜ 120      std::unique_ptr<RetrieveInstances> retrieve_instances_;
⬜ 121    
⬜ 122      void SetupStateMachine();
⬜ 123      void OnInstancesLoaded(QVector<orbit_ggp::Instance> instances);
⬜ 124      void OnSshInfoLoaded(ErrorMessageOr<orbit_ggp::SshInfo> ssh_info_result, std::string instance_id);
⬜ 125      void TrySelectRememberedInstance();
🟩 126      ErrorMessageOr<orbit_ggp::Account> GetAccountSync();
⬜ 127    };
⬜ 128    
⬜ 129    }  // namespace orbit_session_setup
```

<br/>

<!-- THIS IS AN AUTOGENERATED SECTION. DO NOT EDIT THIS SECTION DIRECTLY -->
### Swimm Note

<span id="f-2c72kk">Account</span>[^](#2c72kk) - "src/OrbitGgp/include/OrbitGgp/Account.h" L14
```c
struct Account {
```

<br/>

This file was generated by Swimm. [Click here to view it in the app](http://localhost:5000/repos/Z2l0aHViJTNBJTNBb3JiaXQlM0ElM0FBZGRpZUNvaGVu/docs/k5jhk).