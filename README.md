# EmbeddedCommandTestKit

## 中文说明

EmbeddedCommandTestKit 是一个用于测试嵌入式设备指令的轻量级 C++ 库。它运行在电脑端，可以通过自定义传输适配层接入实际设备，也可以先使用模拟传输层编写测试。

它主要处理指令发送、响应检查、超时、重试和多条指令流程。库本身不模拟完整的 MCU、GPIO 或传感器，只关注设备指令和通信行为。

### 当前功能

- 指令请求和响应的基本数据结构
- 可替换的传输接口
- 内存中的 `MockTransport`
- 响应超时和传输错误处理
- 最大尝试次数和重试
- 自定义响应校验
- 多步骤场景执行
- 响应延迟、丢失响应和执行后断开等异常模拟
- CMake 安装和静态库集成

核心库不依赖 Qt 或其他第三方库。串口、TCP 或 Qt 等通信方式可以在 `ITransport` 的基础上单独实现。

### 构建

需要 CMake 3.16 或更高版本以及 C++17 编译器：

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

如果只需要库文件，可以关闭示例和源码测试：

```bash
cmake -S . -B build \
  -DECTK_BUILD_EXAMPLES=OFF \
  -DECTK_BUILD_TESTS=OFF
cmake --build build
```

### 基本使用

```cpp
#include <chrono>
#include <string>

#include <EmbeddedCommandTestKit/CommandRunner.hpp>
#include <EmbeddedCommandTestKit/MockTransport.hpp>

ectk::MockTransport transport;
transport.open();
transport.queueResponse(0x1001, 0, {0x01, 0x02});

ectk::CommandRequest request;
request.commandId = 0x1001;
request.timeout = std::chrono::milliseconds(500);
request.maxAttempts = 2;

auto result = ectk::CommandRunner(transport).execute(
    request,
    [](const ectk::CommandRequest&, const ectk::CommandResponse& response) {
        return response.statusCode == 0 ? std::string{} : "device returned an error";
    });

if (result.passed()) {
    // 指令测试通过
}
```

实际串口或 TCP 适配层只需要实现 `ITransport`，把设备协议的编码和解码放在适配层中。这样测试执行器和具体通信方式可以分开维护。

### 使用范围

核心库提供指令执行、响应校验、超时重试和 `MockTransport`。串口、TCP 等实际连接方式可以通过实现 `ITransport` 接入，具体协议的编码和解码由对应适配层负责。

### CMake 集成

安装库后，其他 CMake 项目可以直接使用导出的目标：

```cmake
find_package(EmbeddedCommandTestKit CONFIG REQUIRED)
target_link_libraries(your_app PRIVATE ECTK::Core)
```

## English

EmbeddedCommandTestKit is a small C++ library for testing commands sent to embedded devices. It runs on the host computer and can use a custom transport adapter for a real device or the built-in mock transport while a test is being written.

The library focuses on command requests, response checks, timeouts, retries, and small command sequences. It does not try to emulate a complete MCU, GPIO set, or sensor board.

### Features

- Basic command request and response types
- Replaceable transport interface
- In-memory `MockTransport`
- Timeout and transport error handling
- Attempt limits and retries
- Custom response validators
- Multi-step scenario execution
- Response delay, dropped response, and disconnect fault simulation
- CMake installation and static library integration

The core library has no Qt or third-party dependency. Serial, TCP, or Qt communication can be implemented separately on top of `ITransport`.

### Build

Use CMake 3.16 or newer with a C++17 compiler:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

To build only the library, turn off the examples and source tests:

```bash
cmake -S . -B build \
  -DECTK_BUILD_EXAMPLES=OFF \
  -DECTK_BUILD_TESTS=OFF
cmake --build build
```

### Basic use

```cpp
#include <chrono>
#include <string>

#include <EmbeddedCommandTestKit/CommandRunner.hpp>
#include <EmbeddedCommandTestKit/MockTransport.hpp>

ectk::MockTransport transport;
transport.open();
transport.queueResponse(0x1001, 0, {0x01, 0x02});

ectk::CommandRequest request;
request.commandId = 0x1001;
request.timeout = std::chrono::milliseconds(500);
request.maxAttempts = 2;

auto result = ectk::CommandRunner(transport).execute(
    request,
    [](const ectk::CommandRequest&, const ectk::CommandResponse& response) {
        return response.statusCode == 0 ? std::string{} : "device returned an error";
    });

if (result.passed()) {
    // The command test passed.
}
```

An actual serial or TCP adapter only needs to implement `ITransport`. Protocol encoding and decoding can stay in that adapter, keeping the test runner independent from the communication method.

### Scope

The core library provides command execution, response validation, timeout handling, retries, and `MockTransport`. Serial, TCP, or other communication methods can be integrated through `ITransport`, while protocol encoding and decoding remain in the adapter layer.

### CMake integration

After installing the library, a consumer project can use the exported target:

```cmake
find_package(EmbeddedCommandTestKit CONFIG REQUIRED)
target_link_libraries(your_app PRIVATE ECTK::Core)
```

