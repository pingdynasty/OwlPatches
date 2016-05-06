#ifndef __TempoSyncedPingPongDelayPatch_hpp__
#define __TempoSyncedPingPongDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#include "TapTempo.hpp"
#include "BiquadFilter.hpp"

#if 0
static const int RATIOS_COUNT = 9;
static const float ratios[RATIOS_COUNT] = { 1.0/4, 
					    1.0/3, 
					    1.0/2, 
					    3.0/4, 
					    1.0, 
					    3.0/2, 
					    2.0,
					    3.0, 
					    4.0 };
#endif
class TempoSyncedPingPongDelayPatch : public Patch {
private:
  static const int TRIGGER_LIMIT = (1<<16);
  CircularBuffer* delayBufferL;
  CircularBuffer* delayBufferR;
  int delayL, delayR;
  TapTempo<TRIGGER_LIMIT> tempo;
  StereoBiquadFilter* highpass;
  StereoBiquadFilter* lowpass;
public:
  TempoSyncedPingPongDelayPatch() : 
    delayL(0), delayR(0), tempo(getSampleRate()*60/120) {
    registerParameter(PARAMETER_A, "Tempo");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "Ratio");
    registerParameter(PARAMETER_D, "Dry/Wet");
    delayBufferL = CircularBuffer::create(TRIGGER_LIMIT);
    delayBufferR = CircularBuffer::create(TRIGGER_LIMIT);
    highpass = StereoBiquadFilter::create(1);
    highpass->setHighPass(40/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q); // dc filter
    lowpass = StereoBiquadFilter::create(1);
    lowpass->setLowPass(8000/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q);
  }
  ~TempoSyncedPingPongDelayPatch(){
    CircularBuffer::destroy(delayBufferL);
    CircularBuffer::destroy(delayBufferR);
    StereoBiquadFilter::destroy(highpass);
    StereoBiquadFilter::destroy(lowpass);
  }
  float delayTime(int ratio){
    float time = tempo.getPeriod() * ratios[ratio];
    time = max(0.01, min(1.0, time));
    return time;
  }
  void processAudio(AudioBuffer& buffer){
    int speed = getParameterValue(PARAMETER_A)*4096;
    float feedback = getParameterValue(PARAMETER_B);
    int ratio = (int)(getParameterValue(PARAMETER_C) * RATIOS_COUNT);
    int size = buffer.getSize();
    tempo.trigger(isButtonPressed(PUSHBUTTON));
    //    tempo.trigger(isButtonPressed(PUSHBUTTON), getSamplesSinceButtonPressed(PUSHBUTTON));
    tempo.clock(size);
    tempo.setSpeed(speed);
    float time = delayTime(ratio);
    int newDelayL = time*(delayBufferL->getSize()-1);
    int newDelayR = time*(delayBufferR->getSize()-1);
    float wet = getParameterValue(PARAMETER_D);
    float dry = 1.0-wet;
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    highpass->process(buffer);
    for(int n=0; n<size; n++){
      float x1 = n/(float)size;
      float x0 = 1.0-x1;
      float ldly = delayBufferL->read(delayL)*x0 + delayBufferL->read(newDelayL)*x1;
      float rdly = delayBufferR->read(delayR)*x0 + delayBufferR->read(newDelayR)*x1;
      // ping pong
      delayBufferR->write(feedback*ldly + left[n]);
      delayBufferL->write(feedback*rdly + right[n]);
      left[n] = ldly*wet + left[n]*dry;
      right[n] = rdly*wet + right[n]*dry;
    }
    lowpass->process(buffer);
    delayL = newDelayL;
    delayR = newDelayR;
  }
};

#endif   // __TempoSyncedPingPongDelayPatch_hpp__
