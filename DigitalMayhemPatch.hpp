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


/* created by Ryan Sarver */


////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __DigitalMayhemPatch_hpp__
#define __DigitalMayhemPatch_hpp__

#include "StompBox.h"

class DigitalMayhemPatch : public Patch {
private:
	float samp;	

public:
  DigitalMayhemPatch(){
    registerParameter(PARAMETER_A, "My Knob A");
    registerParameter(PARAMETER_B, "My Knob B");
    registerParameter(PARAMETER_C, "My Knob C");
    registerParameter(PARAMETER_D, "My Knob D");
	
  }
  void processAudio(AudioBuffer &buffer)
  {

    int size = buffer.getSize();
	
	float samp_float = getParameterValue(PARAMETER_A);
	int samp_freq = ceil(samp_float*128+0.1);
    //for(int ch=0; ch<buffer.getChannels(); ++ch){
      float* buf = buffer.getSamples(0);
      for(int i=0; i<size; ++i)
		{
			if(i%samp_freq==0)
				samp = buf[i];
			else
				buf[i] = samp;
		}
	}
};


#endif // __DigitalMayhemPatch_hpp__
