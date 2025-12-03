#include "openperf/accessibility.hpp"

namespace openperf {

std::vector<AccessibilityIssue> AccessibilityAnalyzer::analyze(const Page& page) const {
    std::vector<AccessibilityIssue> issues;
    if (page.root) checkNode(page.root, issues);
    return issues;
}

void AccessibilityAnalyzer::checkNode(const std::shared_ptr<Node>& node, std::vector<AccessibilityIssue>& out) const {
    if (!node) return;

    // Rule: Images must have alt text or aria-label
    if (node->tag == "img") {
        bool hasAlt = !node->ariaLabel.empty() || !node->text.empty();
        if (!hasAlt) {
            out.emplace_back("IMG_ALT_MISSING", 
                "Image element is missing descriptive text or aria-label.",
                Severity::Warning, 
                node->id);
        }
    }

    // Rule: Interactive elements must have labels
    if (node->isInteractive) {
        bool hasLabel = !node->ariaLabel.empty() || !node->text.empty();
        if (!hasLabel) {
            out.emplace_back("INTERACTIVE_MISSING_LABEL",
                "Interactive element lacks a visible label or aria-label.",
                Severity::Error,
                node->id);
        }
    }

    // Rule: Buttons and links must have accessible text
    if (node->tag == "button" || node->tag == "a") {
        bool hasAccessibleText = !node->ariaLabel.empty() || !node->text.empty();
        if (!hasAccessibleText) {
            out.emplace_back("BUTTON_LINK_NO_TEXT",
                node->tag == "button" 
                    ? "Button element has no accessible text or aria-label."
                    : "Link element has no accessible text or aria-label.",
                Severity::Error,
                node->id);
        }
    }

    // Rule: Heading hierarchy (simplified - check for h1-h6 tags)
    if (node->tag.length() == 2 && node->tag[0] == 'h' && node->tag[1] >= '1' && node->tag[1] <= '6') {
        // In a full implementation, we'd track heading levels across the tree
        // For now, we just check that headings have text
        if (node->text.empty() && node->ariaLabel.empty()) {
            out.emplace_back("HEADING_NO_TEXT",
                "Heading element has no text content.",
                Severity::Warning,
                node->id);
        }
    }

    // Recursively check children
    for (auto& child : node->children) {
        checkNode(child, out);
    }
}

}