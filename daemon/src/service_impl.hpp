#pragma once

#include "openperf/engine.hpp"
#include "openperf/page.hpp"
#include "openperf.grpc.pb.h"

#include <grpcpp/grpcpp.h>

/**
 * gRPC implementation of the OpenPerf service.
 * 
 * This is one concrete implementation of the IPC layer. The core Engine is
 * transport-agnostic, allowing alternative implementations such as:
 * - Unix domain sockets (for lower latency on same-machine IPC)
 * - Shared memory transport (for high-throughput scenarios)
 * - Named pipes (for Windows compatibility)
 * 
 * See core/include/openperf/ipc_endpoint.hpp for the abstract interface.
 */
class OpenPerfServiceImpl final : public openperf_rpc::OpenPerfService::Service {
public:
    explicit OpenPerfServiceImpl(openperf::Engine& engine);

    ::grpc::Status SubmitPage(::grpc::ServerContext* context,
                              const openperf_rpc::SubmitPageRequest* request,
                              openperf_rpc::SubmitPageResponse* response) override;

    ::grpc::Status RunRenderPipeline(::grpc::ServerContext* context,
                                     const openperf_rpc::RunRenderRequest* request,
                                     openperf_rpc::RunRenderResponse* response) override;

    ::grpc::Status AnalyzeAccessibility(::grpc::ServerContext* context,
                                        const openperf_rpc::AnalyzeAccessibilityRequest* request,
                                        openperf_rpc::AnalyzeAccessibilityResponse* response) override;

    ::grpc::Status GetMetrics(::grpc::ServerContext* context,
                              const openperf_rpc::GetMetricsRequest* request,
                              openperf_rpc::GetMetricsResponse* response) override;

private:
    openperf::Engine& engine_; // core engine stays in openperf namespace

    // helpers to convert between proto and core types
    openperf::Page fromProto(const openperf_rpc::Page& protoPage);
    std::shared_ptr<openperf::Node> fromProto(const openperf_rpc::Node& protoNode);

    std::string ensurePageId(openperf::Page& page);
};