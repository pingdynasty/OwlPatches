#ifndef ___FlangerPatch_hpp__
#define __FlangerPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#include "math.h"

#define REQUEST_BUFFER_SIZE 1024

class FlangerPatch : public Patch {
private:
  CircularBuffer delayBuffer;
    float rate, depth, wetDry, feedback, phase;
    unsigned int olddelaySamples = 0, dSamples;

public:
  FlangerPatch(){
    AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
    delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
      registerParameter(PARAMETER_A, "Rate");
      registerParameter(PARAMETER_B, "Depth");
      registerParameter(PARAMETER_C, "Feedback");
      registerParameter(PARAMETER_D, "Dry/Wet");
  }
    
    float modulate(float rate) {
        
        float output;
        
        if ( phase >= 1.0 ) phase -= 1.0;
        phase += (1./(7200/rate));
        output = sin(phase*(2*M_PI));    //sine function
        
        return output;
    };

  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
    float y;
    unsigned int delaySamples;
      
    rate     = getParameterValue(PARAMETER_A);
    depth    = getParameterValue(PARAMETER_B);
    feedback = getParameterValue(PARAMETER_C);
    wetDry   = getParameterValue(PARAMETER_D);
    rate *= 0.1f;
      
      for (int ch = 0; ch<buffer.getChannels(); ++ch) {
          
          for (int i = 0 ; i < size; i++) {
              
              float* buf = buffer.getSamples(ch);
              
              delaySamples = (depth * modulate(rate)) * (delayBuffer.getSize()-1);
              y = buf[i] + feedback * delayBuffer.read(delaySamples);
              buf[i] = buf[i] * (1.f - wetDry)+ wetDry * y;
              delayBuffer.write(buf[i]);
          }
      }
      olddelaySamples = delaySamples;
  }
    
};


#endif /* __FlangerPatch_hpp__ */
