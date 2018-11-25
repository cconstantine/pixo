#include <librealsense2/rs.hpp>
#include <memory>
#include <glm/glm.hpp>
#include <thread>
#include <pixlib/timer.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <dlib/dnn.h>
#include <dlib/data_io.h>

namespace Pixlib {

  class FaceDetectDlibMMOD {
  public:
    FaceDetectDlibMMOD();

    virtual std::vector<dlib::mmod_rect> detect(cv::Mat &frame);
  private:
    template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;
    template <long num_filters, typename SUBNET> using con5  = dlib::con<num_filters,5,5,1,1,SUBNET>;

    template <typename SUBNET> using downsampler  = dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16,SUBNET>>>>>>>>>;
    template <typename SUBNET> using rcon5  = dlib::relu<dlib::affine<con5<45,SUBNET>>>;

    using net_type = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;
    net_type mmodFaceDetector;
  };


	class FaceFinder
	{
	public:
		FaceFinder();
		~FaceFinder();

		int tick(glm::vec3 &face_location);

		glm::vec3 face;
		int faces_found;

    Timer timer;
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

FaceDetectDlibMMOD face_detect;
  };
}
