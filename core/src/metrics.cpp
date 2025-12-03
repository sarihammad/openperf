#include "openperf/metrics.hpp"

namespace openperf {

void Metrics::record(const std::string& name, double value) {
    std::lock_guard<std::mutex> lock{mutex_};
    samples_.emplace_back(name, value, std::chrono::steady_clock::now());
}

std::vector<Metric> Metrics::getMetrics() const { 
    std::lock_guard<std::mutex> lock{mutex_};
    return samples_;
}

}
