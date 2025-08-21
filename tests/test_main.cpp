#include "EmbeddedCommandTestKit/CommandRunner.hpp"
#include "EmbeddedCommandTestKit/MockTransport.hpp"
#include "EmbeddedCommandTestKit/ScenarioRunner.hpp"

#include <cassert>

namespace {

void successResponseIsAccepted()
{
    ectk::MockTransport transport;
    transport.open();
    transport.queueResponse(0x10, 0, {0x01});

    ectk::CommandRequest request;
    request.commandId = 0x10;

    const auto result = ectk::CommandRunner(transport).execute(request);
    assert(result.passed());
    assert(result.attempts == 1);
    assert(result.response->payload == ectk::Bytes{0x01});
}

void timeoutCanBeRetried()
{
    ectk::MockTransport transport;
    transport.open();
    transport.queueTimeout();
    transport.queueResponse(0x20, 0, {});

    ectk::CommandRequest request;
    request.commandId = 0x20;
    request.maxAttempts = 2;

    const auto result = ectk::CommandRunner(transport).execute(request);
    assert(result.passed());
    assert(result.attempts == 2);
}

void validatorCanRejectResponse()
{
    ectk::MockTransport transport;
    transport.open();
    transport.queueResponse(0x30, 7, {});

    ectk::CommandRequest request;
    request.commandId = 0x30;

    const auto result = ectk::CommandRunner(transport).execute(
        request,
        [](const ectk::CommandRequest&, const ectk::CommandResponse& response) {
            return response.statusCode == 0 ? std::string{} : "device returned an error";
        });
    assert(!result.passed());
    assert(result.code == ectk::CommandResultCode::InvalidResponse);
}

void scenarioStopsAfterFailure()
{
    ectk::MockTransport transport;
    transport.open();
    transport.queueError("device disconnected");
    transport.queueResponse(0x40, 0, {});

    ectk::CommandRequest first;
    first.commandId = 0x40;
    ectk::CommandRequest second;
    second.commandId = 0x41;

    const auto result = ectk::ScenarioRunner(transport).run({
        {"first", first, {}},
        {"second", second, {}}
    });
    assert(!result.passed());
    assert(result.stoppedOnFailure);
    assert(result.results.size() == 1);
}

} // namespace

int main()
{
    successResponseIsAccepted();
    timeoutCanBeRetried();
    validatorCanRejectResponse();
    scenarioStopsAfterFailure();
    return 0;
}

