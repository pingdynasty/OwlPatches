#ifndef __GainPatch_hpp__
#define __GainPatch_hpp__

#include "StompBox.h"

class GainPatch : public Patch {
public:
  GainPatch(){
    registerParameter(PARAMETER_A, "A");
    registerParameter(PARAMETER_B, "B");
    registerParameter(PARAMETER_C, "C");
    registerParameter(PARAMETER_D, "D");
    registerParameter(PARAMETER_E, "E");
  }
  void processAudio(AudioBuffer &buffer){
    float gain = getParameterValue(PARAMETER_A);
      
    int size = buffer.getSize();
      
        for(int ch=0; ch<buffer.getChannels(); ++ch){
            
          float* buf = buffer.getSamples(ch);
          for(int i=0; i<size; ++i) buf[i] = gain*buf[i];
        }
  }
};

#endif // __GainPatch_hpp__
