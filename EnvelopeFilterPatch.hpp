#ifndef __EnvelopeFilterPatch_hpp__
#define __EnvelopeFilterPatch_hpp__

#include "StompBox.h"
#include <math.h>
// originally https://github.com/micknoise/Maximilian/blob/master/ofxMaxim/ofxMaxim/libs/maximilian.h
// ported to js, ported back again
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
  };

};

class EnvelopeFilterPatch : public Patch {
public:
  EnvelopeFilter::LPF filter;
  EnvelopeFilterPatch(){
    registerParameter(PARAMETER_A, "Cutoff");
    registerParameter(PARAMETER_B, "Range");
    registerParameter(PARAMETER_C, "Q");
    registerParameter(PARAMETER_D, "Attack");
    registerParameter(PARAMETER_E, "E");
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
    float range = getParameterValue(PARAMETER_B)*10000;
    float Q = 1+getParameterValue(PARAMETER_C)*9;
    sensitivity *= sensitivity*160;
    a = 0.9995 + (1-0.9995) * getParameterValue(PARAMETER_D);
    b = 1 - a;
    cutoff = 100 + cutoff * 1500;
//     for(int ch = 0; ch<buffer.getChannels(); ++ch) {
      float* x = buffer.getSamples(0);
      for(int i=0; i<size; ++i) {
	x[i] = filter.process(x[i], cutoff+follow(x[i])*range, Q);
      }
//     }
  }
};

#endif // __EnvelopeFilterPatch_hpp__
