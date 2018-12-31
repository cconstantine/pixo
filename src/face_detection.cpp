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

  fprintf(stderr, "Rectangle (%d, %d) x (%d, %d)\n", rect.x, rect.y, rect.width, rect.height);

  cv::rectangle(image, cv::Point(x1, y1), cv::Point(x2, y2), color, 5, 4);
}

int main( int argc, const char** argv )
{
  Pixlib::RealsenseTracker tracker;
  while(1)
  {

    std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    glm::vec3 face;
    tracker.tick(face);

    int frameHeight = tracker.image_matrix.rows;
    int frameWidth = tracker.image_matrix.cols;

    if (frameHeight > 0 && frameWidth > 0) {
      fprintf(stderr, "%d x %d\n", frameWidth, frameHeight);
      if (tracker.tracked_face.has_face)
      {
        draw_rectangle(tracker.image_matrix, tracker.tracked_face.face);
        draw_rectangle(tracker.image_matrix, tracker.tracked_face.scoping, cv::Scalar(255,255,0));
      }

      std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
      float delta = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

      putText(tracker.image_matrix, cv::format("%4.2fms", delta/1000), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.4, cv::Scalar(0, 0, 255), 4);

      cv::imshow( "Face Detection", tracker.image_matrix );
      int k = cv::waitKey(5);
      if(k == 27) {
        cv::destroyAllWindows();
        break;
      }
    }
  }
}
