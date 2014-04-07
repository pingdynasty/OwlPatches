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


#ifndef __StutterPatch_hpp__
#define __StutterPatch_hpp__

#include "StompBox.h"

class StutterPatch : public Patch {
private:
    CircularBuffer stutterBuffer;
    bool  patchBegin;
public:
  StutterPatch(){
      patchBegin =  true;
       registerParameter(PARAMETER_A, "Blend");
    registerParameter(PARAMETER_B, "Speed");
    registerParameter(PARAMETER_C, "Length");
    registerParameter(PARAMETER_D, "Fade");
    AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
    stutterBuffer.initialise(buffer->getSamples(0), buffer->getSize());
      

  }
  void processAudio(AudioBuffer &buffer){ // put your code here!
      float Blend, Speed, Length, Fade;
      int stutterIndex = 0;
      bool stutterReady = false;
      bool clipFlag = false;
      Blend = getParameterValue(PARAMETER_A);
      Speed = getParameterValue(PARAMETER_B);
      Length = getParameterValue(PARAMETER_C);
      Fade = getParameterValue(PARAMETER_D);
      
      float maxGrain = 0.100 * stutterBuffer.getSize(); // 100mS
      
      float* x = buffer.getSamples(0);
      float y = 0;
      int size = buffer.getSize();
      
      if(patchBegin == true)
      {
          stutterReady = fillBuffer(x, maxGrain, size);
          patchBegin = false;
    
      }
      for (int n = 0; n < size; n++)
      {
          
         if(stutterReady)
         {
             
         y  = (stutterBuffer.read(stutterIndex) * 0.5) ;
         
          x[n] = (y *  (1 - Blend)) +  (x[n] * Blend);
         }
         
          if( abs(x[n])>1)
          {
              clipFlag = true;
              x[n] *= 0.3;
          }
          
          
          stutterIndex++;
          
          if(stutterIndex >= maxGrain)
          {
              stutterIndex = 0;
          }
      }
      
         
  }
    
    
    bool fillBuffer(float* x, float maxGrain, int size)
    {
       
        for (int n = 0; n < maxGrain; n++)
        {
            
            
            stutterBuffer.write(x[n]);
            
        
        }
        return TRUE;
    }
};

#endif // __TemplatePatch_hpp__
