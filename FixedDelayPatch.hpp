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

#ifndef __FixedDelayPatch_hpp__
#define __FixedDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#define REQUEST_BUFFER_SIZE 262144

class FixedDelayPatch : public Patch {
    
private:
  CircularBuffer delayBuffer;
    
public:
  FixedDelayPatch() {
    AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
    delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
    registerParameter(PARAMETER_A, "Feedback");
    registerParameter(PARAMETER_B, "Mix");
  }
  void processAudio(AudioBuffer &buffer) {
    float* x = buffer.getSamples(0);
    float feedback = getParameterValue(PARAMETER_A);
    float mix = getParameterValue(PARAMETER_B);
    for(int n = 0; n < buffer.getSize(); n++){
      x[n] = delayBuffer.tail()*mix + x[n]*(1.0f-mix);
      delayBuffer.write(feedback * x[n]);
    }
  }
};

#endif   // __FixedDelayPatch_hpp__


////////////////////////////////////////////////////////////////////////////////////////////////////
