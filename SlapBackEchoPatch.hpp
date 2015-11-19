#ifndef __SlapBackEchoPatch_hpp__
#define __SlapBackEchoPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

/**
 * Slap Back Echo with stereo output separation.
 * 1 input, 2 output channels.
 * A slap-back or doubling delay produces a single echo, with no feedback.
 * Delay time is adjustable from 17 to 340 milliseconds. 
 * Use shorter delay times for doubling effects, longer for slap-back.
 * Set Mix to 0 for dry signal in the left output and echo in the right.
 * Set Mix to 50% for dry and wet (echo) in both channels.
 * Set Mix to 100% for echo in the left channel and dry in the left.
 */
class SlapBackEchoPatch : public Patch {
private:
  CircularBuffer* delayBuffer;
  float delay;
public:
  SlapBackEchoPatch() : delay(0) {
    registerParameter(PARAMETER_A, "Delay");
    registerParameter(PARAMETER_D, "Mix");
    delayBuffer = CircularBuffer::create(16*1024);  // 170ms
  }
  ~SlapBackEchoPatch(){
    CircularBuffer::destroy(delayBuffer);
  }
  void processAudio(AudioBuffer &buffer){
    float time = 0.05+0.95*getParameterValue(PARAMETER_A);
    float newDelay = time*(delayBuffer->getSize()-1);
    int size = buffer.getSize();
    float incr = (newDelay - delay)/size;
    float mix = getParameterValue(PARAMETER_D);
    float nmix = 1.0-mix;
    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    for(int n = 0; n < size; n++){
      float sample = left[n];
      delayBuffer->write(sample);
      float echo = delayBuffer->interpolate(delay);
      left[n] = nmix*sample + mix*echo;
      right[n] = mix*sample + nmix*echo;
      delay += incr;
    }
    delay = newDelay;
  }
};

#endif   // __SlapBackEchoPatch_hpp__
