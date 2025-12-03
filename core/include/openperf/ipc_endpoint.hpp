#pragma once

#include "openperf/page.hpp"
#include "openperf/accessibility.hpp"
#include "openperf/metrics.hpp"

#include <string>
#include <vector>
#include <memory>

namespace openperf {

/**
 * Abstract interface for IPC endpoints that expose Engine functionality.
 * 
 * This abstraction allows plugging in different transport mechanisms:
 * - gRPC (current implementation in daemon/)
 * - Unix domain sockets
 * - Shared memory transport
 * - Named pipes
 * 
 * The core Engine remains transport-agnostic, making it easy to:
 * - Test with different IPC backends
 * - Optimize for specific use cases (low-latency shared memory, etc.)
 * - Support multiple protocols simultaneously
 */
class IEngineEndpoint {
public:
    virtual ~IEngineEndpoint() = default;

    /**
     * Submit a page to the engine for processing.
     * Returns the page ID (generated if not provided).
     */
    virtual std::string submitPage(Page page) = 0;

    /**
     * Trigger the render pipeline for a given page.
     * This is asynchronous - the pipeline runs in a background task.
     */
    virtual void runRenderPipeline(const std::string& pageId) = 0;

    /**
     * Analyze accessibility issues for a given page.
     * Returns a list of discovered issues.
     */
    virtual std::vector<AccessibilityIssue> analyzeAccessibility(const std::string& pageId) = 0;

    /**
     * Get current metrics from the engine.
     */
    virtual std::vector<Metric> getMetrics() const = 0;
};

} // namespace openperf

