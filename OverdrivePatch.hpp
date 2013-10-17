#ifndef __OverdrivePatch_hpp__
#define __OverdrivePatch_hpp__

#include "StompBox.h"

class OverdrivePatch : public Patch {
public:
  OverdrivePatch(){
    registerParameter(PARAMETER_A, "Drive");
    registerParameter(PARAMETER_B, "Offset");
    registerParameter(PARAMETER_D, "Gain");
  }
  void processAudio(AudioBuffer &buffer){
      
    float drive = getParameterValue(PARAMETER_A);     // get input drive value
    float offset = getParameterValue(PARAMETER_B); 	  // get offset value
    float gain = getParameterValue(PARAMETER_D);      // get output gain value
    offset /= 10;
    drive += 0.03;
    drive *= 40;
    gain/= 2;
      
    int size = buffer.getSize();
      
      for (int ch = 0; ch<buffer.getChannels(); ++ch) {     //for each channel
          float* buf = buffer.getSamples(ch);

          for (int i = 0; i < size; ++i) {                  //process each sample
              buf[i] = gain*nonLinear((buf[i]+offset)*drive);
          }
      }
  }
    
  float nonLinear(float x){ 		// Overdrive curve
    if (x<-3)
      return -1;
    else if (x>3)
      return 1;
    else
      return x * ( 27 + x*x ) / ( 27 + 9*x*x );
  }
};

#endif // __OverdrivePatch_hpp__
