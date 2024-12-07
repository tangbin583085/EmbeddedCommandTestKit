#pragma once

#include "EmbeddedCommandTestKit/CommandRunner.hpp"

#include <string>
#include <vector>

namespace ectk {

struct ScenarioStep {
    std::string name;
    CommandRequest request;
    ResponseValidator validator;
};

struct ScenarioResult {
    std::vector<CommandResult> results;
    std::vector<std::string> stepNames;
    bool stoppedOnFailure{false};

    bool passed() const noexcept
    {
        if (results.empty()) {
            return true;
        }
        for (const auto& result : results) {
            if (!result.passed()) {
                return false;
            }
        }
        return true;
    }
};

class ScenarioRunner {
public:
    explicit ScenarioRunner(ITransport& transport);

    ScenarioResult run(
        const std::vector<ScenarioStep>& steps,
        bool stopOnFailure = true);

private:
    CommandRunner commandRunner_;
};

} // namespace ectk

