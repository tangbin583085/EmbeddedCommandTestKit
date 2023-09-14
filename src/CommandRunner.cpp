#include "EmbeddedCommandTestKit/CommandRunner.hpp"

#include <chrono>
#include <utility>

namespace ectk {

CommandRunner::CommandRunner(ITransport& transport)
    : transport_(transport)
{
}

CommandResult CommandRunner::execute(
    const CommandRequest& request,
    ResponseValidator validator)
{
    const auto started = std::chrono::steady_clock::now();
    const std::size_t attemptsAllowed = request.maxAttempts == 0 ? 1 : request.maxAttempts;

    CommandResult result;
    CommandResultCode lastCode = CommandResultCode::TransportError;
    std::string lastMessage;

    for (std::size_t attempt = 1; attempt <= attemptsAllowed; ++attempt) {
        result.attempts = attempt;
        const auto reply = transport_.transact(request, request.timeout);

        if (reply.status == TransportStatus::Timeout) {
            lastCode = CommandResultCode::Timeout;
            lastMessage = reply.message.empty() ? "response timeout" : reply.message;
        } else if (reply.status == TransportStatus::Error) {
            lastCode = CommandResultCode::TransportError;
            lastMessage = reply.message.empty() ? "transport error" : reply.message;
        } else if (!reply.response.has_value()) {
            lastCode = CommandResultCode::InvalidResponse;
            lastMessage = "transport returned no response";
        } else if (reply.response->commandId != request.commandId) {
            lastCode = CommandResultCode::InvalidResponse;
            lastMessage = "response command id does not match request";
            result.response = reply.response;
        } else {
            result.response = reply.response;
            if (validator) {
                lastMessage = validator(request, *reply.response);
                if (!lastMessage.empty()) {
                    lastCode = CommandResultCode::InvalidResponse;
                } else {
                    result.code = CommandResultCode::Success;
                    result.message.clear();
                    break;
                }
            } else {
                result.code = CommandResultCode::Success;
                result.message.clear();
                break;
            }
        }
    }

    if (!result.passed()) {
        result.code = attemptsAllowed > 1
            ? CommandResultCode::RetryLimitReached
            : lastCode;
        result.message = attemptsAllowed > 1
            ? "attempt limit reached: " + lastMessage
            : lastMessage;
    }

    result.elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        std::chrono::steady_clock::now() - started);
    return result;
}

} // namespace ectk

