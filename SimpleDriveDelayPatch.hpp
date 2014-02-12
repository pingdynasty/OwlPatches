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


/* created by the OWL team 2013 */


////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SimpleDriveDelayPatch_hpp__
#define __SimpleDriveDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

#define REQUEST_BUFFER_SIZE 32768

class SimpleDelayPatch : public Patch {
private:
  CircularBuffer delayBuffer;
  int32_t delay;
public:
  SimpleDelayPatch() : delay(0)
  {
    registerParameter(PARAMETER_A, "Delay");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "Drive");
    registerParameter(PARAMETER_D, "Wet/Dry");
    AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
    delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
  }
  void processAudio(AudioBuffer &buffer)
  {
    float delayTime, feedback, wetDry,drive, offset;
    delayTime = getParameterValue(PARAMETER_A);
    feedback  = getParameterValue(PARAMETER_B);
    drive     = getParameterValue(PARAMETER_C);
    wetDry    = getParameterValue(PARAMETER_D);
     
      offset = 1;
      offset /= 10;
      drive += 0.03;
      drive *= 40;
      
    int32_t newDelay;
    newDelay = delayTime * (delayBuffer.getSize()-1);
      
    float* x = buffer.getSamples(0);
    int size = buffer.getSize();
    for (int n = 0; n < size; n++)
    {
      x[n] = (delayBuffer.read(delay)*(size-1-n) + delayBuffer.read(newDelay)*n)*wetDry/size + (1.f - wetDry) * x[n];  // crossfade for wet/dry balance
        delayBuffer.write(feedback * nonLinear((x[n]+offset)*drive));
    }
    delay=newDelay;
  }
    
    float nonLinear(float x){ 		// Overdrive curve
        if (x<-3)
            return -1;
        else if (x>3)
            return 1;
        else
            return x * ( 27 + x*x ) / ( 27 + 9*x*x );
    }
};

#endif   // __SimpleDelayPatch_hpp__
