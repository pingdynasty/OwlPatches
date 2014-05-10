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

#define reverb_buf_length 10240
#define pi 3.14159265359f
//const int reverb_buf_length = 10240;

class ReverseReverbPatch : public Patch 
{
private:

	int reverb_index, reverse_cnt;
	char reverse_flag;
	float reverse_storage[reverb_buf_length], reverse_current[reverb_buf_length];
	
public:
  ReverseReverbPatch()
  {
    registerParameter(PARAMETER_A, "Reverb Length");
    registerParameter(PARAMETER_B, "Wet/Dry Mix");
    registerParameter(PARAMETER_C, "Output Level");
    registerParameter(PARAMETER_D, "");
	reverb_index = 0;
	reverse_flag = 0;
	reverse_cnt = 0;
  }
  
  
  void processAudio(AudioBuffer &buffer)
  {
	int size = buffer.getSize();

	float reverb_scale = getParameterValue(PARAMETER_A);				//get reverb length from knob
	if(reverb_scale<0.25) reverb_scale=0.25;
	if(reverb_scale>0.95) reverb_scale=0.95;
	int reverb_time = round(reverb_scale*reverb_buf_length);			//apply scaling factor to the window size to obtain reverb_time (in samples)
	if(reverse_cnt>reverb_time)	reverse_cnt = 0;

	
	//float wet = getParameterValue(PARAMETER_B);			//get wet/dry mix from knob		
	
	float level = getParameterValue(PARAMETER_C);		//get output level from knob
	level /= 200;

	
    for(int ch=0; ch<buffer.getChannels(); ch++)
	{
		float* buf = buffer.getSamples(ch);
		
		for(int i=0; i<size; i++)
		{
			reverse_current[i+reverb_index*size] = buf[i];	//load number of samples into the reverse buffer equal to
															//size of audio buffer
															
			if (reverse_flag == 1)
			{
				//buf[i] = level*((1-wet)*500*buf[i]+wet*reverse_storage[reverb_time-1-reverse_cnt]/2);
				buf[i] = level*(reverse_storage[reverb_time-1-reverse_cnt]/2);
				reverse_cnt++;
				if(reverse_cnt==reverb_time)
				{
					reverse_cnt=0;
					reverse_flag=0;
				}
			}
			
			else
			{	
				buf[i] = level*buf[i]*500;
			}
		}
		
		reverb_index++;		//increment the window index
		
		if(reverb_index >= round(reverb_time/size))	//if the reverse buffer is full
		{
			reverb_index=0;				//reset the window index to 0
			reverse_flag = 1;			//set flag to trigger reverse
			for(int j=0; j<reverb_time; j++)
			{
				reverse_storage[j]=reverse_current[j]*abs(j-reverb_time)*j/(reverb_time/2);  //0.5*(1+cos(2*pi*j/(reverb_time-1)));
			}
		}
	}
  }
  
};

#endif // __ReverseReverbPatch_hpp__
