#pragma once

#include "openperf/engine.hpp"
#include "openperf/ipc_endpoint.hpp"

namespace openperf {

/**
 * Concrete implementation of IEngineEndpoint that delegates to an Engine instance.
 * 
 * This adapter allows the Engine to be used through any IPC endpoint interface,
 * keeping the Engine itself free of transport-specific code.
 */
class EngineEndpointAdapter : public IEngineEndpoint {
public:
    explicit EngineEndpointAdapter(Engine& engine) : engine_(engine) {}

    std::string submitPage(Page page) override {
        return engine_.submitPage(std::move(page));
    }

    void runRenderPipeline(const std::string& pageId) override {
        engine_.runRenderPipeline(pageId);
    }

    std::vector<AccessibilityIssue> analyzeAccessibility(const std::string& pageId) override {
        return engine_.analyzeAccessibility(pageId);
    }

    std::vector<Metric> getMetrics() const override {
        return engine_.getMetrics();
    }

private:
    Engine& engine_;
};

} // namespace openperf

