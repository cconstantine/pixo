/*
 *
 * Copyright 2015 gRPC authors.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#include <iostream>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>
#include "pixrpc.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;
using pixrpc::LocationRequest;
using pixrpc::LocationResponse;
using pixrpc::Pattern;

class LocationClient {
 public:
  LocationClient(std::shared_ptr<Channel> channel)
      : stub_(Pattern::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  bool send_location(float x, float y, float z) {
    // Data we are sending to the server.
    LocationRequest request;
    request.set_x(x);
    request.set_y(y);
    request.set_z(z);

    // Container for the data we expect from the server.
    LocationResponse reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->target_location(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return true;
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return false;
    }
  }

 private:
  std::unique_ptr<Pattern::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  LocationClient location_client(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));

  bool reply = location_client.send_location(1.0, 1.0, 1.0);
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}
