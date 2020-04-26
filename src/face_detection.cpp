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

void draw_rectangle(cv::Mat& image, cv::Rect rect, cv::Scalar color = cv::Scalar(255,255,255)) {
  int x1 = (int)(rect.x);
  int y1 = (int)(rect.y);
  int x2 = (int)((rect.x + rect.width));
  int y2 = (int)((rect.y + rect.height));

  //

  cv::rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), color, 5, 4);
}

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
  cv::namedWindow("Full Frame", cv::WINDOW_FULLSCREEN);
  cv::Mat image_draw;
  while(1)
  {
    glm::vec3 face;
    tracker.tick(face_tracker, face);

    int frameHeight = face_tracker.scoped_resized_frame.rows;
    int frameWidth = face_tracker.scoped_resized_frame.cols;
    cv::normalize(face_tracker.scoped_resized_frame,face_tracker.scoped_resized_frame,0.,255.,cv::NORM_MINMAX,CV_8U);
    // cv::cvtColor(face_tracker.scoped_resized_frame, image_draw, cv::COLOR_GRAY2BGR);

    if (frameHeight > 0 && frameWidth > 0) {
      //fprintf(stderr, "%d x %d\n", frameWidth, frameHeight);
      if (tracker.tracked_face.is_tracking())
      {
        service.send_location(face.x, face.y, face.z);
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