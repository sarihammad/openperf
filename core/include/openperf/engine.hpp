#pragma once

#include "openperf/page.hpp"
#include "openperf/task_scheduler.hpp"
#include "openperf/metrics.hpp"
#include "openperf/accessibility.hpp"

#include <unordered_map>
#include <mutex>
#include <atomic>
#include <optional>
#include <iostream>

namespace openperf {

class Engine {
public:
    Engine();
    ~Engine();

    void start();
    void stop();

    std::string submitPage(Page page) {
        static std::atomic<std::uint64_t> g_pageCounter{0};

        std::cout << "[engine] submitPage: initial id='" << page.id << "'\n";

        if (page.id.empty()) {
            auto idNum = g_pageCounter.fetch_add(1, std::memory_order_relaxed);
            page.id = "page-" + std::to_string(idNum);
        }

        std::cout << "[engine] submitPage: generated id='" << page.id << "'\n";

        {
            std::lock_guard<std::mutex> lock(pagesMutex_);
            pages_.emplace(page.id, std::move(page));
        }

        return page.id;
    }

    void runRenderPipeline(const std::string& pageId); // async via scheduler

    std::vector<AccessibilityIssue> analyzeAccessibility(const std::string& pageId);
    std::vector<Metric> getMetrics() const;

private:
    std::optional<Page> getPage(const std::string& pageId) const;

    mutable std::mutex pagesMutex_;
    std::unordered_map<std::string, Page> pages_;

    TaskScheduler scheduler_;
    Metrics metrics_;
    AccessibilityAnalyzer accessibility_;
};

}