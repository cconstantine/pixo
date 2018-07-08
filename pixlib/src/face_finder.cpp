#include <pixlib/face_finder.hpp>


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

  void FaceFinder::tick() {
    rs2::frameset frames;

    if (started && pipe->poll_for_frames(&frames)) {
      fprintf(stderr, "Frame\n");
      rs2::depth_frame depths = frames.get_depth_frame();

      fprintf(stderr, "center distance: %f\n",
        depths.get_distance(depths.get_width() / 2,
                            depths.get_height() / 2));
    }
  }

  void FaceFinder::update_pipe() {
    size_t device_count = realsense_context.query_devices().size();
    if(!started && device_count > 0) {
      pipe->start();
      started = true;
    } else if (started && device_count == 0) {
      pipe->stop();
      started = false;
    }

  }
}