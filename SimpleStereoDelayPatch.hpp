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


/* created by the OWL team 2013, modified by Silvere Letellier for stereo capabilities */


////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __SimpleStereoDelayPatch_hpp__
#define __SimpleStereoDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

#define SIMPLE_STEREO_DELAY_REQUEST_BUFFER_SIZE (64*1024)

class SimpleStereoDelayPatch : public Patch {
private:
  CircularBuffer delayBufferL;
  CircularBuffer delayBufferR;
  int delay;
  float alpha, dryWet;
public:
  SimpleStereoDelayPatch() : delay(0), alpha(0.04), dryWet(0.f)
  {
    registerParameter(PARAMETER_A, "Input");
    registerParameter(PARAMETER_B, "Delay");
    registerParameter(PARAMETER_C, "Feedback");
    registerParameter(PARAMETER_D, "Dry/Wet");
    AudioBuffer* buffer = createMemoryBuffer(2, SIMPLE_STEREO_DELAY_REQUEST_BUFFER_SIZE);
    delayBufferL.initialise(buffer->getSamples(0), buffer->getSize());
    delayBufferR.initialise(buffer->getSamples(1), buffer->getSize());
  }
  void processAudio(AudioBuffer &buffer)
  {
    float gain, delayTime, feedback, dly;
    gain = getParameterValue(PARAMETER_A)*2;
    delayTime = 0.05+0.95*getParameterValue(PARAMETER_B);
    feedback  = getParameterValue(PARAMETER_C);
    int32_t newDelay;
    newDelay = alpha*delayTime*(delayBufferL.getSize()-1) + (1-alpha)*delay; // Smoothing
    dryWet = alpha*getParameterValue(PARAMETER_D) + (1-alpha)*dryWet;       // Smoothing
      
    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    int size = buffer.getSize();
    for (int n = 0; n < size; n++)
    {
      float sample = gain*left[n];
      dly = (delayBufferL.read(delay)*(size-1-n) + delayBufferL.read(newDelay)*n)/size;
      delayBufferL.write(feedback * dly + sample);
      left[n] = dly*dryWet + (1.f - dryWet) * sample;  // dry/wet
      sample = gain*right[n];
      dly = (delayBufferR.read(delay)*(size-1-n) + delayBufferR.read(newDelay)*n)/size;
      delayBufferR.write(feedback * dly + sample);
      right[n] = dly*dryWet + (1.f - dryWet) * sample;  // dry/wet
    }
    delay=newDelay;
  }
};

#endif   // __SimpleStereoDelayPatch_hpp__
