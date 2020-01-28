#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "opencv2/objdetect.hpp"
#include <chrono>

#include <pixsense/face_finder.hpp>

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
void draw_rectangle(cv::Mat& image, cv::Rect rect, cv::Scalar color = cv::Scalar(0,255,0)) {
  int x1 = (int)(rect.x);
  int y1 = (int)(rect.y);
  int x2 = (int)((rect.x + rect.width));
  int y2 = (int)((rect.y + rect.height));

  //

  cv::rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), color, 5, 4);
}

int main( int argc, const char** argv )
{
  LocationClient location_client(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));

  Pixsense::FaceTracker face_tracker;
  Pixsense::RealsenseTracker tracker;
  cv::namedWindow("Full Frame", cv::WINDOW_FULLSCREEN);
  while(1)
  {
    glm::vec3 face;
    tracker.tick(face_tracker, face);

    int frameHeight = face_tracker.scoped_resized_frame.rows;
    int frameWidth = face_tracker.scoped_resized_frame.cols;

    if (frameHeight > 0 && frameWidth > 0) {
      //fprintf(stderr, "%d x %d\n", frameWidth, frameHeight);
      if (tracker.tracked_face.is_tracking())
      {


        bool reply = location_client.send_location(face.x, face.y, face.z);
        if (tracker.tracked_face.get_has_face()) {
          draw_rectangle(face_tracker.scoped_resized_frame, face_tracker.scoped_resized_face);
        }
        //draw_rectangle(tracker.image_matrix, tracker.tracked_face.scoping, cv::Scalar(255,255,0));
      }

      putText(face_tracker.scoped_resized_frame, cv::format("%d x %d - % 4.2fms", frameHeight, frameWidth, tracker.tracked_face.timer.duration()*1000), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 4);
      fprintf(stderr, "%s: % 3.2fms ( % 3.2fms, % 3.2fms )\n", tracker.tracked_face.get_has_face() ? " TRUE" : "FALSE", tracker.timer.duration()*1000, tracker.tracked_face.timer.duration()*1000, tracker.timer.duration()*1000 - tracker.tracked_face.timer.duration()*1000);
      cv::imshow( "Full Frame", face_tracker.scoped_resized_frame);
      //cv::imshow( "Face Detection", tracker.image_matrix );

      int k = cv::waitKey(5);
      if(k == 27) {
        cv::destroyAllWindows();
        break;
      }
    }
  }
}