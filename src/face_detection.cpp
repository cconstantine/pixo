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
#include <sys/stat.h>

cv::Rect rect_to_cvrect(dlib::rectangle rect) {
  return cv::Rect(
          rect.left(),
          rect.top(),
          (rect.right() - rect.left()),
          (rect.bottom() - rect.top())
        );
}

dlib::rectangle cvrect_to_rect(cv::Rect rect) {
  return dlib::rectangle(
          rect.x,
          rect.y,
          (rect.x + rect.width),
          (rect.y + rect.height)
        );
}

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
    previous_tracking.timer.start();

    previous_tracking.scoping = cv::Rect(
      0,0,
      frame.cols, frame.rows);
    previous_tracking.original_frame = frame.clone();
    previous_tracking.original_depth = depth_frame.clone();


    faces = face_detect.detect(frame);

    previous_tracking.timer.end();
    return previous_tracking;
  }

  std::vector<cv::Rect> faces;

private:
  Pixlib::FaceDetectDlibMMOD face_detect;
  Pixlib::TrackedFace previous_tracking;
};

class DatasetManager {
private:
  dlib::image_dataset_metadata::dataset dataset;
  std::string root_dir;
  std::string dataset_name;
  std::string images_dir;

  bool ensure_directory(const char* dirname) {
    DIR* dir = opendir(dirname);
    if (dir) {
      closedir(dir);
      return true;
    } else {
      return mkdir(dirname, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
    }
  }

public:
  DatasetManager(const std::string& root_dir, const std::string& dataset_name) 
  : root_dir(root_dir), dataset_name(dataset_name) {
    images_dir = root_dir + "/" + dataset_name;

    ensure_directory(root_dir.c_str());
    ensure_directory((images_dir).c_str());

    try {
      dlib::image_dataset_metadata::load_image_dataset_metadata(dataset, images_dir +".xml");
    } catch(dlib::error) { }
  }

  void addImage(const cv::Mat& image, dlib::image_dataset_metadata::image image_metadata) {
    int i = dataset.images.size() + 1;
    char filename[255];
    snprintf(filename, sizeof(filename) - 1, "%s/image_%04d.png", dataset_name.c_str(), i);
    fprintf(stderr, "writing: %s\n", (root_dir + "/" + filename).c_str());
    cv::imwrite(                      root_dir + "/" + filename, image);
    image_metadata.filename = std::string(filename);
    dataset.images.push_back(image_metadata);
  }

  void save() {
    dlib::image_dataset_metadata::save_image_dataset_metadata(dataset, images_dir +".xml");
  }
};


int main( int argc, const char** argv )
{
  if (argc != 2) {
    fprintf(stderr, "Usage: face_detection <datadir>\n");
    exit(1);
  }
  std::string directory(argv[1]);
  
  DatasetManager color_dataset(directory, "color");
  DatasetManager depth_dataset(directory, "depth");

  Pixlib::RealsenseTracker tracker;
  FaceTrackerRecorder face_tracker;
  cv::namedWindow("Full Frame", cv::WINDOW_FULLSCREEN);
  while(true)
  {
    glm::vec3 face;
    tracker.tick(face_tracker, face);

    int frameHeight = tracker.tracked_face.original_frame.rows;
    int frameWidth = tracker.tracked_face.original_frame.cols;

    if (frameHeight > 0 && frameWidth > 0) {
      dlib::image_dataset_metadata::image image;
      //fprintf(stderr, "%d x %d\n", frameWidth, frameHeight);
      for (cv::Rect face : face_tracker.faces) {
        image.boxes.push_back(dlib::image_dataset_metadata::box(cvrect_to_rect(face)));
        draw_rectangle(tracker.tracked_face.original_frame, face);
      }

      if (face_tracker.faces.size() > 0) {
        color_dataset.addImage(tracker.tracked_face.original_frame, image);
        depth_dataset.addImage(tracker.tracked_face.original_depth, image);
      }
    
      putText(tracker.tracked_face.original_frame, cv::format("%d x %d - % 4.2fms", frameHeight, frameWidth, tracker.tracked_face.timer.duration()*1000), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.0, cv::Scalar(0, 0, 255), 4);
      fprintf(stderr, "%s: % 3.2fms ( % 3.2fms )\n", face_tracker.faces.size() ? " TRUE" : "FALSE", tracker.tracked_face.timer.duration()*1000, tracker.timer.duration()*1000);
      cv::imshow( "Full Frame", tracker.tracked_face.original_frame);
      //cv::imshow( "Face Detection", tracker.image_matrix );



      int k = cv::waitKey(5);
      if(k == 27) {
        cv::destroyAllWindows();
        break;
      }
    }
  }
  color_dataset.save();
  depth_dataset.save();
}
