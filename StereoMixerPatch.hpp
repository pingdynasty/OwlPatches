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

#ifndef __StereoMixerPatch_h__
#define __StereoMixerPatch_h__

#include "StompBox.h"

class StereoMixerPatch : public Patch {
public:
  StereoMixerPatch(){
    registerParameter(PARAMETER_A, "LL");
    registerParameter(PARAMETER_B, "LR");
    registerParameter(PARAMETER_C, "RL");
    registerParameter(PARAMETER_D, "RR");
  }
  void processAudio(AudioBuffer &buffer){
//     assert_param(buffer.getChannels() > 1);
    float gainLL = getParameterValue(PARAMETER_A);
    float gainLR = getParameterValue(PARAMETER_B);
    float gainRL = getParameterValue(PARAMETER_C);
    float gainRR = getParameterValue(PARAMETER_D);
    int size = buffer.getSize();
    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    float l, r;
    for(int i=0; i<size; ++i){
      l = gainLL*left[i] + gainLR*right[i];
      r = gainRL*left[i] + gainRR*right[i];
      left[i] = l;
      right[i] = r;
    }
  }
};

#endif // __StereoMixerPatch_h__
