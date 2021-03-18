#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <optional>
#include <string>
#include <utility>
#include <vector>

namespace ectk {

using Bytes = std::vector<std::uint8_t>;

enum class TransportStatus {
    Response,
    Timeout,
    Error
};

enum class CommandResultCode {
    Success,
    Timeout,
    TransportError,
    InvalidResponse,
    RetryLimitReached
};

struct CommandRequest {
    std::uint16_t commandId{0};
    Bytes payload;
    std::chrono::milliseconds timeout{1000};
    std::size_t maxAttempts{1};
};

struct CommandResponse {
    std::uint16_t commandId{0};
    std::uint16_t statusCode{0};
    Bytes payload;
};

struct TransportReply {
    TransportStatus status{TransportStatus::Error};
    std::optional<CommandResponse> response;
    std::string message;

    static TransportReply responseReceived(CommandResponse value)
    {
        return {TransportStatus::Response, std::move(value), {}};
    }

    static TransportReply timeout(std::string reason = "response timeout")
    {
        return {TransportStatus::Timeout, std::nullopt, std::move(reason)};
    }

    static TransportReply error(std::string reason)
    {
        return {TransportStatus::Error, std::nullopt, std::move(reason)};
    }
};

// Return an empty string when the response is valid. Otherwise return a short
// message that can be written to a test report.
using ResponseValidator = std::function<std::string(
    const CommandRequest& request,
    const CommandResponse& response)>;

struct CommandResult {
    CommandResultCode code{CommandResultCode::TransportError};
    std::optional<CommandResponse> response;
    std::size_t attempts{0};
    std::chrono::milliseconds elapsed{0};
    std::string message;

    bool passed() const noexcept
    {
        return code == CommandResultCode::Success;
    }
};

} // namespace ectk

