#pragma once

#include "openperf/page.hpp"
#include <string>
#include <vector>

namespace openperf {

enum class Severity {
    Info, Warning, Error
};

struct AccessibilityIssue {
    std::string code;
    std::string message;
    Severity severity;
    std::string nodeId;
};

class AccessibilityAnalyzer {
public:
    std::vector<AccessibilityIssue> analyze(const Page& page) const;

private:
    void checkNode(const std::shared_ptr<Node>& node, std::vector<AccessibilityIssue>& out) const;
};

}
