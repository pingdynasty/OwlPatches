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


#ifndef __ReverseReverbPatch_hpp__
#define __ReverseReverbPatch_hpp__

#include "StompBox.h"
#include "string.h"

#define REVERSE_REVERB_BUF_LENGTH 20480

class ReverseReverbPatch : public Patch 
{
private:

	int reverb_index, reverse_cnt, reverb_time;
	char reverse_flag;
	float reverb_buffer[REVERSE_REVERB_BUF_LENGTH];

public:
  ReverseReverbPatch()
  {
    registerParameter(PARAMETER_A, "Reverb Length");
    registerParameter(PARAMETER_B, "");
    registerParameter(PARAMETER_C, "Output Level");
    registerParameter(PARAMETER_D, "Wet/Dry Mix");
	reverb_index = 0;
	reverse_flag = 0;
	reverse_cnt = 0;
	memset(reverb_buffer, 0, sizeof(reverb_buffer));	//initialize reverb buffer
  }
  
  
  void processAudio(AudioBuffer &buffer)
  {
	int size = buffer.getSize();

	float reverb_scale = getParameterValue(PARAMETER_A);			//get reverb length from knob
	if(reverb_scale<0.1) reverb_scale=0.1;							//apply lower limit to reverb length
	reverb_time = round(reverb_scale*REVERSE_REVERB_BUF_LENGTH/2);			//apply scaling factor to the window size to obtain reverb_time (in samples)
	int mod = reverb_time%size;		//ensure that reverb_time is an even multiple of audio buffer size
	reverb_time -= mod;
	if(reverse_cnt>reverb_time)	reverse_cnt = 0;	

	
	float wet = getParameterValue(PARAMETER_D);			//get wet/dry mix from knob		
	
	float level = getParameterValue(PARAMETER_C);		//get output level from knob
	level*=2;

	
    //for(int ch=0; ch<buffer.getChannels(); ch++)
	{
		float* buf = buffer.getSamples(0);
		
		for(int i=0; i<size; i++)
		{		
			reverb_buffer[REVERSE_REVERB_BUF_LENGTH-1-i-reverb_index*size] = reverb_buffer[i+reverb_index*size];	//load reverse into end of buffer
			reverb_buffer[i+reverb_index*size] = buf[i];	//load number of samples into the reverse buffer equal to size of audio buffer

			if (reverse_flag == 1)
			{
				buf[i] = level*((1-wet)*buf[i]+wet*reverb_buffer[REVERSE_REVERB_BUF_LENGTH-1-reverse_cnt]*abs(reverse_cnt-reverb_time)*reverse_cnt/reverb_time/200);
				reverse_cnt++;
				if(reverse_cnt==reverb_time)
				{
					reverse_cnt=0;
					reverse_flag=0;
//					reverb_index=0;
				}
			}
			
			else buf[i] = level*buf[i];
			
		}
		
		reverb_index++;		//increment the window index
		
		if(reverse_flag==0)
		{
			reverb_index=0;				//reset the window index to 0
			reverse_flag = 1;			//set flag to trigger reverse
		}
	}
  }
  
};

#endif // __ReverseReverbPatch_hpp__
