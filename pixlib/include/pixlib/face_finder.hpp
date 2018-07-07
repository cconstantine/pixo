#include <librealsense2/rs.hpp>
#include <memory>

namespace Pixlib {
	class FaceFinder
	{
	public:
		FaceFinder();

		void tick();

	private:
		std::shared_ptr<rs2::pipeline> pipe;

	};

}
