#ifndef __PingPongDelayPatch_hpp__
#define __PingPongDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#include "BiquadFilter.hpp"

class PingPongDelayPatch : public Patch {
private:
  CircularBuffer* delayBufferL;
  CircularBuffer* delayBufferR;
  int delayL, delayR;
  StereoBiquadFilter* highpass;
  StereoBiquadFilter* lowpass;
public:
  PingPongDelayPatch() : delayL(0), delayR(0) {
    registerParameter(PARAMETER_A, "Ping");
    registerParameter(PARAMETER_B, "Pong");
    registerParameter(PARAMETER_C, "Feedback");
    registerParameter(PARAMETER_D, "Dry/Wet");
    delayBufferL = CircularBuffer::create(64*1024);  // 1.36s at 48kHz
    delayBufferR = CircularBuffer::create(64*1024);
    highpass = StereoBiquadFilter::create(1);
    highpass->setHighPass(40/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q); // dc filter
    lowpass = StereoBiquadFilter::create(1);
    lowpass->setLowPass(8000/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q);
  }
  ~PingPongDelayPatch(){
    CircularBuffer::destroy(delayBufferL);
    CircularBuffer::destroy(delayBufferR);
    StereoBiquadFilter::destroy(highpass);
    StereoBiquadFilter::destroy(lowpass);
  }
  void processAudio(AudioBuffer &buffer){
    float ping = 0.01+0.99*getParameterValue(PARAMETER_A);
    float pong = 0.01+0.99*getParameterValue(PARAMETER_B);
    float feedback = getParameterValue(PARAMETER_C);
    int newDelayL = ping*(delayBufferL->getSize()-1);
    int newDelayR = pong*(delayBufferR->getSize()-1);
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0-wet;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    int size = buffer.getSize();
    highpass->process(buffer);
    for(int n = 0; n < size; n++){
      float x1 = n/(float)size;
      float x0 = 1-x1;
      float ldly = delayBufferL->read(delayL)*x0 + delayBufferL->read(newDelayL)*x1;
      float rdly = delayBufferR->read(delayR)*x0 + delayBufferR->read(newDelayR)*x1;
      delayBufferL->write(feedback*rdly + left[n]);
      delayBufferR->write(feedback*ldly + right[n]);
      left[n] = ldly*wet + left[n]*dry;
      right[n] = rdly*wet + right[n]*dry;
    }
    lowpass->process(buffer);
    delayL = newDelayL;
    delayR = newDelayR;
  }
};

#endif   // __PingPongDelayPatch_hpp__
