#include <pixlib/face_finder.hpp>

#include <limits>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <dlib/dnn.h>
#include <dlib/data_io.h>

#include <librealsense2/rsutil.h>

float rect_distance(const rs2::depth_frame& depth, const cv::Rect& area) {
  float sum = 0;
  int count = 0;
  for(int x = 0;x < area.width; x++) {
    for(int y = 0;y < area.height; y++) {
      float d = depth.get_distance(area.x + x, area.y + y);
      if(d > 0) {
        sum += d;
        count++;
      }
    }
  }
  if (count == 0) {
    return 0.0f;
  }

  return sum / count;
}

namespace Pixlib {

  FaceDetectDlibMMOD::FaceDetectDlibMMOD() {
    cv::String mmodModelPath = "./pixlib/models/mmod_human_face_detector.dat";
    dlib::deserialize(mmodModelPath) >> mmodFaceDetector;
  }

  std::vector<cv::Rect> FaceDetectDlibMMOD::detect(const cv::Mat& frame) {
    int frameHeight = frame.rows;
    int frameWidth = frame.cols;

    float scale = 1.0f;

    fprintf(stderr, "(%d, %d)\n", frameWidth, frameHeight);	

    // Convert OpenCV image format to Dlib's image format
    dlib::cv_image<dlib::bgr_pixel> dlibIm(frame);
    dlib::matrix<dlib::rgb_pixel> dlibMatrix;
    dlib::assign_image(dlibMatrix, dlibIm);

    std::vector<cv::Rect> faceRects;
    // Detect faces in the image
    for(dlib::mmod_rect rect : mmodFaceDetector(dlibMatrix)) {
      cv::Rect cv_rect(
          rect.rect.left(),
          rect.rect.top(),
          (rect.rect.right() - rect.rect.left()),
          (rect.rect.bottom() - rect.rect.top())
        );
      faceRects.push_back(cv_rect);
    }

    return faceRects;
  }
  TrackedFace::TrackedFace(const TrackedFace& copy) :
   face(copy.face), has_face(copy.has_face), scoping(copy.scoping)
  { }

  TrackedFace::TrackedFace() :
   has_face(false)
  { }

  TrackedFace FaceTracker::detect(const cv::Mat& frame) {
    previous_tracking.scoping = cv::Rect(
      0,0,
      frame.cols, frame.rows);

    if (previous_tracking.has_face) {
      previous_tracking.scoping.width  = previous_tracking.face.width  * 2;
      previous_tracking.scoping.height = previous_tracking.face.height * 2;

      previous_tracking.scoping.x = previous_tracking.face.x - previous_tracking.face.width  / 2;
      previous_tracking.scoping.y = previous_tracking.face.y - previous_tracking.face.height / 2;

      if (previous_tracking.scoping.x < 0) {
        previous_tracking.scoping.width = previous_tracking.scoping.width + previous_tracking.scoping.x;
        previous_tracking.scoping.x = 0;
      }
      if (previous_tracking.scoping.y < 0) {
        previous_tracking.scoping.height = previous_tracking.scoping.height + previous_tracking.scoping.y;
        previous_tracking.scoping.y = 0;
      }

      if (previous_tracking.scoping.width + previous_tracking.scoping.x > frame.cols) {
        previous_tracking.scoping.width = frame.cols - previous_tracking.scoping.x;
      }
      if (previous_tracking.scoping.height + previous_tracking.scoping.y > frame.rows) {
        previous_tracking.scoping.height = frame.rows - previous_tracking.scoping.y;
      }
    }

    std::vector<cv::Rect> faces = face_detect.detect(frame(previous_tracking.scoping));
    
    fprintf(stderr, "faces        : %d\n", faces.size());
        
    if(faces.size() > 0) {
      previous_tracking.has_face = true;
      previous_tracking.face = faces[0];
      previous_tracking.face.x = previous_tracking.face.x + previous_tracking.scoping.x;
      previous_tracking.face.y = previous_tracking.face.y + previous_tracking.scoping.y;
    } else {
      previous_tracking.has_face = false;
    }


    return previous_tracking;
  }

  RealsenseTracker::RealsenseTracker() : 
   pipe(std::make_shared<rs2::pipeline>(realsense_context)),
   started(false)
  {
    realsense_context.set_devices_changed_callback([&](rs2::event_information& info)
    {
      update_pipe();
    });   
  }

  void RealsenseTracker::tick(glm::vec3 &face_location) {
   
    try {
      rs2::frameset frames;
      //rs2::align align(rs2_stream::RS2_STREAM_DEPTH);

      if (started ) {
        frames = pipe->wait_for_frames();

        rs2::depth_frame depths = frames.get_depth_frame();
        rs2::video_frame images = frames.get_infrared_frame();
        //rs2::video_frame images = aligned_frame.get_color_frame();
        const int scale = 2;
        cv::Mat frame = RealsenseTracker::frame_to_mat(images);
        cv::Mat resized;
        cv::resize(frame, resized, cv::Size(frame.cols*scale,frame.rows*scale));
        cv::cvtColor(resized, image_matrix, cv::COLOR_GRAY2RGB);

        tracked_face = face_detect.detect(image_matrix);
        if(!tracked_face.has_face) {
          return;
        }
        cv::Rect real_face = cv::Rect(
          tracked_face.face.x / scale, tracked_face.face.y / scale,
          tracked_face.face.width / scale, tracked_face.face.height / scale);

        float distance = rect_distance(depths, real_face);

        if (distance == 0.0f) {
          return;
        }

        rs2_intrinsics intrin = images.get_profile().as<rs2::video_stream_profile>().get_intrinsics();

        float pixel[2] = {real_face.x + real_face.width / 2, real_face.y + real_face.height / 2 };
        rs2_deproject_pixel_to_point(&face_location[0], &intrin, pixel, distance);
        face_location.y = -face_location.y;
        
        fprintf(stderr, "FaceFinder: Face -> (%2.2f, %2.2f, %2.2f)\n",
          face_location.x, face_location.y, face_location.z);

      } else {
        update_pipe();
      }
    } catch(const std::exception& e) {
      fprintf(stderr, "RealsenseTracker Exception: %s\n", e.what());
      started = false;
      update_pipe();
    }
    return;
  }

  void RealsenseTracker::update_pipe() {
    fprintf(stderr, "enter update_pipe()\n");
    size_t device_count = realsense_context.query_devices().size();
    if(!started && device_count > 0) {
      fprintf(stderr, "RealsenseTracker: starting with %d devices\n", device_count);
      int width = 1280;
      int height = 720;
      int fps = 30;
      rs2::config config;
      config.enable_stream(RS2_STREAM_INFRARED, 1);
      config.enable_stream(RS2_STREAM_DEPTH);
      //config.enable_stream(RS2_STREAM_COLOR);
      // config.enable_stream(RS2_STREAM_INFRARED, 2, width, height, RS2_FORMAT_Y8, fps);

      // config.enable_stream(RS2_STREAM_INFRARED, 2);
      // config.enable_stream(RS2_STREAM_DEPTH, 1);
      try {
        pipe->stop();
      } catch(const std::exception& e) {

      }
      fprintf(stderr, "RealsenseTracker: pipe->start()\n");
      pipeline_profile = pipe->start(config);
      fprintf(stderr, "RealsenseTracker: pipeline_profile.get_device()\n");

      rs2::device selected_device = pipeline_profile.get_device();
      fprintf(stderr, "RealsenseTracker: pipeline_profile.get_device()\n");

      auto depth_sensor = selected_device.first<rs2::depth_sensor>();
      fprintf(stderr, "RealsenseTracker: selected_device.first<rs2::depth_sensor>()\n");


      if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
      {
          depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f); // Disable emitter
      }
      fprintf(stderr, "RealsenseTracker: started = true\n");

      started = true;
    } else if (started && device_count == 0) {
      fprintf(stderr, "RealsenseTracker: Stopping\n");
      started = false;
      pipe->stop();
    }
    fprintf(stderr, "exit update_pipe()\n");

  }

  // Convert rs2::frame to cv::Mat
  cv::Mat RealsenseTracker::frame_to_mat(const rs2::frame& f)
  {
      using namespace cv;
      using namespace rs2;

      auto vf = f.as<video_frame>();
      const int w = vf.get_width();
      const int h = vf.get_height();

      if (f.get_profile().format() == RS2_FORMAT_BGR8)
      {
          return Mat(Size(w, h), CV_8UC3, (void*)f.get_data(), Mat::AUTO_STEP);
      }
      else if (f.get_profile().format() == RS2_FORMAT_RGB8)
      {
          auto r = Mat(Size(w, h), CV_8UC3, (void*)f.get_data(), Mat::AUTO_STEP);
          cvtColor(r, r, CV_RGB2BGR);
          return r;
      }
      else if (f.get_profile().format() == RS2_FORMAT_Z16)
      {
          return Mat(Size(w, h), CV_16UC1, (void*)f.get_data(), Mat::AUTO_STEP);
      }
      else if (f.get_profile().format() == RS2_FORMAT_Y8)
      {
          return Mat(Size(w, h), CV_8UC1, (void*)f.get_data(), Mat::AUTO_STEP);
      }

      throw std::runtime_error("Frame format is not supported yet!");
  }

  FaceFinder::FaceFinder() :
   running(true),  timer(120), face_found(false)
  {
    reader_thread = std::make_shared<std::thread>(&FaceFinder::thread_method, this);
  }

  FaceFinder::~FaceFinder() {
    running = false;
    reader_thread->join();
  }

  void FaceFinder::thread_method() {
    while (running) {
      timer.start();
      face_detect.tick(face);
      face_found = face_detect.tracked_face.has_face;
      timer.end();
    }
  }
}
