#include <pixsense/face_finder.hpp>

#include <limits>
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <dlib/dnn.h>
#include <dlib/data_io.h>

#include <librealsense2/rsutil.h>

float rect_distance(const rs2::depth_frame& depth, const cv::Rect& area) {
  // fprintf(stderr, "rect_distance: depth: (%d, %d)\n", depth.get_width(), depth.get_height());
  // fprintf(stderr, "rect_distance: area:  (%d, %d) x (%d, %d)\n", area.x, area.y, area.width, area.height);
  float sum = 0;
  int count = 0;
  for(int x = 0;x < area.width; x++) {
    for(int y = 0;y < area.height; y++) {
      int x1 = area.x + x;
      int y1 = area.y + y;
      if (x1 < 0 || x1 >= depth.get_width() ||
          y1 < 0 || y1 >= depth.get_height()) {
        continue;
      }
      float d = depth.get_distance(x1, y1);
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

namespace Pixsense {
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

  FaceDetectDepthDlibMMOD::FaceDetectDepthDlibMMOD() {
    // cv::String mmodModelPath = "../pixsense/models/mmod_human_face_detector.dat";

    cv::String mmodModelPath = "../pixsense/models/mmod_human_face_detector_depth.dat";
    dlib::deserialize(mmodModelPath) >> mmodFaceDetector;
  }

  std::vector<cv::Rect> FaceDetectDepthDlibMMOD::detect(const cv::Mat& frame) {
    // Convert OpenCV image format to Dlib's image format
    dlib::cv_image<dlib::uint16> dlibIm(frame);
    dlib::matrix<dlib::uint16> dlibMatrix;
    dlib::assign_image(dlibMatrix, dlibIm);

    std::vector<cv::Rect> faceRects;

    // Detect faces in the image
    for(dlib::mmod_rect rect : mmodFaceDetector(dlibMatrix)) {
      cv::Rect cv_rect(rect_to_cvrect(rect.rect));
      faceRects.push_back(cv_rect);
    }

    return faceRects;
  }

  FaceDetectRGBDlibMMOD::FaceDetectRGBDlibMMOD() {
    cv::String mmodModelPath = "../pixsense/models/mmod_human_face_detector.dat";

    // cv::String mmodModelPath = "./mmod_network.dat";
    dlib::deserialize(mmodModelPath) >> mmodFaceDetector;
  }

  std::vector<cv::Rect> FaceDetectRGBDlibMMOD::detect(const cv::Mat& frame) {
    // Convert OpenCV image format to Dlib's image format
    dlib::cv_image<dlib::bgr_pixel> dlibIm(frame);
    dlib::matrix<dlib::rgb_pixel> dlibMatrix;
    dlib::assign_image(dlibMatrix, dlibIm);

    std::vector<cv::Rect> faceRects;

    // Detect faces in the image
    for(dlib::mmod_rect rect : mmodFaceDetector(dlibMatrix)) {
      cv::Rect cv_rect(rect_to_cvrect(rect.rect));
      faceRects.push_back(cv_rect);
    }

    return faceRects;
  }

  TrackedFace::TrackedFace() :
   is_copy(false),
   has_face(false),
   was_tracking(false),
   started_tracking_at(std::chrono::system_clock::from_time_t(0)),
   had_face_at(std::chrono::system_clock::from_time_t(0)),
   timer(2)
  { }

  TrackedFace::TrackedFace(const TrackedFace& copy) :
    is_copy(true),
    face(copy.face),
    has_face(copy.has_face),
    was_tracking(copy.was_tracking),
    started_tracking_at(copy.started_tracking_at),
    had_face_at(copy.had_face_at),
    timer(copy.timer)
  { }

  bool TrackedFace::is_tracking()
  {
    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();

    std::chrono::duration<float> since_last_face = now - had_face_at;
    bool tracking = has_face || since_last_face.count() < 1.0f;

    std::chrono::duration<float> since_started_tracking = now - started_tracking_at;
    if (tracking && since_started_tracking.count() > 30.0f) {
      if (!is_copy)
        fprintf(stderr, "Face tracking timed out\n");
      cancel_tracking();
      return false;
    }
    return tracking;
  }

  bool TrackedFace::get_has_face()
  {
    return has_face;
  }

  void TrackedFace::tracking(cv::Rect face)
  {
    if(!is_tracking()) {
      if (!is_copy)
        fprintf(stderr, "Face tracking started\n");
      started_tracking_at = std::chrono::high_resolution_clock::now();
    }
    this->face = face;
    has_face = true;
    had_face_at = std::chrono::high_resolution_clock::now();
    was_tracking = true;
  }

  void TrackedFace::not_tracking()
  {
    has_face = false;
    if (was_tracking && !is_tracking()) {
      if (!is_copy)
        fprintf(stderr, "Face tracking stopped\n");
      cancel_tracking();
    }
  }

  void TrackedFace::cancel_tracking()
  {
      has_face = false;
      was_tracking = false;
      started_tracking_at = std::chrono::system_clock::from_time_t(0);
      had_face_at = std::chrono::system_clock::from_time_t(0);
  }

  TrackedFace FaceTracker::detect(const cv::Mat& frame, const cv::Mat& depth_frame) {
    previous_tracking.timer.start();

    cv::Rect scoping;
    cv::Mat original_frame;
    cv::Mat original_depth;

    scoping = cv::Rect(
      0,0,
      depth_frame.cols, depth_frame.rows);
    original_frame = frame;
    original_depth = depth_frame;

    dlib::cv_image<dlib::bgr_pixel> dlibIm(frame);
    // dlib::cv_image<dlib::uint16>    dlibIm(depth_frame);

    float scale = 1.5F;
    if ( previous_tracking.is_tracking()) {
      tracker.update_noscale(dlibIm);

      previous_tracking.face = rect_to_cvrect(tracker.get_position());


      scoping.width  = previous_tracking.face.width  * 4;
      scoping.height = previous_tracking.face.height * 4;

      scoping.x = previous_tracking.face.x - previous_tracking.face.width  * 1.5;
      scoping.y = previous_tracking.face.y - previous_tracking.face.height * 1.5;

      if (scoping.x < 0) {
        scoping.width = scoping.width + scoping.x;
        scoping.x = 0;
      }
      if (scoping.y < 0) {
        scoping.height = scoping.height + scoping.y;
        scoping.y = 0;
      }

      if (scoping.width + scoping.x > frame.cols) {
        scoping.width = frame.cols - scoping.x;
      }
      if (scoping.height + scoping.y > frame.rows) {
        scoping.height = frame.rows - scoping.y;
      }
      
      const int target_scoping = 400;
      float scaling_based_on = scoping.width < scoping.height ? scoping.width : scoping.height;

      scale = target_scoping / scaling_based_on;
    }

    cv::Mat scoped_frame = frame(scoping);
    cv::resize(scoped_frame, scoped_resized_frame, cv::Size(), scale, scale);

    std::vector<cv::Rect> faces = face_detect.detect(scoped_resized_frame);
  
    if(faces.size() > 0) {
      int selected_face = rand() % faces.size();

      cv::Rect face;
      face.x = faces[selected_face].x/scale + scoping.x;
      face.y = faces[selected_face].y/scale + scoping.y;
      face.width = faces[selected_face].width/scale;
      face.height = faces[selected_face].height/scale;

      previous_tracking.tracking(face);

      tracker.start_track(dlibIm, cvrect_to_rect(previous_tracking.face));

      scoped_resized_face = faces[selected_face];
    } else {
      previous_tracking.not_tracking();
    }

    previous_tracking.timer.end();
    
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

  void RealsenseTracker::tick(AbstractFaceTracker& face_detect, glm::vec3 &face_location) {
    timer.start();
    try {
      rs2::frameset unaligned_frames;
      rs2::align align(rs2_stream::RS2_STREAM_COLOR);

      if (started ) {
        unaligned_frames = pipe->wait_for_frames();
        rs2::frameset aligned_frames = align.process(unaligned_frames);
        rs2::video_frame images = aligned_frames.get_color_frame();
        rs2::depth_frame depths = aligned_frames.get_depth_frame();
        cv::Mat image_matrix = RealsenseTracker::frame_to_mat(images);
        cv::Mat depth_matrix = RealsenseTracker::frame_to_mat(depths);

        tracked_face = face_detect.detect(image_matrix, depth_matrix);
        if(!tracked_face.is_tracking()) {
          timer.end();
          return;
        }
        cv::Rect real_face = cv::Rect(
          tracked_face.face.x , tracked_face.face.y ,
          tracked_face.face.width , tracked_face.face.height);

        float distance = rect_distance(depths, real_face);

        if (distance == 0.0f) {
          timer.end();
          return;
        }

        rs2_intrinsics intrin = depths.get_profile().as<rs2::video_stream_profile>().get_intrinsics();

        float pixel[2] = {real_face.x + real_face.width / 2, real_face.y + real_face.height / 2 };
        rs2_deproject_pixel_to_point(&face_location[0], &intrin, pixel, distance);
        face_location.y = -face_location.y;
      } else {
        update_pipe();
      }
    } catch(const std::exception& e) {
      fprintf(stderr, "RealsenseTracker Exception: %s\n", e.what());
      started = false;
      update_pipe();
    }
    timer.end();
    return;
  }

  void RealsenseTracker::update_pipe() {
    size_t device_count = realsense_context.query_devices().size();
    if(!started && device_count > 0) {
      fprintf(stderr, "RealsenseTracker: starting with %d devices\n", device_count);
      rs2::config config;
      config.enable_stream(RS2_STREAM_DEPTH, 1280, 720,  RS2_FORMAT_Z16, 30);
      config.enable_stream(RS2_STREAM_COLOR, 1920, 1080, RS2_FORMAT_RGB8, 30);
      // config.enable_stream(RS2_STREAM_INFRARED, 2, width, height, RS2_FORMAT_Y8, fps);

      // config.enable_stream(RS2_STREAM_INFRARED, 2);
      // config.enable_stream(RS2_STREAM_DEPTH, 1);
      try {
        pipe->stop();
      } catch(const std::exception& e) {

      }
      pipeline_profile = pipe->start(config);
      rs2::device selected_device = pipeline_profile.get_device();
      auto depth_sensor = selected_device.first<rs2::depth_sensor>();
      if (depth_sensor.supports(RS2_OPTION_EMITTER_ENABLED))
      {
          depth_sensor.set_option(RS2_OPTION_EMITTER_ENABLED, 1.f); // Enable emitter
      }

      started = true;
    } else if (started && device_count == 0) {
      started = false;
      pipe->stop();
    }
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
   running(true), enabled(true)
  {
    reader_thread = std::make_shared<std::thread>(&FaceFinder::thread_method, this);
  }

  FaceFinder::~FaceFinder() {
    running = false;
    reader_thread->join();
  }

  void FaceFinder::thread_method() {
    std::chrono::time_point<std::chrono::high_resolution_clock> last_face_at = std::chrono::high_resolution_clock::now();
    FaceTracker face_tracker;
    while (running) {
      if (enabled) {
        face_detect.tick(face_tracker, face);

        if (tracked_face.is_tracking()) {
          last_face_at = std::chrono::high_resolution_clock::now();
        } else {
          std::chrono::duration<float> since_last_face = std::chrono::high_resolution_clock::now() - last_face_at;
          if (since_last_face.count() > 300) {
            std::this_thread::sleep_for(std::chrono::duration<float>(1));
          }
        }

        tracked_face = face_detect.tracked_face;
        realsense_timer = face_detect.timer;
      } else {
        tracked_face.timer.start();
        float target = 1.0f/60;
        std::this_thread::sleep_for(std::chrono::duration<float>(target));
        tracked_face.timer.end();
      }
    }
  }
}
