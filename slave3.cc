#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/masterslave.grpc.pb.h"
#else
#include "masterslave.grpc.pb.h"
#endif

using grpc::Server;
using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::ServerReaderWriter;
using grpc::Status;
using masterslave::MasterSlave;
using masterslave::Request;
using masterslave::Response;


// Logic and data behind the server's behavior.
class MasterSlaveServiceImpl final : public MasterSlave::Service {
  Status GetData(ServerContext* context, const Request* request, Response* response) override {
    return Status::OK;
  }
};

void RunServer() {
  std::string server_address("0.0.0.0:50053");
  MasterSlaveServiceImpl service;

  ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case, it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<Server> server(builder.BuildAndStart());
  std::cout << "Slave 3 listening on " << server_address << std::endl;

  // Wait for the server to shutdown. Note that some other thread must be
  // responsible for shutting down the server for this call to ever return.
  server->Wait();
}

int main(int argc, char** argv) {
  RunServer();

  return 0;
}
