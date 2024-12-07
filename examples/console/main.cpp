#include "EmbeddedCommandTestKit/MockTransport.hpp"
#include "EmbeddedCommandTestKit/ScenarioRunner.hpp"

#include <iostream>

int main()
{
    ectk::MockTransport transport;
    transport.open();
    transport.queueResponse(0x1001, 0, {1, 2, 3});
    transport.queueResponse(0x1002, 0, {0x42});

    ectk::ScenarioRunner runner(transport);

    ectk::CommandRequest version;
    version.commandId = 0x1001;

    ectk::CommandRequest readValue;
    readValue.commandId = 0x1002;

    const auto scenario = runner.run({
        {"read version", version, {}},
        {"read value", readValue,
            [](const ectk::CommandRequest&, const ectk::CommandResponse& response) {
                return response.payload.size() == 1 ? std::string{} : "value size is not one";
            }}
    });

    std::cout << "scenario: " << (scenario.passed() ? "passed" : "failed")
              << " (" << scenario.results.size() << " step(s))\n";
    return scenario.passed() ? 0 : 1;
}

