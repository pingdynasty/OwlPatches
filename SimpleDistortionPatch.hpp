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


#ifndef __SimpleDistortionPatch_hpp__
#define __SimpleDistortionPatch_hpp__

#include "StompBox.h"

class SimpleDistortionPatch : public Patch {
public:
  SimpleDistortionPatch(){
  }
  void processAudio(AudioBuffer &buffer)
  {

    int size = buffer.getSize();
    //for(int ch=0; ch<buffer.getChannels(); ++ch){
      float* buf = buffer.getSamples(0);
      for(int i=0; i<size; ++i)
		{
			if(buf[i]<0)
			buf[i] = -buf[i];
		}
	}
};


#endif // __SimpleDistortionPatch_hpp__
