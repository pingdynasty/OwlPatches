/*
 Copyright (C) 2013 Jason Frank
 
 This program is free software; you can redistribute it and/or
 modify it under the terms of the GNU General Public License
 as published by the Free Software Foundation; either version 2
 of the License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef __BitH8rPatch_hpp__
#define __BitH8rPatch_hpp__

#include "StompBox.h"
#include <math.h>

class BitH8rPatch : public Patch {
public:
  BitH8rPatch(){

    registerParameter(PARAMETER_A, "Bits Suck");
	registerParameter(PARAMETER_B, "Samples Suck");
	registerParameter(PARAMETER_C, "Attenuation Sucks");
	registerParameter(PARAMETER_D, "OWL is cool, however");

  }
   void processAudio(AudioBuffer &buffer){
    
	  int totalBits = 8;

	  float bitReduce =  floor((getParameterValue(PARAMETER_A) * (totalBits -1)) + .5);
	  int bitStutter = (int)floor((getParameterValue(PARAMETER_B) * 50) + .5);
	  int gain = (int)ceil((getParameterValue(PARAMETER_C) * 25) + .5); 
	  
	  int crushedMax = pow(2, totalBits - bitReduce) - 1;
	  int loopCount = bitStutter;	
	  float currentSample = 0;

	  int size = buffer.getSize();

	for(int ch = 0; ch<buffer.getChannels(); ++ch)
	{
	  float* buf = buffer.getSamples(ch);
	  for(int i = 0; i < size; i++)
	  {
		 if(--loopCount <= 0)
		  {
			  float x = buf[i];
			  x = (x + 1.0) * crushedMax;
			  x = x > 0 ? floor(x + 0.5) : ceil(x - 0.5);
			  x = (x / crushedMax) - 1.0;
			  x = x * gain;
				
			  currentSample = x;

			  loopCount = bitStutter;
		  }
			  buf[i] = currentSample;
	  }
       }

  }

};

#endif // __BitH8rPatch_hpp__
