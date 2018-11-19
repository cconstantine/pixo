#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include "opencv2/objdetect.hpp"
#include <opencv2/dnn.hpp>


#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <dlib/dnn.h>
#include <dlib/data_io.h>
#include <chrono>

class FaceDetect {
public:
  virtual std::string detect(cv::Mat &frame) = 0;
};


class FaceDetectOpenCVDNN : public FaceDetect {
public:

  FaceDetectOpenCVDNN() : net(cv::dnn::readNetFromCaffe(caffeConfigFile, caffeWeightFile))
  { }

  virtual std::string detect(cv::Mat &frame) {
    const size_t inWidth = 300;
    const size_t inHeight = 300;
    const double inScaleFactor = 1.0;
    const float confidenceThreshold = 0.7;
    const cv::Scalar meanVal(104.0, 177.0, 123.0);

    int frameHeight = frame.rows;
    int frameWidth = frame.cols;

    cv::Mat inputBlob = cv::dnn::blobFromImage(frame, inScaleFactor, cv::Size(frameWidth, frameHeight), meanVal, false, false);

    net.setInput(inputBlob, "data");
    cv::Mat detection = net.forward("detection_out");


    cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());

    for(int i = 0; i < detectionMat.rows; i++)
    {
      float confidence = detectionMat.at<float>(i, 2);

      if(confidence > confidenceThreshold)
      {
        int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frameWidth);
        int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frameHeight);
        int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frameWidth);
        int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frameHeight);

        cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0, 255, 0),2, 4);
      }
    }

    return "OpenCV DNN";
  }
private:
  const std::string caffeConfigFile = "./models/deploy.prototxt";
  const std::string caffeWeightFile = "./models/res10_300x300_ssd_iter_140000_fp16.caffemodel";

  cv::dnn::Net net;
};


class FaceDetectOpenCVHAAR : public FaceDetect {
public:

  FaceDetectOpenCVHAAR() {
    std::string faceCascadePath = "./models/haarcascade_frontalface_default.xml";

    if( !faceCascade.load( faceCascadePath ) ){
      fprintf(stderr, "--(!)Error loading face cascade\n");
    }
  }

  virtual std::string detect(cv::Mat &frame) {
    int frameHeight = frame.rows;
    int frameWidth = frame.cols;

    cv::Mat frameGray;
    cvtColor(frame, frameGray, cv::COLOR_BGR2GRAY);

    std::vector<cv::Rect> faces;
    faceCascade.detectMultiScale(frameGray, faces);

    for ( size_t i = 0; i < faces.size(); i++ )
    {
      int x1 = (int)(faces[i].x);
      int y1 = (int)(faces[i].y);
      int x2 = (int)((faces[i].x + faces[i].width));
      int y2 = (int)((faces[i].y + faces[i].height));
      cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0,255,0), (int)(frameHeight/150.0), 4);
    }
    return "OpenCV HAAR";
  }
private:
  cv::CascadeClassifier faceCascade;
};


class FaceDetectDlibHOG : public FaceDetect {
public:
  FaceDetectDlibHOG() {
    hogFaceDetector = dlib::get_frontal_face_detector();
  }

  virtual std::string detect(cv::Mat &frame) {
    int frameHeight = frame.rows;
    int frameWidth = frame.cols;

    // Convert OpenCV image format to Dlib's image format
    dlib::cv_image<dlib::bgr_pixel> dlibIm(frame);

    // Detect faces in the image
    std::vector<dlib::rectangle> faceRects = hogFaceDetector(dlibIm);

    for ( size_t i = 0; i < faceRects.size(); i++ )
    {
      int x1 = (int)(faceRects[i].left());
      int y1 = (int)(faceRects[i].top());
      int x2 = (int)(faceRects[i].right());
      int y2 = (int)(faceRects[i].bottom());
      cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0,255,0), (int)(frameHeight/150.0), 4);
    }
    return "DLIB HOG";
  }
private:
  dlib::frontal_face_detector hogFaceDetector;
};


class FaceDetectDlibMMOD : public FaceDetect {
public:
  FaceDetectDlibMMOD() {
    cv::String mmodModelPath = "./models/mmod_human_face_detector.dat";
    dlib::deserialize(mmodModelPath) >> mmodFaceDetector;
  }

  virtual std::string detect(cv::Mat &frame) {
    int frameHeight = frame.rows;
    int frameWidth = frame.cols;


    // Convert OpenCV image format to Dlib's image format
    dlib::cv_image<dlib::bgr_pixel> dlibIm(frame);
    dlib::matrix<dlib::rgb_pixel> dlibMatrix;
    dlib::assign_image(dlibMatrix, dlibIm);

    // Detect faces in the image
    std::vector<dlib::mmod_rect> faceRects = mmodFaceDetector(dlibMatrix);

    for ( size_t i = 0; i < faceRects.size(); i++ )
    {
      int x1 = (int)(faceRects[i].rect.left() );
      int y1 = (int)(faceRects[i].rect.top() );
      int x2 = (int)(faceRects[i].rect.right() );
      int y2 = (int)(faceRects[i].rect.bottom() );
      cv::rectangle(frame, cv::Point(x1, y1), cv::Point(x2, y2), cv::Scalar(0,255,0), (int)(frameHeight/150.0), 4);
    }
    return "DLIB MMOD";
  }

private:
  template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;
  template <long num_filters, typename SUBNET> using con5  = dlib::con<num_filters,5,5,1,1,SUBNET>;

  template <typename SUBNET> using downsampler  = dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16,SUBNET>>>>>>>>>;
  template <typename SUBNET> using rcon5  = dlib::relu<dlib::affine<con5<45,SUBNET>>>;

  using net_type = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;
  net_type mmodFaceDetector;
};  


int main( int argc, const char** argv )
{
  std::vector<std::shared_ptr<FaceDetect>> detectors;
  detectors.push_back(std::make_shared<FaceDetectOpenCVDNN>());
  detectors.push_back(std::make_shared<FaceDetectOpenCVHAAR>());
  detectors.push_back(std::make_shared<FaceDetectDlibHOG>());
  detectors.push_back(std::make_shared<FaceDetectDlibMMOD>());

  cv::VideoCapture source;
  if (argc == 1)
      source.open(0);
  else
      source.open(std::stoi(argv[1]));
  cv::Mat frame;
  source.set(CV_CAP_PROP_FRAME_WIDTH, 1280);
  source.set(CV_CAP_PROP_FRAME_HEIGHT, 720);
  int width = source.get(CV_CAP_PROP_FRAME_WIDTH);
  int height = source.get(CV_CAP_PROP_FRAME_HEIGHT);

  double tt_dlibMmod = 0;
  double fpsDlibMmod = 0;

  int detector_index = 3;
  while(1)
  {
      source >> frame;
      if(frame.empty())
          break;

      std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

      std::string name = detectors[detector_index]->detect(frame);

      std::chrono::steady_clock::time_point end= std::chrono::steady_clock::now();
      float delta = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();

      putText(frame, cv::format("%s %dx%d %4.2fms",name.c_str(), width, height, delta/1000), cv::Point(10, 50), cv::FONT_HERSHEY_SIMPLEX, 1.4, cv::Scalar(0, 0, 255), 4);

      cv::imshow( "Face Detection", frame );
      int k = cv::waitKey(5);
      if(k == 27) {
        cv::destroyAllWindows();
        break;
      } else if(k == 32) {
        detector_index = (detector_index + 1) % (detectors.size());
      }
    }
}
