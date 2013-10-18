#ifndef __SimpleDelayPatch_hpp__
#define __SimpleDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#include <stdint.h>

#define REQUEST_BUFFER_SIZE 262144

class SimpleDelayPatch : public Patch {
private:
  CircularBuffer delayBuffer;
  int32_t delay;
  float time;
public:
  SimpleDelayPatch() : delay(0), time(0.0) {
    AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
    delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
  }
  void processAudio(AudioBuffer &buffer) {
    float delayTime, feedback, wetDry;
    delayTime = getParameterValue(PARAMETER_A);
    feedback  = getParameterValue(PARAMETER_B);
    wetDry    = getParameterValue(PARAMETER_D);
    int size = buffer.getSize();
    int32_t newDelay;
    if(abs(time - delayTime) > 0.01){
      newDelay = delayTime * (delayBuffer.getSize()-1);
      time = delayTime;
    }else{
      newDelay = delay;
    }
    float* x = buffer.getSamples(0);
    float y;
    for (int n = 0; n < size; n++){
//       y = buf[i] + feedback * delayBuffer.read(delay);
//       buf[i] = wetDry * y + (1.f - wetDry) * buf[i];
//       delayBuffer.write(buf[i]);
      if(newDelay - delay > 4){
	y = getDelayAverage(delay-5, 5);
	delay -= 5;
      }else if(delay - newDelay > 4){
	y = getDelayAverage(delay+5, 5);
	delay += 5;
      }else{
	y = delayBuffer.read(delay);
      }
      x[n] = wetDry * y + (1.f - wetDry) * x[n];  // crossfade for wet/dry balance
      delayBuffer.write(feedback * x[n]);
    }
  }
  float getDelayAverage(int index, int points){
    float result = delayBuffer.read(index);
    for(int i=1; i<points; ++i)
      result += delayBuffer.read(index+i);
    return result/points;
  }
};

#endif   // __SimpleDelayPatch_hpp__
