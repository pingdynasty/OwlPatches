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


#pragma once

#include "StompBox.h"

namespace OctaveDown {

	class LPF {
	public:
	  float z;
	  float x;
	  float y;
	  float r;
	  float c;
	  LPF() {
	    z = x = y = r = c = 0;
	  }
	#define SQRT2 1.414213562f
	#define TWOPI 6.2831853071f
	#define TWOPI_BY_SAMPLERATE 0.00014247585731f
	  void setCoeffs(float cutoff, float resonance) {
	    if(cutoff>11025) cutoff = 11025;
	      z=cos(TWOPI_BY_SAMPLERATE*cutoff);
	      c = 2 - 2*z;
	      float zzz = z-1;
	      zzz = zzz*zzz*zzz;
	      r = (SQRT2*sqrt(-zzz)+resonance*(z-1))/(resonance*(z-1));
	  }
	  // cutoff in hz/2 (min 10Hz/2), resonance 1 to 10
	  float process(float input) {
	      

	      x += (input - y)*c;
	      y += x;
	      x *= r;
	      return y; 
	  }
	};
};

#define ABS(X) (X>0?X:-X)
class OctaveDownPatch : public Patch {
public:
  OctaveDown::LPF inLpf, out1Lpf, out2Lpf;
  bool lastRect;
  bool oct1;
  bool oct2;
  bool lastOct1;
  float phase;
  OctaveDownPatch(){
    phase = 0;
    lastRect = 0;
    lastOct1 = 0;
    oct1 = 0;
    oct2 = 0;
    inLpf.setCoeffs(100, 0.5);
    out1Lpf.setCoeffs(200, 0.5);
    out2Lpf.setCoeffs(200, 0.5);
    registerParameter(PARAMETER_A, "DRY");
    registerParameter(PARAMETER_B, "Octave1");
    registerParameter(PARAMETER_C, "Octave2");
  }

  void processAudio(AudioBuffer& buffer){
    float gain = getParameterValue(PARAMETER_A);
    float o1g = getParameterValue(PARAMETER_B);
    float o2g = getParameterValue(PARAMETER_C);
    int size = buffer.getSize();
    float* x = buffer.getSamples(0);
    for(int i=0; i<size; ++i) {
      float in = x[i];		
      float inFilt = inLpf.process(in);
      bool rect = inFilt>0;		
      // the flip flops
      if(lastRect && !rect) {
        oct1 ^= true;		  
	if(lastOct1 && !oct1) {
	  oct2 ^= true;
	}
      }
      float o1 = in * out1Lpf.process(oct1?1:-1);
      float o2 = in * out2Lpf.process(oct2?1:-1);
      lastOct1 = oct1;
      lastRect = rect;      
		x[i] = in * gain + o1*o1g + o2*o2g;
    }
    
  }
};


////////////////////////////////////////////////////////////////////////////////////////////////////

