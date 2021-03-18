#pragma once

#include "EmbeddedCommandTestKit/CommandTypes.hpp"

namespace ectk {

class ITransport {
public:
    virtual ~ITransport() = default;

    virtual bool open() = 0;
    virtual void close() noexcept = 0;
    virtual bool isOpen() const noexcept = 0;

    virtual TransportReply transact(
        const CommandRequest& request,
        std::chrono::milliseconds timeout) = 0;
};

} // namespace ectk

