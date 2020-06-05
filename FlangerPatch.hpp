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


#ifndef ___FlangerPatch_hpp__
#define __FlangerPatch_hpp__

#include "CircularBuffer.hpp"

#define FLANGER_BUFFER_SIZE 1024

class FlangerPatch : public Patch {
private:
    CircularBuffer **delayBuffers;
    float rate, depth, feedback, phase;
    
public:
  FlangerPatch(){
    delayBuffers = (CircularBuffer **) calloc(2, sizeof(CircularBuffer *));
    for (int ch = 0; ch < 2; ch++)
    {
      delayBuffers[ch] = CircularBuffer::create(FLANGER_BUFFER_SIZE);
    }
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Depth");
    registerParameter(PARAMETER_C, "Feedback");
    registerParameter(PARAMETER_D, "");    
    phase = 0;
  }
  float modulate(float rate) {
    phase += rate;
    if ( phase >= 1.0 ) {
        phase -= 1.0;
    }
    return sinf(phase*(2*M_PI)) * 0.5 + 0.5;    // sine function between 0..1
  };
  void processAudio(AudioBuffer &buffer){
    int size = buffer.getSize();
    unsigned int delaySamples;
      
    rate     = getParameterValue(PARAMETER_A) * 0.000005f; // flanger needs slow rate
    depth    = getParameterValue(PARAMETER_B);
    feedback = getParameterValue(PARAMETER_C) * 0.9;
      
    for (int ch = 0; ch<buffer.getChannels(); ++ch) {
        for (int i = 0 ; i < size; i++) {
            float* buf = buffer.getSamples(ch);
            delaySamples = (depth * modulate(rate)) * (delayBuffers[ch]->getSize()-1) + 1; // compute delay according to rate and depth
            buf[i] += feedback * delayBuffers[ch]->read(delaySamples); // add scaled delayed signal to dry signal
            delayBuffers[ch]->write(buf[i]); // update delay buffer
        }
    }
  }
    
};


#endif /* __FlangerPatch_hpp__ */

////////////////////////////////////////////////////////////////////////////////////////////////////
