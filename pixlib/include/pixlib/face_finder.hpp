#include <librealsense2/rs.hpp>
#include <memory>
#include <glm/glm.hpp>
#include <thread>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

namespace Pixlib {
	class FaceFinder
	{
	public:
		FaceFinder();
		~FaceFinder();

		int tick(glm::vec3 &face_location);

		glm::vec3 face;
		int faces_found;

	private:
    rs2::context realsense_context;
		std::shared_ptr<rs2::pipeline> pipe;
    rs2::pipeline_profile pipeline_profile;

    bool started;

    void update_pipe();


    cv::CascadeClassifier face_cascade;

    bool running;
    std::shared_ptr<std::thread> reader_thread;
    void thread_method();

    glm::vec2 previous_face;
  };
}
