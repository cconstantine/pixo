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

class TrackingClient {
 public:
  TrackingClient(std::shared_ptr<Channel> channel)
      : stub_(pixrpc::Tracking::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  bool get_locations() {
    pixrpc::LocationStreamArgs args;
    pixrpc::Location location;
    ClientContext context;

    std::unique_ptr<grpc::ClientReader<pixrpc::Location> > reader(stub_->location_stream(&context, args));

    while (reader->Read(&location)) {
      printf("%f, %f, %f\n", location.x(), location.y(), location.z());
    }
    Status status = reader->Finish();
    std::cout << "location_stream rpc" << (status.ok() ? " succeded." : " failed.")<< std::endl;
  }

 private:
  std::unique_ptr<pixrpc::Tracking::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  TrackingClient location_client(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));

  bool reply = location_client.get_locations();
  std::cout << "Greeter received: " << reply << std::endl;

  return 0;
}
