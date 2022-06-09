#pragma once

#include "EmbeddedCommandTestKit/Transport.hpp"

#include <deque>
#include <functional>

namespace ectk {

struct FaultProfile {
    std::chrono::milliseconds responseDelay{0};
    bool dropResponse{false};
    bool disconnectAfterReply{false};
};

class MockTransport final : public ITransport {
public:
    using Handler = std::function<TransportReply(const CommandRequest&)>;

    bool open() override;
    void close() noexcept override;
    bool isOpen() const noexcept override;

    TransportReply transact(
        const CommandRequest& request,
        std::chrono::milliseconds timeout) override;

    void queueResponse(
        std::uint16_t commandId,
        std::uint16_t statusCode,
        Bytes payload = {});
    void queueTimeout(std::string message = "scripted timeout");
    void queueError(std::string message);

    void setHandler(Handler handler);
    void setFaultProfile(FaultProfile profile);
    void clearScript();

    const std::vector<CommandRequest>& sentRequests() const noexcept;

private:
    bool open_{false};
    std::deque<TransportReply> script_;
    Handler handler_;
    FaultProfile faults_;
    std::vector<CommandRequest> sentRequests_;
};

} // namespace ectk

