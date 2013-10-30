#ifndef __FixedDelayPatch_hpp__
#define __FixedDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#define REQUEST_BUFFER_SIZE 262144

class FixedDelayPatch : public Patch {
private:
  CircularBuffer delayBuffer;
public:
  FixedDelayPatch() {
    AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
    delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
    registerParameter(PARAMETER_A, "Feedback");
    registerParameter(PARAMETER_B, "Mix");
  }
  void processAudio(AudioBuffer &buffer) {
    float* x = buffer.getSamples(0);
    float feedback = getParameterValue(PARAMETER_A);
    float mix = getParameterValue(PARAMETER_B);
    for(int n = 0; n < buffer.getSize(); n++){
      x[n] = delayBuffer.tail()*mix + x[n]*(1.0f-mix);
      delayBuffer.write(feedback * x[n]);
    }
  }
};

#endif   // __FixedDelayPatch_hpp__
