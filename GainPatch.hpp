#ifndef __GainPatch_h__
#define __GainPatch_h__

#include "StompBox.h"

class GainPatch : public Patch {
public:
  GainPatch(){
    registerParameter(PARAMETER_A, "Gain");    
  }
  void processAudio(AudioBuffer &buffer){
    float gain = getParameterValue(PARAMETER_A);
    int size = buffer.getSize();
    for(int ch=0; ch<buffer.getChannels(); ++ch){
      float* buf = buffer.getSamples(ch);
      for(int i=0; i<size; ++i)
	buf[i] = gain*buf[i];
    }
  }
};

#endif // __GainPatch_h__
