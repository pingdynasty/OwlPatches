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
	float mayhem_freq;
	const float depth = 0.5;
	int inc_flag;	//indicates whether freq. is increasing or decreasing (0 = decreasing, 1 = increasing)
	float freq;
	int update_freq_cnt;
	float prev_freq;
        const float fs;
public:
  DigitalMayhemPatch() : fs(getSampleRate()) {
    registerParameter(PARAMETER_A, "Sampling Freq");
    registerParameter(PARAMETER_B, "Mayhem Rate");
    registerParameter(PARAMETER_C, "Mayhem Freq");
    registerParameter(PARAMETER_D, "Mayhem Depth");
	
	mayhem_freq = 1;	//starting freq = 1*fs/size = 375 Hz
	inc_flag = 1;
	update_freq_cnt = 0;
	prev_freq = 0;
	
  }
  void processAudio(AudioBuffer &buffer)
  {

    int size = buffer.getSize();
	
	float samp_float = getParameterValue(PARAMETER_A);
	int samp_freq = ceil(samp_float*63+0.1);
	
	float mayhem_rate = getParameterValue(PARAMETER_B);
	mayhem_rate *= 0.03;
	float mayhem = 1;
	
	if(abs(getParameterValue(PARAMETER_C)*2+1-prev_freq)>0.01)	//if the knob was turned
	{
		mayhem_freq = getParameterValue(PARAMETER_C);	//update center frequency					
		mayhem_freq *= 2;
		mayhem_freq += 1;			//mayhem_freq range = 1 to 3 --> 375 -- 1125 Hz
		prev_freq = mayhem_freq;	//store value to compare next time	
	}
	
	float mayhem_depth = getParameterValue(PARAMETER_D);
	mayhem_depth *= depth;
	
    //for(int ch=0; ch<buffer.getChannels(); ++ch){
      float* buf = buffer.getSamples(0);
      for(int i=0; i<size; ++i)
		{
			if(i%samp_freq==0)
			{	
				buf[i] = buf[i]*((1-mayhem)+mayhem*abs(cos(2*M_PI*mayhem_freq*(i+update_freq_cnt*size)/size)));
				samp = buf[i];	
			}
			else
				buf[i] = samp;
//				buf[i] = samp*(1-mayhem)+buf[i]*mayhem*abs(cos(2*M_PI*mayhem_freq*(i+update_freq_cnt*size)/size));
		}
//		update_freq_cnt++;
//		if(update_freq_cnt == 10)
		{
		update_freq_cnt = 0;
		if(mayhem_freq>=prev_freq+mayhem_depth || mayhem_freq>=3) inc_flag = 0;			//sets maximum freq 3*fs/size = 1125 Hz
		if(mayhem_freq<=prev_freq-mayhem_depth || mayhem_freq<=1)inc_flag = 1;		//minimum freq that can be achieved in 128 samples is 375 Hz
		if(inc_flag == 0) 
		{
			mayhem_freq /= 1+mayhem_rate*mayhem_depth/depth;
	//		freq = floor(fs/size*mayhem_freq);	//only integer frequencies
		}
		
		if(inc_flag == 1) 
		{
			mayhem_freq *= 1+mayhem_rate*mayhem_depth/depth;
	//		freq = ceil(fs/size*mayhem_freq);	//only integer frequencies
		}
		
	//	mayhem_freq = freq*size/fs;		//only integer frequencies
		}
	}
};


#endif // __DigitalMayhemPatch_hpp__
