#include "openperf/engine.hpp"
#include <iostream>

using namespace openperf;

int main() {
    Engine engine;
    engine.start();

    // build a simple page
    auto root = std::make_shared<Node>();
    root->tag = "div";
    root->id = "root";

    auto img = std::make_shared<Node>();
    img->tag = "img";
    img->id = "hero-image";
    // no ariaLabel/text -> should trigger IMG_ALT_MISSING

    root->children.push_back(img);

    Page page;
    page.url = "https://example.com";
    page.root = root;

    auto pageId = engine.submitPage(std::move(page));

    // kick off render pipeline
    engine.runRenderPipeline(pageId);

    // run accessibility analysis
    auto issues = engine.analyzeAccessibility(pageId);

    std::println("Accessibility issues for page {}:", pageId);
    for (const auto& issue : issues) {
        std::println("- [{}] {} (node: {})", issue.code, issue.message, issue.nodeId);

    }

    // fetch some metrics. (later: wait or poll)
    auto samples = engine.getMetrics();
    std::println("Metrics collected:");
    for (const auto& s : samples) {
        std::println("- {} = {}", s.name, s.value);
    }

    engine.stop();
    return 0;
}