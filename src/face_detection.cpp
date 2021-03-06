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