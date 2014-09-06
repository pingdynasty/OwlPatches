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


#ifndef __ZoelzerMultiFilterPatch_hpp__
#define __ZoelzerMultiFilterPatch_hpp__

#include "StompBox.h"

#define ZOELZER_LOWPASS_FILTER_MODE   0
#define ZOELZER_PEQ_BOOST_MODE        1
#define ZOELZER_PEQ_CUT_MODE          2
#define ZOELZER_LOWSHELF_BOOST_MODE   3
#define ZOELZER_LOWSHELF_CUT_MODE     4
#define ZOELZER_HIGHPASS_FILTER_MODE  5
#define ZOELZER_BANDPASS_FILTER_MODE  6
#define ZOELZER_NOTCH_FILTER_MODE     7
#define ZOELZER_HIGHSHELF_FILTER_MODE 8
#define ZOELZER_MODES                 9
/**

http://www.musicdsp.org/showArchiveComment.php?ArchiveID=37

Type : biquad IIR
References : Udo Zoelzer: Digital Audio Signal Processing (John Wiley & Sons, ISBN 0 471 97226 6), Chris Townsend

omega = 2*PI*frequency/sample_rate

K=tan(omega/2)
Q=Quality Factor
V=gain

LPF: 
b0 = K^2
b1 = 2*K^2
b2 = K^2
a0 = 1 + K/Q + K^2
a1 = 2*(K^2 - 1)
a2 = 1 - K/Q + K^2

peakingEQ boost:
b0 = 1 + V*K/Q + K^2
b1 = 2*(K^2 - 1)
b2 = 1 - V*K/Q + K^2
a0 = 1 + K/Q + K^2
a1 = 2*(K^2 - 1)
a2 = 1 - K/Q + K^2

peakingEQ cut:
b0 = 1 + K/Q + K^2
b1 = 2*(K^2 - 1)
b2 = 1 - K/Q + K^2
a0 = 1 + V*K/Q + K^2
a1 = 2*(K^2 - 1)
a2 = 1 - V*K/Q + K^2

lowshelf boost:
b0 = 1 + sqrt(V)*K/Q + V*K^2 
b1 = 2*(V*K^2 - 1) 
b2 = 1 - sqrt(V)*K/Q + V*K^2 
a0 = 1 + K/Q + K^2 
a1 = 2*(K^2 - 1) 
a2 = 1 - K/Q + K^2
// or
b0 = 1 + sqrt(2*V)*K + V*K^2
b1 = 2*(V*K^2 - 1)
b2 = 1 - sqrt(2*V)*K + V*K^2
a0 = 1 + K/Q + K^2
a1 = 2*(K^2 - 1)
a2 = 1 - K/Q + K^2

lowshelf cut:
b0 = 1 + K/Q + K^2
b1 = 2*(K^2 - 1)
b2 = 1 - K/Q + K^2
a0 = 1 + sqrt(2*V)*K + V*K^2
a1 = 2*(v*K^2 - 1)
a2 = 1 - sqrt(2*V)*K + V*K^2

BPF1 (peak gain = Q):
b0 = K
b1 = 0
b2 = -K
a0 = 1 + K/Q + K^2
a1 = 2*(K^2 - 1)
a2 = 1 - K/Q + K^2

BPF2 (peak gain = zero):
b0 = K/Q
b1 = 0
b2 = -K/Q
a0 = 1 + K/Q + K^2
a1 = 2*(K^2 - 1)
a2 = 1 - K/Q + K^2

notch:
b0 = 1 + K^2
b1 = 2*(K^2 - 1)
b2 = 1 + K^2
a0 = 1 + K/Q + K^2
a1 = 2*(K^2 - 1)
a2 = 1 - K/Q + K^2

HPF:
b0 = 1 + K/Q
b1 = -2
b2 = 1 - K/Q
a0 = 1 + K/Q + K^2
a1 = 2*(K^2 - 1)
a2 = 1 - K/Q + K^2

highshelf:
b0 =   (K*K + sqrt(2*V)*K + V);
b1 = 2*(K*K               - V);
b2 =   (K*K - sqrt(2*V)*K + V);
a0 =   (K*K + K*fQ + 1)
a1 =-2*(K*K        - 1);
a2 =-  (K*K - K*fQ + 1);

*/

class ZoelzerMultiFilterPatch : public Patch {
private:
public:
  class Biquad {
  public:
    float b0, b1, b2;
    float a0, a1, a2;
    float x1, x2, y1, y2;
    Biquad(): x1(0), x2(0), y1(0), y2(0) {}
    float compute(float x0){
      /* compute next sample */
      float y0 = (b0*x0 + b1*x1 + b2*x2 - a1*y1 - a2*y2)/a0;
      x2 = x1;
      x1 = x0;
      y2 = y1;
      y1 = y0;
      return y0;
    }
  };
  Biquad filter;
  ZoelzerMultiFilterPatch(){
    registerParameter(PARAMETER_A, "Mode");
    registerParameter(PARAMETER_B, "Frequency");
    registerParameter(PARAMETER_C, "Resonance");
    registerParameter(PARAMETER_D, "Gain");
  }
  void processAudio(AudioBuffer &buffer){
    int mode = getParameterValue(PARAMETER_A)*ZOELZER_MODES;
    float omega = 2*M_PI*getParameterValue(PARAMETER_B)/getSampleRate();
    float k = tan(omega/2);
    float q = getParameterValue(PARAMETER_C); // Resonance
    float v = getParameterValue(PARAMETER_D); // Gain
    switch(mode){
    case ZOELZER_LOWPASS_FILTER_MODE:
      filter.b0 = k*k;
      filter.b1 = 2*k*k;
      filter.b2 = k*k;
      filter.a0 = 1 + k/q + k*k;
      filter.a1 = 2*(k*k - 1);
      filter.a2 = 1 - k/q + k*k;
      break;
    case ZOELZER_PEQ_BOOST_MODE:
      filter.b0 = 1 + v*k/q + k*k;
      filter.b1 = 2*(k*k - 1);
      filter.b2 = 1 - v*k/q + k*k;
      filter.a0 = 1 + k/q + k*k;
      filter.a1 = 2*(k*k - 1);
      filter.a2 = 1 - k/q + k*k; 
      break;
    case ZOELZER_PEQ_CUT_MODE:
      filter.b0 = 1 + k/q + k*k;
      filter.b1 = 2*(k*k - 1);
      filter.b2 = 1 - k/q + k*k;
      filter.a0 = 1 + v*k/q + k*k;
      filter.a1 = 2*(k*k - 1);
      filter.a2 = 1 - v*k/q + k*k;
      break;
    case ZOELZER_LOWSHELF_BOOST_MODE:
      filter.b0 = 1 + sqrt(v)*k/q + v*k*k;
      filter.b1 = 2*(v*k*k - 1);
      filter.b2 = 1 - sqrt(v)*k/q + v*k*k;
      filter.a0 = 1 + k/q + k*k;
      filter.a1 = 2*(k*k - 1);
      filter.a2 = 1 - k/q + k*k;
      break;
    case ZOELZER_LOWSHELF_CUT_MODE:
      filter.b0 = 1 + k/q + k*k;
      filter.b1 = 2*(k*k - 1);
      filter.b2 = 1 - k/q + k*k;
      filter.a0 = 1 + sqrt(2*v)*k + v*k*k;
      filter.a1 = 2*(v*k*k - 1);
      filter.a2 = 1 - sqrt(2*v)*k + v*k*k;
      break;
    case ZOELZER_HIGHPASS_FILTER_MODE:
      filter.b0 = 1 + k/q;
      filter.b1 = -2;
      filter.b2 = 1 - k/q;
      filter.a0 = 1 + k/q + k*k;
      filter.a1 = 2*(k*k - 1);
      filter.a2 = 1 - k/q + k*k;
      break;
    case ZOELZER_BANDPASS_FILTER_MODE:
      filter.b0 = k;
      filter.b1 = 0;
      filter.b2 = -k;
      filter.a0 = 1 + k/q + k*k;
      filter.a1 = 2*(k*k - 1);
      filter.a2 = 1 - k/q + k*k;
      break;
    case ZOELZER_NOTCH_FILTER_MODE:
      filter.b0 = 1 + k*k;
      filter.b1 = 2*(k*k - 1);
      filter.b2 = 1 + k*k;
      filter.a0 = 1 + k/q + k*k;
      filter.a1 = 2*(k*k - 1);
      filter.a2 = 1 - k/q + k*k;
      break;
    case ZOELZER_HIGHSHELF_FILTER_MODE:
      filter.b0 = (k*k + sqrt(2*v)*k + v);
      filter.b1 = 2*(k*k - v);
      filter.b2 = (k*k - sqrt(2*v)*k + v);
      filter.a0 = (k*k + k*q + 1);
      filter.a1 = -2*(k*k - 1);
      filter.a2 = -(k*k - k*q + 1);
      break;
    }
    int size = buffer.getSize();
    for(int ch=0; ch<buffer.getChannels(); ++ch){
      float* samples = buffer.getSamples(ch);
      for(int i=0; i<size; ++i)
	samples[i] = filter.compute(samples[i]);
    }
  }
};

#endif // __ZoelzerMultiFilterPatch_hpp__
