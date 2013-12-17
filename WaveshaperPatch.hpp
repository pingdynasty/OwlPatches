////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 
 
 LICENSE:
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */


/* created by the OWL team 2013 */


////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __WaveshaperPatch_hpp__
#define __WaveshaperPatch_hpp__

#include "StompBox.h"

class WaveshaperPatch : public Patch {
public:
  WaveshaperPatch(){
    registerParameter(PARAMETER_A, "Drive");
    registerParameter(PARAMETER_B, "Shape");
    registerParameter(PARAMETER_C, "Gain");
    registerParameter(PARAMETER_D, "");
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
