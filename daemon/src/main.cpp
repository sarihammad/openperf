// daemon/src/main.cpp
#include <grpcpp/grpcpp.h>
#include <iostream>
#include <memory>
#include <string>

#include "openperf/engine.hpp"
#include "service_impl.hpp"

int main(int argc, char** argv) {
    std::string address("0.0.0.0:50051");
    if (argc > 1)
        address = argv[1]; // allow overriding listen address

    openperf::Engine engine;
    engine.start();

    OpenPerfServiceImpl service(engine);

    grpc::ServerBuilder builder;
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);

    std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
    std::cout << "OpenPerf daemon listening on " << address << std::endl;

    server->Wait(); // block

    engine.stop();
    return 0;
}