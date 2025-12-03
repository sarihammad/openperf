#include "openperf/engine.hpp"
#include <chrono>

namespace openperf {

Engine::Engine() : scheduler_(std::thread::hardware_concurrency()) {}

Engine::~Engine() {
    stop();
}

void Engine::start() {
    scheduler_.start();
}

void Engine::stop() {
    scheduler_.stop();
}

std::optional<Page> Engine::getPage(const std::string& pageId) const {
    std::lock_guard<std::mutex> lock(pagesMutex_);
    auto it = pages_.find(pageId);
    if (it == pages_.end()) return std::nullopt;
    return it->second;
}

void Engine::runRenderPipeline(const std::string& pageId) {
    auto maybePage = getPage(pageId);
    if (!maybePage) return;

    Page pageCopy = *maybePage;

    scheduler_.enqueue([this, pageCopy]() {
        auto t0 = std::chrono::steady_clock::now();

        // Stage 1: Parse
        auto t_parse_start = t0;
        // Simulate parsing DOM structure
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto t_parse_end = std::chrono::steady_clock::now();

        // Stage 2: Layout
        auto t_layout_start = t_parse_end;
        // Simulate layout calculations (box model, positioning)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto t_layout_end = std::chrono::steady_clock::now();

        // Stage 3: Paint
        auto t_paint_start = t_layout_end;
        // Simulate paint operations (drawing to layers)
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto t_paint_end = std::chrono::steady_clock::now();

        // Stage 4: Composite
        auto t_comp_start = t_paint_end;
        // Simulate compositing layers into final image
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
        auto t_comp_end = std::chrono::steady_clock::now();

        using ms = std::chrono::duration<double, std::milli>;
        metrics_.record("parse_ms", ms(t_parse_end - t_parse_start).count());
        metrics_.record("layout_ms", ms(t_layout_end - t_layout_start).count());
        metrics_.record("paint_ms", ms(t_paint_end - t_paint_start).count());
        metrics_.record("composite_ms", ms(t_comp_end - t_comp_start).count());
        metrics_.record("render_pipeline_latency_ms", ms(t_comp_end - t0).count());
        
        // Record queue depth after task completion
        auto queueDepth = scheduler_.getQueueDepth();
        metrics_.record("task_queue_depth", static_cast<double>(queueDepth));
    });
}

std::vector<AccessibilityIssue> Engine::analyzeAccessibility(const std::string& pageId) {
    auto maybePage = getPage(pageId);
    if (!maybePage) return {};

    return accessibility_.analyze(*maybePage);
}

std::vector<Metric> Engine::getMetrics() const {
    return metrics_.getMetrics();
}

}