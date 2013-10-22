#ifndef __WaveshaperPatch_hpp__
#define __WaveshaperPatch_hpp__

#include "StompBox.h"

class WaveshaperPatch : public Patch {
public:
  WaveshaperPatch(){
    registerParameter(PARAMETER_A, "Drive");
    registerParameter(PARAMETER_B, "Shape");
    registerParameter(PARAMETER_C, "Gain");
  }
  void processAudio(AudioBuffer& buffer){
    float drive = 1+ getParameterValue(PARAMETER_A) * 30 ; // get input drive value
    float gain = getParameterValue(PARAMETER_C) / 2.0 ;  // get output gain value
    
    int size = buffer.getSize();
    for(int ch=0; ch<buffer.getChannels(); ++ch){
      float* x = buffer.getSamples(ch);
      for(int i=0; i<size; i++)
	x[i] = gain*clip(nonLinear((x[i])*drive)); // process each sample
    }
  }    
    
  float nonLinear(float x){ 		// Waveshaper curve
    float y;
    int s = getParameterValue(PARAMETER_B)*3; // shape      
    switch (s) {
    case 0:
      y = 2*x*x - 1; // Chebyshef order 2
      break;
    case 1:
      y = (4*x*x - 3)*x; // Chebyshef order 3
      break;
    case 2:
      y = 8*x*x*(x*x-1) + 1; // Chebyshef order 4
      break;
    case 3:
      y = x*(16*x*x*(x*x-1.25)+5); // Chebyshef order 5
      break;
    }
    return y;
  }
    
  float clip(float x){ 		// Limiter
    if (x<-1)
      return -1;
    else if (x>1)
      return 1;
    else
      return x;
  }
    
    
};

#endif // __WaveshaperPatch_hpp__
