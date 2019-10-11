#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "opencv2/objdetect.hpp"
#include <chrono>

#include <pixlib/face_finder.hpp>

void draw_rectangle(cv::Mat& image, cv::Rect rect, cv::Scalar color = cv::Scalar(0,255,0)) {
  int x1 = (int)(rect.x);
  int y1 = (int)(rect.y);
  int x2 = (int)((rect.x + rect.width));
  int y2 = (int)((rect.y + rect.height));

  //

  cv::rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), color, 5, 4);
}


  class FaceTrackerRecorder : public Pixlib::AbstractFaceTracker
  {
  public:

    virtual Pixlib::TrackedFace detect(const cv::Mat& frame, const cv::Mat& depth_frame) {
      static int i = 1;
      i++;
      Pixlib::TrackedFace previous_tracking;

      previous_tracking.scoping = cv::Rect(
        0,0,
        frame.cols, frame.rows);
      previous_tracking.original_frame = frame;
      previous_tracking.original_depth = depth_frame;


      faces = face_detect.detect(frame);

      if (faces.size() > 0) {
        {
          fprintf(stderr, "writing color\n");
          char filename[255];
          snprintf(filename, sizeof(filename) - 1, "training/color/image_%d.png", i);
          fprintf(stderr, "writing color: %s\n", filename);
          cv::imwrite(filename, frame);
          fprintf(stderr, "wrote color\n");
        }

        {
          fprintf(stderr, "writing depth\n");
          char filename[255];
          snprintf(filename, sizeof(filename) - 1, "training/depth/image_%d.png", i);
          cv::imwrite(filename, depth_frame);
          fprintf(stderr, "wrote color\n");
        }        

      }
      return previous_tracking;
    }

    std::vector<cv::Rect> faces;

  private:
    Pixlib::FaceDetectDlibMMOD face_detect;
    //dlib::correlation_tracker tracker;
  };


int main( int argc, const char** argv )
{
  int i = 1;
  Pixlib::RealsenseTracker tracker;
  FaceTrackerRecorder face_tracker;
  cv::namedWindow("Full Frame", cv::WINDOW_FULLSCREEN);
  while(i++)
  {
    glm::vec3 face;
    tracker.tick(face_tracker, face);

    int frameHeight = tracker.tracked_face.original_frame.rows;
    int frameWidth = tracker.tracked_face.original_frame.cols;

    if (frameHeight > 0 && frameWidth > 0) {
      //fprintf(stderr, "%d x %d\n", frameWidth, frameHeight);
        for (cv::Rect face : face_tracker.faces) {
          draw_rectangle(tracker.tracked_face.original_frame, face);
        }

        //dlib::cv_image<dlib::uint16>    dlibIm(tracker.tracked_face.original_frame);
        //draw_rectangle(tracker.image_matrix, tracker.tracked_face.scoping, cv::Scalar(255,255,0));
    
      putText(tracker.tracked_face.original_frame, cv::format("%d x %d - % 4.2fms", frameHeight, frameWidth, tracker.tracked_face.timer.duration()*1000), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 4);
      fprintf(stderr, "%s: % 3.2fms ( % 3.2fms )\n", tracker.tracked_face.has_face ? " TRUE" : "FALSE", tracker.tracked_face.timer.duration()*1000, tracker.timer.duration()*1000);
      cv::imshow( "Full Frame", tracker.tracked_face.original_frame);
      //cv::imshow( "Face Detection", tracker.image_matrix );



      int k = cv::waitKey(5);
      if(k == 27) {
        cv::destroyAllWindows();
        break;
      }
    }
  }
}
