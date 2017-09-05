#include <pixlib/timer.hpp>

namespace Pixlib {
  Timer::Timer(unsigned int smoothing) :
   start_time(std::chrono::high_resolution_clock::now()),
   smoothing(smoothing),
   previous_duration(0.0f)
  { }


  void Timer::start()
  {
    start_time = std::chrono::high_resolution_clock::now();
  }

  void Timer::end()
  {

    std::chrono::time_point<std::chrono::high_resolution_clock> now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<float> time_duration = now - start_time;
    start_time = now;

    if (previous_duration == 0.0f)
    {
      previous_duration = time_duration.count();
    }

    float duration = (previous_duration*(smoothing - 1) + time_duration.count()) / smoothing;

    previous_duration = duration;

    return;
  }

  float Timer::duration()
  {
    return previous_duration;
  }
}