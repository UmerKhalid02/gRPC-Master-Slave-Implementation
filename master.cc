#include <iostream>
#include <memory>
#include <string>
#include <stdio.h>
#include <vector>
#include <iomanip>
#include <chrono>
#include <grpcpp/grpcpp.h>

#ifdef BAZEL_BUILD
#include "examples/protos/masterslave.grpc.pb.h"
#else
#include "masterslave.grpc.pb.h"
#endif

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using masterslave::MasterSlave;
using masterslave::Request;
using masterslave::Response;

class MasterSlaveClient {
 public:
  MasterSlaveClient(std::shared_ptr<Channel> channel)
      : stub_(MasterSlave::NewStub(channel)) {}

  bool StatusCheck() {
    // Data we are sending to the server.
    Request request;
    Response response;
    ClientContext context;

    // The actual RPC.
    Status status = stub_->GetData(&context, request, &response);

    // Act upon its status.
    if (status.ok()) {
      return true;
    } else {      
      return false;
    }
  }

 private:
  std::unique_ptr<MasterSlave::Stub> stub_;
};

int main(int argc, char** argv) { 
  int controlInterval = 12;
  int timeoutInterval = controlInterval/4;
  std::string slaveAddresses[4] = {"localhost:50051", "localhost:50052", "localhost:50053", "localhost:50054"};

  int i;
  while(true){
    system("clear");
    i = 0;
    while(i < 4){
      bool response;

      // loop for timeout interval - (checks if a slave becomes online in 4 seconds otherwise it will be marked unresponsive)
      for (auto start = std::chrono::steady_clock::now(), now = start; now < start + std::chrono::seconds{timeoutInterval}; now = std::chrono::steady_clock::now()) {
        MasterSlaveClient master(grpc::CreateChannel(slaveAddresses[i], grpc::InsecureChannelCredentials()));
        response = master.StatusCheck();
        if(response){
          break;
        }
      }
      std::string message = "";
      std::string slaveNo = std::to_string(i + 1);
      if(response){
          message = "Slave " + slaveNo + ": Responsive - alive and working perfectly";
      } else {
          message = "Slave " + slaveNo + ": Unresponsive - currently offline";
      }

      std::cout << message << std::endl;
      i++;
    }

    sleep(3);
    // pauses its execution for 12 seconds then checks again if the slaves are alive or not
    int secs = controlInterval;
    while(secs >= 0){
      system("clear");
      std::cout << "Next call will be in " << std::setfill('0') << std::setw(2) << secs << " seconds\n";
      secs--; 
      sleep(1);
    }
  }  
  
  return 0;
}
