#include <pixlib/face_finder.hpp>

#include <limits>

// Convert rs2::frame to cv::Mat
cv::Mat frame_to_mat(const rs2::frame& f)
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
  return sum / count;
}

#include <librealsense2/rsutil.h>

namespace Pixlib {
  FaceFinder::FaceFinder() :
   pipe(std::make_shared<rs2::pipeline>(realsense_context)),
   started(false), running(true), face_found(false)
  {
    reader_thread = std::make_shared<std::thread>(&FaceFinder::thread_method, this);
  }

  FaceFinder::~FaceFinder() {
    running = false;
    reader_thread->join();
  }

  void FaceFinder::thread_method() {
    if( !face_cascade.load("/home/cconstantine/cconstantine/workplace/opencv/data/haarcascades/haarcascade_frontalface_default.xml" ) )
    {
      fprintf(stderr, "Failed to load cascade\n");
    };

    realsense_context.set_devices_changed_callback([&](rs2::event_information& info)
    {
      update_pipe();
    });

    while (running) {
      try {
        update_pipe();
        while (running) {
          face_found = tick(face);
        }
      } catch(const std::exception& e) {
        fprintf(stderr, "FaceFinder Exception: %s\n", e.what());
      }
    }
  }

  bool FaceFinder::tick(glm::vec3 &face_location) {
    rs2::frameset frames;
    // rs2::align align(rs2_stream::RS2_STREAM_COLOR);


    if (started ) {
      frames = pipe->wait_for_frames();
      // rs2::frameset aligned_frame = align.process(frames);
      rs2::depth_frame depths = frames.get_depth_frame();
      rs2::video_frame images = frames.get_infrared_frame();

      auto image_matrix = frame_to_mat(images);

      equalizeHist( image_matrix, image_matrix );

      std::vector<cv::Rect> faces;
      std::vector<int> numDetections;
      std::vector<double> levelWeights;

      // face_cascade.detectMultiScale( image_matrix, faces);//, 1.1, 3, 0, cv::Size(), cv::Size(), true );
      face_cascade.detectMultiScale( image_matrix,   faces, 1.1, 6, 0|CV_HAAR_SCALE_IMAGE, cv::Size(30, 30) );

      //fprintf(stderr, "faces        : %d\n", faces.size());
      // fprintf(stderr, "numDetections: %d\n", numDetections.size());
      // fprintf(stderr, "levelWeights : %d\n", levelWeights.size());
      if (faces.size() == 0) {
        return false;
      }

      float min_distance = std::numeric_limits<float>::infinity();

      cv::Rect nearest_face;
      glm::vec2 nearest_face_point;
      for(int i = 0;i < faces.size(); i++) {
        glm::vec2 face = glm::vec2(faces[i].x + faces[i].width / 2, faces[i].y + faces[i].height / 2);

        float distance = glm::distance(previous_face, face);
        if (distance < min_distance) {
          nearest_face = faces[i];
          min_distance = distance;
          nearest_face_point = face;
        }
      }
      float pixel[2] = {nearest_face.x, nearest_face.y};

      float distance = rect_distance(depths, nearest_face);
      fprintf(stderr, "(%2.1f, %2.1f) distance: %f\n", pixel[0], pixel[1], distance);
      if (distance == 0) {
        return false;
      }
      previous_face = nearest_face_point;
      rs2_intrinsics intrin = images.get_profile().as<rs2::video_stream_profile>().get_intrinsics();

      rs2_deproject_pixel_to_point(&face_location[0], &intrin, pixel, distance);
      face_location.y = -face_location.y;
      //fprintf(stderr, "(%2.2f, %2.2f, %2.2f)\n", face_location.x, face_location.y, face_location.z);
      return true;
    }
    return false;
  }

  void FaceFinder::update_pipe() {
    size_t device_count = realsense_context.query_devices().size();
    if(!started && device_count > 0) {
      int width = 1280;
      int height = 720;
      int fps = 30;
      rs2::config config;
      config.enable_stream(RS2_STREAM_INFRARED, 1);
      config.enable_stream(RS2_STREAM_DEPTH);
      // config.enable_stream(RS2_STREAM_INFRARED, 2, width, height, RS2_FORMAT_Y8, fps);

      // config.enable_stream(RS2_STREAM_INFRARED, 2);
      // config.enable_stream(RS2_STREAM_DEPTH, 1);

      pipeline_profile = pipe->start(config);
      started = true;
    } else if (started && device_count == 0) {
      pipe->stop();
      started = false;
    }

  }
}