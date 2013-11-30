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


// originally https://github.com/micknoise/Maximilian/blob/master/ofxMaxim/ofxMaxim/libs/maximilian.h
// ported to js, ported back again

////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __EnvelopeFilterPatch_hpp__
#define __EnvelopeFilterPatch_hpp__

#include "StompBox.h"
#include <math.h>

namespace EnvelopeFilter {
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
    // cutoff in hz/2 (min 10Hz/2), resonance 1 to 10
    float process(float input, float cutoff, float resonance) {
      if(cutoff>11025) cutoff = 11025;
      z=cos(TWOPI_BY_SAMPLERATE*cutoff);
      c = 2 - 2*z;
      float zzz = z-1;
      zzz = zzz*zzz*zzz;
      r = (SQRT2*sqrt(-zzz)+resonance*(z-1))/(resonance*(z-1));

      x += (input - y)*c;
      y += x;
      x *= r;
      return y; 
    }
	  // by rbj
	  static float fastSqrt(register float x)
	  {
		  
		  if (x > 5.877471754e-39)
		  {
			  register float accumulator, xPower;
			  register long intPart;
			  register union {float f; long i;} xBits;
			  
			  xBits.f = x;
			  
			  intPart = ((xBits.i)>>23);	 /* get biased exponent */
			  intPart -= 127;	 /* unbias it */
			  
			  x = (float)(xBits.i & 0x007FFFFF);	 /* mask off exponent leaving 0x800000*(mantissa - 1) */
			  x *= 1.192092895507812e-07;	 /* divide by 0x800000 */
			  
			  accumulator =  1.0 + 0.49959804148061*x;
			  xPower = x*x;
			  accumulator += -0.12047308243453*xPower;
			  xPower *= x;
			  accumulator += 0.04585425015501*xPower;
			  xPower *= x;
			  accumulator += -0.01076564682800*xPower;
			  
			  if (intPart & 0x00000001)
			  {
				  accumulator *= SQRT2;	 /* an odd input exponent means an extra sqrt(2) in the output */
			  }
			  
			  xBits.i = intPart >> 1;	 /* divide exponent by 2, lose LSB */
			  xBits.i += 127;	 /* rebias exponent */
			  xBits.i <<= 23;	 /* move biased exponent into exponent bits */
			  
			  return accumulator * xBits.f;
		  }
		  else
		  {
			  return 0.0;
		  }
		  
	  }
  };
	
	

};

class EnvelopeFilterPatch : public Patch {
public:
  EnvelopeFilter::LPF filter;
  EnvelopeFilterPatch(){
    registerParameter(PARAMETER_A, "Cutoff");
    registerParameter(PARAMETER_B, "Range");
    registerParameter(PARAMETER_C, "Blend");
    registerParameter(PARAMETER_D, "Q");
  //  registerParameter(PARAMETER_E, "E");
    env = 0;
    a = 0.999;
    b = 0.001;
  }
  float sensitivity;
  float env;
  float a, b;
  inline float follow(float input) {
	if(input<0) input = -input;
	env = env * a + input * b;
	
	return env*env*160;
	
  }
	
  void processAudio(AudioBuffer& buffer){
    float gain = 1;//getParameterValue(PARAMETER_D);
    int size = buffer.getSize();
    float cutoff = getParameterValue(PARAMETER_A);
	  float range = //EnvelopeFilter::LPF::fastSqrt
	  (getParameterValue(PARAMETER_B))*1000;
    float Q = 3+getParameterValue(PARAMETER_D)*9;
	  float mix = getParameterValue(PARAMETER_C)*0.5;
    sensitivity *= sensitivity*160;
    a = 0.9995 + (1-0.9995) * 0.05;//getParameterValue(PARAMETER_D);
    b = 1 - a;
    cutoff = 100 + cutoff * 1500;

	float* x = buffer.getSamples(0);
	float mixm1 = 1.0 - mix;
	for(int i=0; i<size; ++i) {
		x[i] = x[i]*mix + (mixm1)*filter.process(x[i], cutoff+follow(x[i])*range, Q);
	}

  }
};

#endif // __EnvelopeFilterPatch_hpp__
