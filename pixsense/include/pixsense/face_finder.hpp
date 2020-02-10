#include <librealsense2/rs.hpp>
#include <memory>
#include <glm/glm.hpp>
#include <pixlib/timer.hpp>

#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <dlib/opencv.h>
#include <dlib/image_processing.h>
#include <dlib/image_processing/frontal_face_detector.h>

#include <dlib/dnn.h>
#include <dlib/data_io.h>

#include "opencv2/objdetect.hpp"
#include <opencv2/dnn.hpp>

namespace Pixsense
{
  template <typename PYRAMID_TYPE>
  class input_grayscale_16bit_image_pyramid : public dlib::detail::input_image_pyramid<PYRAMID_TYPE>
  {
  public:
    typedef dlib::matrix<dlib::uint16> input_type;
    typedef PYRAMID_TYPE pyramid_type;

    template <typename forward_iterator>
    void to_tensor (
        forward_iterator ibegin,
        forward_iterator iend,
        dlib::resizable_tensor& data
    ) const
    {
      this->to_tensor_init(ibegin, iend, data, 1);

      const auto rects = data.annotation().get<std::vector<dlib::rectangle>>();
      if (rects.size() == 0)
          return;

      // copy the first raw image into the top part of the tiled pyramid.  We need to
      // do this for each of the input images/samples in the tensor.
      auto ptr = data.host_write_only();
      for (auto i = ibegin; i != iend; ++i)
      {
        auto& img = *i;
        ptr += rects[0].top()*data.nc();
        for (long r = 0; r < img.nr(); ++r)
        {
          auto p = ptr+rects[0].left();
          for (long c = 0; c < img.nc(); ++c)
            p[c] = (img(r,c))/256.0;
          ptr += data.nc();
        }
        ptr += data.nc()*(data.nr()-rects[0].bottom()-1);
      }

      this->create_tiled_pyramid(rects, data);
    }

    friend void serialize(const input_grayscale_16bit_image_pyramid& item, std::ostream& out)
    {
      dlib::serialize("input_grayscale_16bit_image_pyramid", out);
      dlib::serialize(item.pyramid_padding, out);
      dlib::serialize(item.pyramid_outer_padding, out);
    }

    friend void deserialize(input_grayscale_16bit_image_pyramid& item, std::istream& in)
    {
      std::string version;
      dlib::deserialize(version, in);
      if (version != "input_grayscale_16bit_image_pyramid")
          throw dlib::serialization_error("Unexpected version found while deserializing dlib::input_grayscale_16bit_image_pyramid.");
      dlib::deserialize(item.pyramid_padding, in);
      dlib::deserialize(item.pyramid_outer_padding, in);
    }

    friend std::ostream& operator<<(std::ostream& out, const input_grayscale_16bit_image_pyramid& item)
    {
      out << "input_grayscale_16bit_image_pyramid()";
      out << " pyramid_padding="<<item.pyramid_padding;
      out << " pyramid_outer_padding="<<item.pyramid_outer_padding;
      return out;
    }

    friend void to_xml(const input_grayscale_16bit_image_pyramid& item, std::ostream& out)
    {
      out << "<input_grayscale_16bit_image_pyramid"
          <<"' pyramid_padding='"<<item.pyramid_padding
          <<"' pyramid_outer_padding='"<<item.pyramid_outer_padding
          <<"'/>";
    }
  };

  template <long num_filters, typename SUBNET> using con5d = dlib::con<num_filters,5,5,2,2,SUBNET>;
  template <long num_filters, typename SUBNET> using con5  = dlib::con<num_filters,5,5,1,1,SUBNET>;

  template <typename SUBNET> using downsampler  = dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<32, dlib::relu<dlib::affine<con5d<16,SUBNET>>>>>>>>>;
  template <typename SUBNET> using rcon5  = dlib::relu<dlib::affine<con5<45,SUBNET>>>;

  using depth_net = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<Pixsense::input_grayscale_16bit_image_pyramid<dlib::pyramid_down<6>>>>>>>>;
  using image_net = dlib::loss_mmod<dlib::con<1,9,9,1,1,rcon5<rcon5<rcon5<downsampler<dlib::input_rgb_image_pyramid<dlib::pyramid_down<6>>>>>>>>;


  class  FaceDetect {
  public:
    virtual std::vector<cv::Rect> detect(const cv::Mat& frame) = 0;
  };

  class FaceDetectDepthDlibMMOD : public FaceDetect {
  public:
    FaceDetectDepthDlibMMOD();

    virtual std::vector<cv::Rect> detect(const cv::Mat& frame);
  private:
    depth_net mmodFaceDetector;
  };

  class FaceDetectRGBDlibMMOD : public FaceDetect {
  public:
    FaceDetectRGBDlibMMOD();

    virtual std::vector<cv::Rect> detect(const cv::Mat& frame);
  private:
    image_net mmodFaceDetector;
  };

  class TrackedFace {
  public:
    TrackedFace();
    TrackedFace(const TrackedFace& copy);

    void tracking(cv::Rect face);
    void not_tracking();
    bool is_tracking();
    bool get_has_face();
    void cancel_tracking();

    cv::Rect face;
    Pixlib::Timer timer;

  private:
    bool is_copy;
    bool was_tracking;
    bool has_face;
    std::chrono::time_point<std::chrono::high_resolution_clock> had_face_at;
    std::chrono::time_point<std::chrono::high_resolution_clock> started_tracking_at;
  };

  class AbstractFaceTracker {
  public:
    virtual TrackedFace detect(const cv::Mat& frame, const cv::Mat& depth_frame) = 0;
  };

  class FaceTracker : public AbstractFaceTracker
  {
  public:

    virtual TrackedFace detect(const cv::Mat& frame, const cv::Mat& depth_frame);

    cv::Rect scoped_resized_face;;
    cv::Mat scoped_resized_frame;
  private:
    FaceDetectDepthDlibMMOD face_detect;
    TrackedFace previous_tracking;
    dlib::correlation_tracker tracker;
  };

  class RealsenseTracker
  {
  public:
    RealsenseTracker();
    void tick(AbstractFaceTracker& face_detect, glm::vec3 &face_location);

    TrackedFace  tracked_face;
    cv::Mat frame;
    cv::Mat image_matrix;

    Pixlib::Timer timer;

  private:
    void update_pipe();

    rs2::context realsense_context;
    std::shared_ptr<rs2::pipeline> pipe;
    rs2::pipeline_profile pipeline_profile;

    bool started;

    static cv::Mat frame_to_mat(const rs2::frame& f);
  };

	class FaceFinder
	{
	public:
		FaceFinder();
		~FaceFinder();

    glm::vec3 face;
    TrackedFace tracked_face;
    Pixlib::Timer realsense_timer;
    bool enabled;
	private:

    bool running;
    std::shared_ptr<std::thread> reader_thread;
    void thread_method();


    RealsenseTracker face_detect;
  };
}
