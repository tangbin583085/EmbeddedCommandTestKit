#include "EmbeddedCommandTestKit/ScenarioRunner.hpp"

#include <utility>

namespace ectk {

ScenarioRunner::ScenarioRunner(ITransport& transport)
    : commandRunner_(transport)
{
}

ScenarioResult ScenarioRunner::run(
    const std::vector<ScenarioStep>& steps,
    bool stopOnFailure)
{
    ScenarioResult scenario;
    scenario.results.reserve(steps.size());
    scenario.stepNames.reserve(steps.size());

    for (const auto& step : steps) {
        scenario.stepNames.push_back(step.name);
        auto result = commandRunner_.execute(step.request, step.validator);
        const bool failed = !result.passed();
        scenario.results.push_back(std::move(result));
        if (failed && stopOnFailure) {
            scenario.stoppedOnFailure = true;
            break;
        }
    }
    return scenario;
}

} // namespace ectk

