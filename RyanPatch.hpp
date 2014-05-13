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


#ifndef __RyanPatch_hpp__
#define __RyanPatch_hpp__

#include "StompBox.h"
#define window_size 2048
#define decay_length 48000

class RyanPatch : public Patch 
{
private:
	int window_index;
	float window[window_size];
	float E_now, E_then;
	char E_flag;
	unsigned int decay_cnt;
	
public:
  RyanPatch()
  {
    registerParameter(PARAMETER_A, "Threshold");
    registerParameter(PARAMETER_B, "Gain");
    registerParameter(PARAMETER_C, "My Knob C");
    registerParameter(PARAMETER_D, "My Knob D");
	
	window_index = 0;
	E_now = 1;
	E_flag = 0;
	decay_cnt = 0;
	
  }
  
  float energy(float *x, int win)
  {
	float sum=0;
  
	for(int i=0; i<win; i++)
	{
		sum += x[i] * x[i];		//running sum of squares for the window
	}
	
	sum/=win;				//average of squares over the window (average energy)
	
	return	sum;			//return average value of energy for the window
  }
  
  
  void processAudio(AudioBuffer &buffer)
  {
	int size = buffer.getSize();

	//float thresh = getParameterValue(PARAMETER_A);     // 
	//float gain = getParameterValue(PARAMETER_B);
 

	
    //for(int ch=0; ch<buffer.getChannels(); ch++)
	{
		float* buf = buffer.getSamples(0);
		
		for(int i=0; i<size; i++)
		{
			window[i+window_index*size] = buf[i];	//load number of samples into the window equal to
													//size of audio buffer
			if (E_flag == 1)
			{
				buf[i] = buf[i]*decay_length/(decay_cnt+decay_length);
				decay_cnt++;
				if(decay_cnt==decay_length)
				{
					decay_cnt=0;
					E_flag=0;
				}
			}
		}
		
		window_index++;		//increment the window index
		
		if(window_index>=window_size/size && E_flag==0)	//if the window is full
		{
			E_then = E_now;							//load previous window's energy into E_then
			E_now = energy(window,window_size);		//call function to calculate current energy
			window_index=0;							//reset the window index to 0
			
			if(E_now > (E_then))
			{
				E_flag = 1;		//set flag to trigger exponential decay if Energy increase detected
			}
		}
	}
  }
  
};

#endif // __RyanPatch_hpp__
