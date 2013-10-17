#ifndef ___FlangerPatch_hpp__
#define __FlangerPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#include "math.h"

// #define FLANGER_DELAY_BUFFER_LENGTH 1024 //must be power of 2

class FlangerPatch : public Patch {
public:
  FlangerPatch(){
    registerParameter(PARAMETER_A, "Rate", "Phaser speed");
    registerParameter(PARAMETER_B, "Depth", "Depth of modulation");
    registerParameter(PARAMETER_C, "Feedback", "Amount of feedback");
    registerParameter(PARAMETER_D, "Dry/Wet", "Wet / Dry mix");
  }

  double sampleRate;
  float rate, depth, wetDry, feedback, phase;
  unsigned int delaySamples;    
//   CircularBuffer<float, FLANGER_DELAY_BUFFER_LENGTH> delayBuffer;    
    
  float modulate(float rate) {    
    float output;        
    if ( phase >= 1.0 ) phase -= 1.0;
    phase += (1./(7200/(rate)));  //scaleable rate for AM!
    output = sin(phase*(2*M_PI));    //sine function        
    return output;
  };

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
    float y;        
    rate     = getParameterValue(PARAMETER_A);
    depth    = getParameterValue(PARAMETER_B);
    feedback = getParameterValue(PARAMETER_C);
    wetDry   = getParameterValue(PARAMETER_D);    
    unsigned int delaySamples;    
    rate *= 0.1f;
      
      for (int ch = 0; ch<buffer.getChannels(); ++ch) {
          
          for (int i = 0 ; i < size; i++) {
              
              float* buf = buffer.getSamples(ch);
              
              delaySamples = (depth * modulate(rate)) * (DELAY_BUFFER_LENGTH-1);
              y = buf[i] + feedback * delayBuffer.read(delaySamples);
              buf[i] = buf[i] * (1.f - wetDry)+ wetDry * y;
              delayBuffer.write(buf[i]);
          }
      }
  }
    
};


#endif /* __FlangerPatch_hpp__ */
