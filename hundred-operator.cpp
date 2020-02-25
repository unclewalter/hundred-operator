#ifdef _WIN32
#define _USE_MATH_DEFINES
#endif

#include <cmath>
#include <thread>
#include <audio>

#include <vector>
#include <algorithm>
#include <numeric>
#include <random>

#include <iostream>
#include <string>
#include <unordered_map>

class SinOsc 
{
private:
  float freq = 440.0f;;
  float gain = 0.5f;
  float phaseDelta = 0.0f;;
  float phase = 0.0f;
  float deltaMultiplier;
public:
   float eval(float f);
   float eval();
   void set(float f = 440.0f, float g = 0.5f, int sampleRate = 48000);
   SinOsc(float f = 440.0f, float g = 0.5f, int sampleRate = 48000);
  ~SinOsc();
};

float SinOsc::eval(float f) {
  SinOsc::freq = f;
  SinOsc::phaseDelta = SinOsc::freq * SinOsc::deltaMultiplier;
  SinOsc::phase = std::fmod(phase + SinOsc::phaseDelta, 2.0f * static_cast<float>(M_PI));
  return std::sin(SinOsc::phase);
}

float SinOsc::eval() {
  float next_sample = std::sin(SinOsc::phase); 
  SinOsc::phase += std::fmod(phase + SinOsc::phaseDelta, 2.0f * static_cast<float>(M_PI));
  return next_sample;
  // return 0.0f;
}

SinOsc::SinOsc(float f, float g, int sampleRate)
{
  SinOsc::freq = f;
  SinOsc::gain = g;
  SinOsc::phaseDelta = SinOsc::freq * SinOsc::deltaMultiplier;
  deltaMultiplier = 2.0f * float(M_PI / sampleRate);
}

void SinOsc::set(float f, float g, int sampleRate)
{
  SinOsc::freq = f;
  SinOsc::gain = g;
  SinOsc::phaseDelta = SinOsc::freq * SinOsc::deltaMultiplier;
  deltaMultiplier = 2.0f * float(M_PI / sampleRate);
}

SinOsc::~SinOsc()
{
}


int main(int argc, char *argv[]) {
  using namespace std::experimental;

  unsigned duration = 5;
  float frequencySkew = 8.0;
  float gainSkew = 1.5;
  auto device = get_default_audio_output_device();
  if (!device)
    return 1;

  if (argc > 0) {
    for (int i = 0; i < argc; i++)
    {
      switch (i)
      {
      case 1:
        duration = atoi(argv[i]);
        break;
      case 2: 
        frequencySkew = atof(argv[i]);
        break;
      case 3:
        gainSkew = atof(argv[i]);
      default:
        break;
      }
    }
    
  }

  std::vector<float> frequency_hz(100);
  std::vector<float> gains(frequency_hz.size());
  std::vector<SinOsc> sineOscillators(100);
  std::random_device rd; 
  std::mt19937 gen(rd()); 
  std::exponential_distribution<float> freqDis(frequencySkew);
  std::exponential_distribution<float> gainDis(gainSkew);

  std::cout << "Frequency, Gain\n";
  for (size_t i = 0; i < frequency_hz.size(); i++) {
    float f = freqDis(gen) * 15000.0f;
    float g = gainDis(gen);
    std::cout << f << ", " << g << "\n"; 
    sineOscillators.at(i).set(f, g, device->get_sample_rate());
  } 

  // for (size_t i = 0; i < frequency_hz.size(); i++) {
  //   std::cout << frequency_hz.at(i) << ", " << gains.at(i) << "\n"; 
  // }

  // std::vector<float> delta(frequency_hz.size());
  std::vector<float> phase(frequency_hz.size()); 
  // std::vector<float> sinFrames(frequency_hz.size()); 
  // float sinValue = 0.0f;

  // for (size_t i = 0; i < frequency_hz.size(); i++)
  // {
  //   delta.at(i) = 2.0f * frequency_hz.at(i) * float(M_PI / device->get_sample_rate()); 
  // }
  
  device->connect([=](audio_device&, audio_device_io<float>& io) mutable noexcept {
    if (!io.output_buffer.has_value())
      return;

    auto& out = *io.output_buffer;

    for (int frame = 0; frame < out.size_frames(); ++frame) {
      // sinValue = 0.5f; // initial sinValue needs to be > 0.0f otherwise all phase deltas effectively start at 0.0f and therefore do not advance.
      // for (size_t i = 0; i < frequency_hz.size(); i++) {
      //   sinFrames.at(i) = sinValue;
      //   sinFrames.at(i) *= gains.at(i);
      //   phase.at(i) = std::fmod(phase.at(i) + delta.at(i) * sinValue, 2.0f * static_cast<float>(M_PI)); 
      //   sinValue = std::sin(phase.at(i));
      // }
      float next_sample = std::accumulate(sineOscillators.begin(), sineOscillators.end(), 0.0f, [](float accum, auto osc) {
        return osc.eval() + accum;
      });


      for (int channel = 0; channel < out.size_channels(); ++channel)
        out(frame, channel) = 0.2f * next_sample;
    }
  });

  device->start();
  if(duration > 0)
  {
    std::this_thread::sleep_for(std::chrono::seconds(duration));
  } 
  else
  {
    while (true) {
      std::string command;
      std::cin >> command;

      if (command == "q") {
        break;
      }
      if (command == "s") {
        break;
      }
    }
  }
  
}
