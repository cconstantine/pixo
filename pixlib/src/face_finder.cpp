#include <pixlib/face_finder.hpp>

#include <librealsense2/rsutil.h>
namespace Pixlib {
  FaceFinder::FaceFinder() :
   pipe(std::make_shared<rs2::pipeline>(realsense_context)),
   started(false)
  {
    realsense_context.set_devices_changed_callback([&](rs2::event_information& info)
    {
      update_pipe();
    });
    update_pipe();
  }

  bool FaceFinder::tick(glm::vec3 &face_location) {
    rs2::frameset frames;
    // rs2::align align(rs2_stream::RS2_STREAM_COLOR);


    if (started && pipe->poll_for_frames(&frames)) {
      // rs2::frameset aligned_frame = align.process(frames);
      rs2::depth_frame depths = frames.get_depth_frame();
      rs2::video_frame images = frames.get_infrared_frame();

      float pixel[2] = {(float)(depths.get_width() / 2 ), (float)(depths.get_height() / 2)};

      float distance = depths.get_distance(pixel[0], pixel[1]);
      // fprintf(stderr, "(%2.1f, %2.1f) distance: %f\n", pixel[0], pixel[1], distance);
      if (distance == 0) {
        return false;
      }

      rs2_intrinsics intrin = images.get_profile().as<rs2::video_stream_profile>().get_intrinsics();

      float point[3] = {0.0f, 0.0f, 0.0f};

      rs2_deproject_pixel_to_point(&face_location[0], &intrin, pixel, distance);

      // fprintf(stderr, "(%2.2f, %2.2f, %2.2f)\n", face_location.x, face_location.y, face_location.z);
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