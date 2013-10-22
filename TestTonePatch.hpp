#ifndef __TestTonePatch_hpp__
#define __TestTonePatch_hpp__

#include "StompBox.h"

class TestTonePatch : public Patch {
private:
  float leftpos, rightpos;
public:
  TestTonePatch() : leftpos(0.0f), rightpos(0.0f) {}
  void processAudio(AudioBuffer &buffer) {
    float frequency = getParameterValue(PARAMETER_A) * 10000;
    float amplitude = getParameterValue(PARAMETER_B) * 2;
    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    float linc = frequency/getSampleRate();
    float rinc = 1000/getSampleRate();
    int size = buffer.getSize();
    for(int n = 0; n<size; n++){
      left[n] = sinf(2*M_PI*leftpos) * amplitude;
      right[n] = sinf(2*M_PI*rightpos) * 0.5;
      if((leftpos += linc) > 1.0f)
	leftpos -= 1.0f;
      if((rightpos += rinc) > 1.0f)
	rightpos -= 1.0f;
    }
  }
};

#endif   // __TestTonePatch_hpp__
