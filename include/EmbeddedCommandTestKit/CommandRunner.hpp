#pragma once

#include "EmbeddedCommandTestKit/Transport.hpp"

namespace ectk {

class CommandRunner {
public:
    explicit CommandRunner(ITransport& transport);

    CommandResult execute(
        const CommandRequest& request,
        ResponseValidator validator = {});

private:
    ITransport& transport_;
};

} // namespace ectk

