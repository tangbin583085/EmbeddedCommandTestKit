#include "EmbeddedCommandTestKit/MockTransport.hpp"

#include <chrono>
#include <thread>
#include <utility>

namespace ectk {

bool MockTransport::open()
{
    open_ = true;
    return true;
}

void MockTransport::close() noexcept
{
    open_ = false;
}

bool MockTransport::isOpen() const noexcept
{
    return open_;
}

TransportReply MockTransport::transact(
    const CommandRequest& request,
    std::chrono::milliseconds timeout)
{
    sentRequests_.push_back(request);

    if (!open_) {
        return TransportReply::error("transport is closed");
    }

    if (faults_.responseDelay > std::chrono::milliseconds::zero()) {
        if (faults_.responseDelay >= timeout) {
            std::this_thread::sleep_for(timeout);
            return TransportReply::timeout("fault profile exceeded timeout");
        }
        std::this_thread::sleep_for(faults_.responseDelay);
    }

    if (faults_.dropResponse) {
        return TransportReply::timeout("fault profile dropped response");
    }

    TransportReply reply;
    if (!script_.empty()) {
        reply = std::move(script_.front());
        script_.pop_front();
    } else if (handler_) {
        reply = handler_(request);
    } else {
        reply = TransportReply::error("no scripted response");
    }

    if (faults_.disconnectAfterReply) {
        close();
    }
    return reply;
}

void MockTransport::queueResponse(
    std::uint16_t commandId,
    std::uint16_t statusCode,
    Bytes payload)
{
    script_.push_back(TransportReply::responseReceived(
        CommandResponse{commandId, statusCode, std::move(payload)}));
}

void MockTransport::queueTimeout(std::string message)
{
    script_.push_back(TransportReply::timeout(std::move(message)));
}

void MockTransport::queueError(std::string message)
{
    script_.push_back(TransportReply::error(std::move(message)));
}

void MockTransport::setHandler(Handler handler)
{
    handler_ = std::move(handler);
}

void MockTransport::setFaultProfile(FaultProfile profile)
{
    faults_ = profile;
}

void MockTransport::clearScript()
{
    script_.clear();
}

const std::vector<CommandRequest>& MockTransport::sentRequests() const noexcept
{
    return sentRequests_;
}

} // namespace ectk

