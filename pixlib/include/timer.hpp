#pragma once

#include <chrono>

class Timer {
public:
  Timer(unsigned int smoothing = 1);

  void start();
  void end();
  float duration();

private:
  std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
  unsigned int smoothing;
  float previous_duration;
};