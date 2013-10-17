#ifndef __SimpleDelayPatch_hpp__
#define __SimpleDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

class SimpleDelayPatch : public Patch {
private:
  float delayTime, feedback, wetDry;

public:
  void processAudio(AudioBuffer &buffer) {                
    delayTime = getParameterValue(PARAMETER_A); // delay time
    feedback  = getParameterValue(PARAMETER_B); // delay feedback
    wetDry    = getParameterValue(PARAMETER_D); // wet/dry balance

    const int size = buffer.getSize();           // samples in block
    const float delaySamples = delayTime * (DELAY_BUFFER_LENGTH-1);        
    float y;
    float* x = buffer.getSamples(0);
    for (int n = 0; n < size; n++){             // for each sample            
      y = x[n] + feedback * delayBuffer.read(delaySamples);
      x[n] = wetDry * y + (1.f - wetDry) * x[n];  // crossfade for wet/dry balance
      delayBuffer.write(x[n]);
    }
  }
};

#endif   // __SimpleDelayPatch_hpp__
