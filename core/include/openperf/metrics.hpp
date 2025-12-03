#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <mutex>

namespace openperf {

struct Metric {
    std::string name;
    double value;
    std::chrono::steady_clock::time_point timestamp;
};

class Metrics {
public:
    void record(const std::string& name, double value);
    std::vector<Metric> getMetrics() const; // by value, avoid potential UB
private:
    mutable std::mutex mutex_;
    std::vector<Metric> samples_;
};

}
