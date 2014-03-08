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


#ifndef __StereoGainPatch_h__
#define __StereoGainPatch_h__

#include "StompBox.h"

class StereoGainPatch : public Patch {
public:
  StereoGainPatch(){
    registerParameter(PARAMETER_A, "Left");
    registerParameter(PARAMETER_B, "Right");
    registerParameter(PARAMETER_C, "");
    registerParameter(PARAMETER_D, "");
  }

  void processAudio(AudioBuffer &buffer){
    if(buffer.getChannels() > 1){
      // stereo gain
      float gainL = getParameterValue(PARAMETER_A)*2;
      float gainR = getParameterValue(PARAMETER_B)*2;
      int size = buffer.getSize();
      float* left = buffer.getSamples(0);
      float* right = buffer.getSamples(1);
      for(int i=0; i<size; ++i){
	left[i] = gainL*left[i];
	right[i] = gainR*right[i];
      }      
    }else{
      // mono gain
      float gain = getParameterValue(PARAMETER_A)*2;
      int size = buffer.getSize();
      float* samples = buffer.getSamples(0);
      for(int i=0; i<size; ++i){
	samples[i] = gain*samples[i];
      }      
    }
  }
};

#endif // __StereoGainPatch_h__
