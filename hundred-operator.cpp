#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <thread>
#include <audio>
#include <cmath>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cstdlib>
#include <ctime>
#include <iostream>

#include <random>

// This example app plays a sine wave of a given frequency for 5 seconds.

int main() {
  using namespace std::experimental;

  auto device = get_default_audio_output_device();
  if (!device)
    return 1;


  std::srand(std::time(nullptr));


  std::vector<float> frequency_hz(100);
  std::vector<float> gains(frequency_hz.size());

  std::default_random_engine freqGenerator;
  std::uniform_real_distribution<float> freqDistribution(0.1,10000.0);

  std::generate(frequency_hz.begin(), frequency_hz.end(), [=](){
    return static_cast <float> (std::rand() % 10000 * 8) / 8.0f;
  });

  std::generate(gains.begin(), gains.end(), [](){
    return static_cast <float> (std::rand()) / static_cast <float> (RAND_MAX);
  });

  std::cout << "Frequency, Gain\n";
  for (size_t i = 0; i < frequency_hz.size(); i++) {
    std::cout << frequency_hz.at(i) << ", " << gains.at(i) << "\n"; 
  }

  std::vector<float> delta(frequency_hz.size());
  std::vector<float> phase(frequency_hz.size()); 
  std::vector<float> sinFrames(frequency_hz.size()); 
  float sinValue = 0.0f;

  for (size_t i = 0; i < frequency_hz.size(); i++)
  {
    delta.at(i) = 2.0f * frequency_hz.at(i) * float(M_PI / device->get_sample_rate()); 
  }
  


  device->connect([=](audio_device&, audio_device_io<float>& io) mutable noexcept {
    if (!io.output_buffer.has_value())
      return;

    auto& out = *io.output_buffer;

    for (int frame = 0; frame < out.size_frames(); ++frame) {
      float next_sample = sinFrames.back();
      sinValue = 0.5f; // initial sinValue needs to be > 0.0f otherwise all phase deltas effectively start at 0.0f and therefore do not advance.
      for (size_t i = 0; i < frequency_hz.size(); i++) {
        sinFrames.at(i) = sinValue;
        sinFrames.at(i) *= gains.at(i);
        phase.at(i) = std::fmod(phase.at(i) + delta.at(i) * sinValue, 2.0f * static_cast<float>(M_PI)); 
        sinValue = std::sin(phase.at(i));
      }

      for (int channel = 0; channel < out.size_channels(); ++channel)
        out(frame, channel) = 0.2f * next_sample;
    }
  });

  device->start();
  std::this_thread::sleep_for(std::chrono::seconds(45));
}
