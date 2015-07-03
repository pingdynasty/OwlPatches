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

#ifndef __SimpleDelayPatch_hpp__
#define __SimpleDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

#define SIMPLE_DELAY_REQUEST_BUFFER_SIZE 1024*192

class SimpleDelayPatch : public Patch {
private:
  CircularBuffer delayBuffer;
  int delay;
  float alpha, dryWet;
public:
  SimpleDelayPatch() : delay(0), alpha(0.04), dryWet(0.f)
  {
    registerParameter(PARAMETER_A, "Delay");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "");
    registerParameter(PARAMETER_D, "Dry/Wet");
    AudioBuffer* buffer = createMemoryBuffer(1, SIMPLE_DELAY_REQUEST_BUFFER_SIZE);
    delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
  }
  void processAudio(AudioBuffer &buffer)
  {
    float delayTime, feedback, dly;
    delayTime = 0.05+0.95*getParameterValue(PARAMETER_A);
    feedback  = getParameterValue(PARAMETER_B);
    int32_t newDelay;
    newDelay = alpha*delayTime*(delayBuffer.getSize()-1) + (1-alpha)*delay; // Smoothing
    dryWet = alpha*getParameterValue(PARAMETER_D) + (1-alpha)*dryWet;       // Smoothing
      
    float* x = buffer.getSamples(0);
    int size = buffer.getSize();
    for (int n = 0; n < size; n++)
    {
      dly = (delayBuffer.read(delay)*(size-1-n) + delayBuffer.read(newDelay)*n)/size;
      delayBuffer.write(feedback * dly + x[n]);
      x[n] = dly*dryWet + (1.f - dryWet) * x[n];  // dry/wet
    }
    delay=newDelay;
  }
};

#endif   // __SimpleDelayPatch_hpp__
