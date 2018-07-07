#include <pixlib/face_finder.hpp>


namespace Pixlib {
  FaceFinder::FaceFinder() : pipe(std::make_shared<rs2::pipeline>())
  {
    pipe->start();
  }

  void FaceFinder::tick() {
    rs2::frameset frames;

    if (pipe->poll_for_frames(&frames)) {
      fprintf(stderr, "Frame\n");
      rs2::depth_frame depths = frames.get_depth_frame();

      fprintf(stderr, "center distance: %f\n",
        depths.get_distance(depths.get_width() / 2,
                            depths.get_height() / 2));
    }


  }
}