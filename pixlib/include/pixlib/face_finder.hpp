#include <librealsense2/rs.hpp>
#include <memory>

namespace Pixlib {
	class FaceFinder
	{
	public:
		FaceFinder();

		void tick();

	private:
    rs2::context realsense_context;
		std::shared_ptr<rs2::pipeline> pipe;

    bool started;

    void update_pipe();
  };
}
