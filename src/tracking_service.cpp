#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>

#include <pixsense/face_finder.hpp>

#include <grpcpp/grpcpp.h>
#include "pixrpc.grpc.pb.h"

// Logic and data behind the server's behavior.
class TrackingServiceImpl final : public pixrpc::Tracking::Service {
public:
  void send_location(float x, float y, float z) {

    {
      std::lock_guard<std::mutex> lk(m);

      current_location.set_x(x);
      current_location.set_y(y);
      current_location.set_z(z);
    }

    cv.notify_all();
  }

private:
  grpc::Status location_stream(
    grpc::ServerContext* context,
    const pixrpc::LocationStreamArgs* args,
    grpc::ServerWriter<pixrpc::Location>* stream
  ) override {
    while(true) {
      std::unique_lock<std::mutex> lk(m);
      cv.wait(lk);

      stream->Write(current_location);
    }
    return grpc::Status::OK;
  }

  std::mutex m;
  std::condition_variable cv;
  pixrpc::Location current_location;
};


int main( int argc, const char** argv )
{
  std::string server_address("0.0.0.0:50051");
  TrackingServiceImpl service;

  grpc::ServerBuilder builder;
  // Listen on the given address without any authentication mechanism.
  builder.AddListeningPort(server_address, grpc::InsecureServerCredentials());
  // Register "service" as the instance through which we'll communicate with
  // clients. In this case it corresponds to an *synchronous* service.
  builder.RegisterService(&service);
  // Finally assemble the server.
  std::unique_ptr<grpc::Server> server(builder.BuildAndStart());
  std::cout << "Server listening on " << server_address << std::endl;

  Pixsense::FaceTracker face_tracker;
  Pixsense::RealsenseTracker tracker;

  std::chrono::time_point<std::chrono::high_resolution_clock> last_face_at = std::chrono::high_resolution_clock::now();
  while(1)
  {
    glm::vec3 face;
    tracker.tick(face_tracker, face);

    if (tracker.tracked_face.is_tracking()) {
      last_face_at = std::chrono::high_resolution_clock::now();
      service.send_location(face.x, face.y, face.z);
    } else {
      std::chrono::duration<float> since_last_face = std::chrono::high_resolution_clock::now() - last_face_at;
      if (since_last_face.count() > 10) {
        std::this_thread::sleep_for(std::chrono::duration<float>(1));
      }
    }

  }
}