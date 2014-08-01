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


#ifndef __GhostNotePatch_hpp__
#define __GhostNotePatch_hpp__

#include "StompBox.h"

class GhostNotePatch : public Patch 
{
private:
	unsigned int window_index, zero_x_cnt, synth_freq;
	float prev_samp;
	
public:
  GhostNotePatch()
  {
    registerParameter(PARAMETER_A, "Gain");
    registerParameter(PARAMETER_B, "Window");
    registerParameter(PARAMETER_C, "Frequency");
    registerParameter(PARAMETER_D, "Wet");
    registerParameter(PARAMETER_E, "Freq Mod");
	
	window_index = 0;
	prev_samp = 0;
	zero_x_cnt = 0;
	synth_freq = 440;
  }
  
  
  void processAudio(AudioBuffer &buffer)
  {
	int size = buffer.getSize();
	float expr = 1 - getParameterValue(PARAMETER_E);
	float window_size_factor = getParameterValue(PARAMETER_B);     //
	unsigned int window_size = round(window_size_factor*8192);
	window_size += size;	//window size should never equal 0
	if(window_size%size != 0) window_size -= window_size%size; //make sure window size is integer multiple of buffer size
	window_size += size;	//window size should never equal 0
	float gain = getParameterValue(PARAMETER_A);
	float freq_factor = expr*getParameterValue(PARAMETER_C);
	freq_factor += 0.01;
	float wet = getParameterValue(PARAMETER_D);

	
    //for(int ch=0; ch<buffer.getChannels(); ch++)
	{
		float* buf = buffer.getSamples(0);
		
		for(int i=0; i<size; i++)
		{
			if(prev_samp/(prev_samp+0.01)<0) zero_x_cnt++;	//if pos->neg or neg->pos transition occurs increment zero_x_cnt
			prev_samp = buf[i];
			
			buf[i] *= sin(2*3.14159*synth_freq*window_index/window_size);	
						
			window_index++;		//increment the window index
			if(window_index>window_size)
			{	
				window_index=0;	//reset window index once it bypasses window size
//				if(48000/window_size*zero_x_cnt>10000) zero_x_cnt=0.2*window_size;
				synth_freq = round(zero_x_cnt*freq_factor/4);
				zero_x_cnt=0;
			}
			buf[i] = (1-wet)*prev_samp + wet*buf[i];
			buf[i] *= gain;
		}

	}
  }
  
};

#endif // __GhostNotePatch_hpp__
