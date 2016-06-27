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
#include "BiquadFilter.h"

class SimpleStereoDelayPatch : public Patch {
private:
  static const int REQUEST_BUFFER_SIZE = 64*1024;
  CircularBuffer* delayBufferL;
  CircularBuffer* delayBufferR;
  int delay;
  float alpha, dryWet;
  StereoBiquadFilter* highpass;
public:
  SimpleStereoDelayPatch() : delay(0), alpha(0.04), dryWet(0.f)
  {
    registerParameter(PARAMETER_A, "Delay");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "Input Level");
    registerParameter(PARAMETER_D, "Dry/Wet");
    delayBufferL = CircularBuffer::create(REQUEST_BUFFER_SIZE);
    delayBufferR = CircularBuffer::create(REQUEST_BUFFER_SIZE);
    highpass = StereoBiquadFilter::create(1);
    highpass->setHighPass(40/(getSampleRate()/2), FilterStage::BUTTERWORTH_Q); // dc filter
  }
  ~SimpleStereoDelayPatch(){
    CircularBuffer::destroy(delayBufferL);
    CircularBuffer::destroy(delayBufferR);
    StereoBiquadFilter::destroy(highpass);
  }
  void processAudio(AudioBuffer &buffer){
    float gain = getParameterValue(PARAMETER_C)*2;
    float delayTime = 0.05+0.95*getParameterValue(PARAMETER_A);
    float feedback  = getParameterValue(PARAMETER_B);
    int32_t newDelay;
    newDelay = alpha*delayTime*(delayBufferL->getSize()-1) + (1-alpha)*delay; // Smoothing
    dryWet = alpha*getParameterValue(PARAMETER_D) + (1-alpha)*dryWet;       // Smoothing      
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    int size = buffer.getSize();
    highpass->process(buffer);
    for (int n = 0; n < size; n++){
      float sample = gain*left[n];
      float dly = (delayBufferL->read(delay)*(size-1-n) + delayBufferL->read(newDelay)*n)/size;
      delayBufferL->write(feedback * dly + sample);
      left[n] = dly*dryWet + (1.f - dryWet) * sample;  // dry/wet
      sample = gain*right[n];
      dly = (delayBufferR->read(delay)*(size-1-n) + delayBufferR->read(newDelay)*n)/size;
      delayBufferR->write(feedback * dly + sample);
      right[n] = dly*dryWet + (1.f - dryWet) * sample;  // dry/wet
    }
    delay = newDelay;
  }
};

#endif   // __SimpleStereoDelayPatch_hpp__
