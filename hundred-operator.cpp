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
#include <random>
#include <iostream>


// This app is a 100 operator FM synth. Each operator's gain and frequency are randomly generated with an exponental distribution.
// Frequencies range from DC up to 20kHz so some operators can function as LFOs. This can lead to very complex modulations between operators
// The values are dumped out in CSV format.

int main() {
  using namespace std::experimental;

  auto device = get_default_audio_output_device();
  if (!device)
    return 1;


  std::srand(std::time(nullptr));

  std::vector<float> frequency_hz(100);
  std::vector<float> gains(frequency_hz.size());

  std::random_device rd; 
  std::mt19937 gen(rd()); 
  std::exponential_distribution<float> freqDis(8.0);
  std::exponential_distribution<float> gainDis(1.5);

  for (size_t i = 0; i < frequency_hz.size(); i++) {
    frequency_hz.at(i) = freqDis(gen) * 20000.0f;
    gains.at(i) = gainDis(gen);
  } 

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
  std::this_thread::sleep_for(std::chrono::seconds(5));
}
