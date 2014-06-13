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
    registerParameter(PARAMETER_A, "Sampling Freq");
    registerParameter(PARAMETER_B, "Mayhem");
    registerParameter(PARAMETER_C, "Mayhem Freq");
    registerParameter(PARAMETER_D, "My Knob D");
	
	
  }
  void processAudio(AudioBuffer &buffer)
  {

    int size = buffer.getSize();
	
	float samp_float = getParameterValue(PARAMETER_A);
	int samp_freq = ceil(samp_float*63+0.1);
	
	float mayhem = getParameterValue(PARAMETER_B);
	
	float mayhem_freq = getParameterValue(PARAMETER_C);
	
	
    //for(int ch=0; ch<buffer.getChannels(); ++ch){
      float* buf = buffer.getSamples(0);
      for(int i=0; i<size; ++i)
		{
			if(i%samp_freq==0)
			{
				samp = buf[i];
				buf[i] = buf[i]*(1+mayhem*cos(2*3.14*mayhem_freq*i/size));
			}
			else
				buf[i] = samp*(1+mayhem*cos(2*3.14*mayhem_freq*i/size));
		}
	}
};


#endif // __DigitalMayhemPatch_hpp__
