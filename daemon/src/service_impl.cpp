#include "service_impl.hpp"
#include "openperf/page.hpp"
#include "openperf.pb.h"

#include <atomic>
#include <iostream>

// Core engine
using openperf::Engine;
using openperf::Metric;

using openperf_rpc::AnalyzeAccessibilityRequest;
using openperf_rpc::AnalyzeAccessibilityResponse;
using openperf_rpc::GetMetricsRequest;
using openperf_rpc::GetMetricsResponse;
using openperf_rpc::RunRenderRequest;
using openperf_rpc::RunRenderResponse;
using openperf_rpc::SubmitPageRequest;
using openperf_rpc::SubmitPageResponse;
using openperf_rpc::MetricSample;

OpenPerfServiceImpl::OpenPerfServiceImpl(Engine& engine)
    : engine_(engine) {}

::grpc::Status OpenPerfServiceImpl::SubmitPage(::grpc::ServerContext*,
                                               const SubmitPageRequest* request,
                                               SubmitPageResponse* response) {
    if (!request->has_page()) {
        return ::grpc::Status(::grpc::StatusCode::INVALID_ARGUMENT, "page is required");
    }

    // convert proto page to core page
    openperf::Page corePage = fromProto(request->page());
    std::cout << "[daemon] SubmitPage: incoming page.id='" << corePage.id
                << "', url='" << corePage.url << "'\n";

    // generate ID here
    auto pageId = ensurePageId(corePage);
    std::cout << "[daemon] SubmitPage: after ensurePageId, id='" << pageId << "'\n";

    // engine just stores it, we don't rely on its return value anymore
    engine_.submitPage(std::move(corePage));
    std::cout << "[daemon] SubmitPage: stored page, id='" << pageId << "'\n";

    response->set_page_id(pageId);
    return ::grpc::Status::OK;
}

::grpc::Status OpenPerfServiceImpl::RunRenderPipeline(::grpc::ServerContext*,
                                                      const RunRenderRequest* request,
                                                      RunRenderResponse*) {
    const auto& pageId = request->page_id();
    if (pageId.empty()) {
        return ::grpc::Status(::grpc::StatusCode::INVALID_ARGUMENT, "page_id is required");
    }

    engine_.runRenderPipeline(pageId);
    return ::grpc::Status::OK;
}

::grpc::Status OpenPerfServiceImpl::AnalyzeAccessibility(::grpc::ServerContext*,
                                                         const AnalyzeAccessibilityRequest* request,
                                                         AnalyzeAccessibilityResponse* response) {
    const auto& pageId = request->page_id();
    if (pageId.empty()) {
        return ::grpc::Status(::grpc::StatusCode::INVALID_ARGUMENT, "page_id is required");
    }

    auto issues = engine_.analyzeAccessibility(pageId);
    for (const auto& issue : issues) {
        auto* out = response->add_issues();
        out->set_code(issue.code);
        out->set_message(issue.message);
        out->set_node_id(issue.nodeId);

        switch (issue.severity) {
            case openperf::Severity::Info:
                out->set_severity(openperf_rpc::SEVERITY_INFO);
                break;
            case openperf::Severity::Warning:
                out->set_severity(openperf_rpc::SEVERITY_WARNING);
                break;
            case openperf::Severity::Error:
                out->set_severity(openperf_rpc::SEVERITY_ERROR);
                break;
        }
    }

    return ::grpc::Status::OK;
}

::grpc::Status OpenPerfServiceImpl::GetMetrics(::grpc::ServerContext*,
                                               const GetMetricsRequest*,
                                               GetMetricsResponse* response) {
    auto samples = engine_.getMetrics();
    for (const auto& s : samples) {
        auto* out = response->add_samples();
        out->set_name(s.name);
        out->set_value(s.value);

        auto ms = std::chrono::time_point_cast<std::chrono::milliseconds>(s.timestamp)
                      .time_since_epoch()
                      .count();
        out->set_timestamp_unix_ms(ms);
    }

    return ::grpc::Status::OK;
}

//conversion helpers

openperf::Page OpenPerfServiceImpl::fromProto(const openperf_rpc::Page& protoPage) {
    openperf::Page page;
    page.id = protoPage.id();
    page.url = protoPage.url();
    if (protoPage.has_root()) {
        page.root = fromProto(protoPage.root());
    }
    return page;
}

std::shared_ptr<openperf::Node> OpenPerfServiceImpl::fromProto(const openperf_rpc::Node& protoNode) {
    auto node = std::make_shared<openperf::Node>();
    node->id = protoNode.id();
    node->tag = protoNode.tag();
    node->text = protoNode.text();
    node->role = protoNode.role();
    node->ariaLabel = protoNode.aria_label();
    node->isInteractive = protoNode.is_interactive();
    node->children.reserve(protoNode.children_size());
    for (const auto& childProto : protoNode.children()) {
        node->children.push_back(fromProto(childProto));
    }
    return node;
}

namespace {
    std::atomic<std::uint64_t> g_pageCounter{0};
}

std::string OpenPerfServiceImpl::ensurePageId(openperf::Page& page) {
    if (page.id.empty()) {
        auto idNum = g_pageCounter.fetch_add(1, std::memory_order_relaxed);
        page.id = "page-" + std::to_string(idNum);
    }
    return page.id;
}