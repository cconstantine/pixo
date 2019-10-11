#include <pixlib/face_finder.hpp>

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

namespace Pixlib {
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

  FaceDetectDlibMMOD::FaceDetectDlibMMOD() {
    cv::String mmodModelPath = "./pixlib/models/mmod_human_face_detector.dat";
    dlib::deserialize(mmodModelPath) >> mmodFaceDetector;
  }

  std::vector<cv::Rect> FaceDetectDlibMMOD::detect(const cv::Mat& frame) {
    int frameHeight = frame.rows;
    int frameWidth = frame.cols;

    float scale = 1.0f;

    //fprintf(stderr, "FaceDetectDlibMMOD::detect: Detecting on image %d x %d\n", frameWidth, frameHeight);

    // Convert OpenCV image format to Dlib's image format
    dlib::cv_image<dlib::bgr_pixel> dlibIm(frame);
    dlib::matrix<dlib::rgb_pixel> dlibMatrix;
    dlib::assign_image(dlibMatrix, dlibIm);

    std::vector<cv::Rect> faceRects;
    // Detect faces in the image
    for(dlib::mmod_rect rect : mmodFaceDetector(dlibMatrix)) {
      cv::Rect cv_rect(rect_to_cvrect(rect.rect));
      //fprintf(stderr, "FaceDetectDlibMMOD::detect: (%d, %d) x (%d, %d)\n", cv_rect.x, cv_rect.y, cv_rect.width, cv_rect.height);
      faceRects.push_back(cv_rect);
    }

    return faceRects;
  }

  // TrackedFace::TrackedFace(const TrackedFace& copy) :
  //  face(copy.face), has_face(copy.has_face), scoped_resized_face(copy.scoped_resized_face), scoping(copy.scoping), had_face_at(copy.had_face_at), scoped_resized_frame(copy.scoped_resized_frame)
  // { }

  TrackedFace::TrackedFace() :
   has_face(false), had_face_at(std::chrono::system_clock::from_time_t(0)), timer(2)
  { }

  bool TrackedFace::is_tracking()
  {
    const float seconds_to_track = 1.0f;
    if (has_face) {
      return true;
    }

    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> since_last_face = now - had_face_at;
    return since_last_face.count() < seconds_to_track;
  }


  TrackedFace FaceTracker::detect(const cv::Mat& frame, const cv::Mat& depth_frame) {
    //fprintf(stderr, "---------------------------------------------\n");
    previous_tracking.timer.start();
    // cv::Mat frame;
    // external_frame.copyTo(frame);
    previous_tracking.scoping = cv::Rect(
      0,0,
      frame.cols, frame.rows);
    previous_tracking.original_frame = frame;
    previous_tracking.original_depth = depth_frame;

    dlib::cv_image<dlib::bgr_pixel> dlibIm(frame);
    //dlib::cv_image<dlib::uint16>    dlibIm(depth_frame);

    float scale = 1.0F;
    if ( previous_tracking.is_tracking()) {
      tracker.update_noscale(dlibIm);

      previous_tracking.face = rect_to_cvrect(tracker.get_position());

      // fprintf(stderr, "FaceDetectDlibMMOD::detect: (%d, %d) x (%d, %d)\n",
      //  previous_tracking.face.x,     previous_tracking.face.y,
      //  previous_tracking.face.width, previous_tracking.face.height);

      previous_tracking.scoping.width  = previous_tracking.face.width  * 4;
      previous_tracking.scoping.height = previous_tracking.face.height * 4;

      previous_tracking.scoping.x = previous_tracking.face.x - previous_tracking.face.width  * 1.5;
      previous_tracking.scoping.y = previous_tracking.face.y - previous_tracking.face.height * 1.5;

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
      
      const int target_scoping = 400;
      // if (previous_tracking.scoping.width < min_scoping || previous_tracking.scoping.height < min_scoping) {
      float scaling_based_on = previous_tracking.scoping.width < previous_tracking.scoping.height ? previous_tracking.scoping.width : previous_tracking.scoping.height;

      scale = target_scoping / scaling_based_on;

    }

    //fprintf(stderr, "Scoping (%d, %d) x (%d, %d)\n", previous_tracking.scoping.x, previous_tracking.scoping.y, previous_tracking.scoping.width, previous_tracking.scoping.height);
    cv::Mat scoped_frame = frame(previous_tracking.scoping);

    //fprintf(stderr, "scaling: % 2.1f\n", scale);
    cv::resize(scoped_frame, previous_tracking.scoped_resized_frame, cv::Size(), scale, scale);

    std::vector<cv::Rect> faces = face_detect.detect(previous_tracking.scoped_resized_frame);
    
    //fprintf(stderr, "faces        : %d\n", faces.size());

  
    if(faces.size() > 0) {
      //fprintf(stderr, "Face    (%d, %d) x (%d, %d)\n", faces[0].x, faces[0].y, faces[0].width, faces[0].height);    

      previous_tracking.has_face = true;
      previous_tracking.had_face_at = std::chrono::high_resolution_clock::now();

      previous_tracking.face.x = faces[0].x/scale + previous_tracking.scoping.x;
      previous_tracking.face.y = faces[0].y/scale + previous_tracking.scoping.y;
      previous_tracking.face.width = faces[0].width/scale;
      previous_tracking.face.height = faces[0].height/scale;

      tracker.start_track(dlibIm, cvrect_to_rect(previous_tracking.face));

      previous_tracking.scoped_resized_face = faces[0];
    } else {
      previous_tracking.has_face = false;
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
//      rs2::frameset frames;
      rs2::frameset unaligned_frames;
      rs2::align align(rs2_stream::RS2_STREAM_COLOR);

      if (started ) {

        // frames = pipe->wait_for_frames();
        // rs2::video_frame images = frames.get_infrared_frame();
        // rs2::depth_frame depths = frames.get_depth_frame();
        // cv::Mat frame = RealsenseTracker::frame_to_mat(images);
        // cv::cvtColor(frame, image_matrix, cv::COLOR_GRAY2RGB);

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

        rs2_intrinsics intrin = images.get_profile().as<rs2::video_stream_profile>().get_intrinsics();

        float pixel[2] = {real_face.x + real_face.width / 2, real_face.y + real_face.height / 2 };
        rs2_deproject_pixel_to_point(&face_location[0], &intrin, pixel, distance);
        face_location.y = -face_location.y;
        
        //fprintf(stderr, "FaceFinder: Face -> (%2.2f, %2.2f, %2.2f)\n",
        //  face_location.x, face_location.y, face_location.z);

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
    fprintf(stderr, "enter update_pipe()\n");
    size_t device_count = realsense_context.query_devices().size();
    if(!started && device_count > 0) {
      fprintf(stderr, "RealsenseTracker: starting with %d devices\n", device_count);
      rs2::config config;
      config.enable_stream(RS2_STREAM_DEPTH, 1280, 720,  RS2_FORMAT_Z16, 30);
      config.enable_stream(RS2_STREAM_COLOR, 1920, 1080, RS2_FORMAT_RGB8, 30);//1920, 1080, RS2_FORMAT_RGB8, 30);
      //config.enable_stream(RS2_STREAM_INFRARED, 1);
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
    fprintf(stderr, "exit update_pipe()\n");

  }

  cv::Mat RealsenseTracker::depth_to_mat(const rs2::depth_frame& depth) {    
    const int w = depth.get_width();
    const int h = depth.get_height();
    dlib::uint16 frame_bytes[h][w];

    for(int y = 0;y< h;y++) {
      for(int x=0;x < w;x++) {
        frame_bytes[y][x] = depth.get_distance(x, y) * 6553;
      }
    }
    return cv::Mat(cv::Size(w, h), CV_16UC1, (void*)frame_bytes, cv::Mat::AUTO_STEP);
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
    FaceTracker face_tracker;
    while (running) {
      if (enabled) {
        face_detect.tick(face_tracker, face);
        tracked_face = face_detect.tracked_face;
      } else {
        tracked_face.timer.start();
        float target = 1.0f/60;
        std::this_thread::sleep_for(std::chrono::duration<float>(target));
        tracked_face.timer.end();
      }
    }
  }
}
