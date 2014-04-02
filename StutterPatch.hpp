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


/* created by the OWL team 2014 */


////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __StuttrePatch_hpp__
#define __StutterPatch_hpp__

#include "StompBox.h"

class StutterPatch : public Patch {
private:
    CircularBuffer stutterBuffer;
    bool  patchBegin;
public:
  StutterPatch(){
      patchBegin =  false;
       registerParameter(PARAMETER_A, "Blend");
    registerParameter(PARAMETER_B, "Speed");
    registerParameter(PARAMETER_C, "Length");
    registerParameter(PARAMETER_D, "Fade");
    AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
    stutterBuffer.initialise(buffer->getSamples(0), buffer->getSize());
  

  }
  void processAudio(AudioBuffer &buffer){ // put your code here!
      float Blend, Speed, Length, Fade;
      Blend = GetParameterValue(PARAMETER_A);
      Speed = GetParameterValue(PARAMETER_B);
      Length = GetParameterValue(PARAMETER_C);
      Fade = GetParameterValue(PARAMETER_D);
      
      float maxGrain = 0.900 * (stutterBuffer.getSize()-1); // 900mS
      
      float* x = buffer.getSamples(0);
       float y = 0;
      if(patchBegin == false)
      {
          y = stutterBuffer.read(maxGrain);
          
      }
      x[n] = (y *  (1 - Blend)) +  (x[n] * Blend);
      
      
         
  }
};

#endif // __TemplatePatch_hpp__
