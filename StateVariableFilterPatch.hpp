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

#ifndef __StateVariableFilterPatch_hpp__
#define __StateVariableFilterPatch_hpp__

#include "SampleBasedPatch.hpp"

/**
State variable Filter
http://musicdsp.org/showArchiveComment.php?ArchiveID=23

Type : 12db resonant low, high or bandpass
References : Effect Deisgn Part 1, Jon Dattorro, J. Audio Eng. Soc., Vol 45, No. 9, 1997 September

Notes : 
Digital approximation of Chamberlin two-pole low pass. Easy to calculate coefficients, easy to process algorithm.

Code : 
cutoff = cutoff freq in Hz
fs = sampling frequency //(e.g. 44100Hz)
f = 2 sin (pi * cutoff / fs) //[approximately]
q = resonance/bandwidth [0 < q <= 1]  most res: q=1, less: q=0
low = lowpass output
high = highpass output
band = bandpass output
notch = notch output
scale = q
low=high=band=0;
//--beginloop
low = low + f * band;
high = scale * input - low - q*band;
band = f * high + band;
notch = high + low;
//--endloop
*/


class StateVariableFilterPatch : public SampleBasedPatch {
private:
  float low, band;
  float f, q;
  float gain;
public:
  StateVariableFilterPatch() : low(0), band(0) {
    registerParameter(PARAMETER_A, "Fc");
    registerParameter(PARAMETER_B, "Q");
    registerParameter(PARAMETER_D, "Gain");
  }
  void prepare(){
    float fc;
    fc = getParameterValue(PARAMETER_A);
    q = getParameterValue(PARAMETER_B);
    gain = getParameterValue(PARAMETER_D); // get gain value
    fc /= 2;
    f = sin(M_PI * fc);
    q = 1 - q;
    // fc = cutoff freq in Hz
    // fs = sampling frequency //(e.g. 44100Hz)
    // q = resonance/bandwidth [0 < q <= 1]  most res: q=1, less: q=0
  }
  float processSample(float sample){
    low = low + f * band;
    float high = q * sample - low - q*band;
    band = f * high + band;
    return gain*low;
  }
};

#endif /* __StateVariableFilterPatch_hpp__ */
