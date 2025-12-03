#pragma once

#include <string>
#include <vector>
#include <memory>

namespace openperf {

struct Node {
    std::string tag;
    std::string id;
    std::string text;
    std::string role;
    std::string ariaLabel;
    bool isInteractive = false;
    std::vector<std::shared_ptr<Node>> children;
};

struct Page {
    std::string id;
    std::string url;
    std::shared_ptr<Node> root;
};

}