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

namespace EnvelopeFilter {

#define LPF_SQRT2 1.414213562f
#define LPF_TWOPI 6.2831853071f
#define LPF_TWOPI_BY_SAMPLERATE 0.00014247585731f

  class LPF {
  public:
    float z, x, y, r, c;

    LPF() {
      z = x = y = r = c = 0;
    }

    // cutoff in hz/2 (min 10Hz/2), resonance 1 to 10
    float process(float input, float cutoff, float resonance) {
      if(cutoff>11025) cutoff = 11025;
      z=cos(LPF_TWOPI_BY_SAMPLERATE*cutoff);
      c = 2 - 2*z;
      float zzz = z-1;
      zzz = zzz*zzz*zzz;
      r = (LPF_SQRT2*sqrt(-zzz)+resonance*(z-1))/(resonance*(z-1));
      x += (input - y)*c;
      y += x;
      x *= r;
      return y; 
    }
  };

  class Follower {
  private:
    float env;
    float a, b;
  public:
    Follower(){
      a = 0.9995 + (1-0.9995) * 0.05;//getParameterValue(PARAMETER_D);
      b = 1 - a;
      env = 0;
    }
    inline float follow(float input) {
      if(input<0) input = -input;
      env = env * a + input * b;
      return env*env*160;	
    }
  };

};

class EnvelopeFilterPatch : public Patch {
private:
  EnvelopeFilter::LPF filterL, filterR;
  EnvelopeFilter::Follower followL, followR;

public:
  EnvelopeFilterPatch(){
    registerParameter(PARAMETER_A, "Cutoff");
    registerParameter(PARAMETER_B, "Range");
    registerParameter(PARAMETER_C, "Q");
    registerParameter(PARAMETER_D, "Dry/Wet");
  }
	
  void processAudio(AudioBuffer& buffer){
    int size = buffer.getSize();
    float cutoff = getParameterValue(PARAMETER_A);
    float range = sqrtf(getParameterValue(PARAMETER_B))*1000;
    float Q = 3+getParameterValue(PARAMETER_C)*9;
    float mix = getParameterValue(PARAMETER_D);
    cutoff = 100 + cutoff * 1500;

    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    float mixm1 = 1.0 - mix;
    for(int i=0; i<size; ++i) {
      left[i] = left[i]*mixm1 + (mix)*filterL.process(left[i], cutoff+followL.follow(left[i])*range, Q);
      right[i] = right[i]*mixm1 + (mix)*filterR.process(right[i], cutoff+followR.follow(right[i])*range, Q);
    }

  }
};

#endif // __EnvelopeFilterPatch_hpp__
