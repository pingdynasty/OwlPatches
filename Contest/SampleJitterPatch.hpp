//
//  SampleJitterPatch.hpp
//
//  A distortion effect: delays incoming samples by random amounts of time.
//  (Strictly speaking: play back samples taken from a randomly selected point
//  in the near past. With variable near/far selection bias.)
//
//  Created by martind on 13/06/2013.
//  http://github.com/dekstop/OwlSim/tree/test/Source/Patches
//
//  Parameter assignment:
//  - A: max sample delay time
//  - B: near/far sample selection bias
//  - C:
//  - D: dry/wet mix
//  - Push-button:
//
//  TODO:
//  - ...
//

/*
 Copyright (C) 2013 Martin Dittus
 
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

#ifndef OwlSim_SampleJitterPatch_hpp
#define OwlSim_SampleJitterPatch_hpp

#include "StompBox.h"
//#include "juce_Random.h"

class SampleJitterPatch : public Patch {
  
  const float MIN_DELAY; // in seconds
  const float MAX_DELAY;
  const float MIN_BIAS; // bias acts as exponent on a random delay time, t = rnd^bias
  const float MAX_BIAS;
  
  float* circularBuffer;
  unsigned int bufferSize;
  unsigned int writeIdx;
  
public:
  SampleJitterPatch() : MIN_DELAY(0.00001), MAX_DELAY(0.02), MIN_BIAS(0.1), MAX_BIAS(6), ramp(0.1) {
    registerParameter(PARAMETER_A, "Rate");
    registerParameter(PARAMETER_B, "Bias");
    registerParameter(PARAMETER_D, "Dry/Wet");
  }
 void processAudio(AudioBuffer &buffer) 
  {

    double rate = getSampleRate();
    
    if (circularBuffer==NULL)
    {
      bufferSize = MAX_DELAY * rate;
      circularBuffer = new float[bufferSize];
      memset(circularBuffer, 0, bufferSize*sizeof(float));
      writeIdx = 0;
    }

    float p1 = getRampedParameterValue(PARAMETER_A);
    float p2 = getRampedParameterValue(PARAMETER_B);
//    float p3 = getRampedParameterValue(PARAMETER_C);
    float p4 = getRampedParameterValue(PARAMETER_D);

    unsigned int maxSampleDelay = rate * (MIN_DELAY + p1*p1 * (MAX_DELAY-MIN_DELAY));
    float bias = MIN_BIAS + p2*p2 * (MAX_BIAS-MIN_BIAS);
    // float cutoff = p3;
    float dryWetMix = p4;
    
    int size = buffer.getSize();

	for(int ch = 0; ch<buffer.getChannels(); ++ch)
	 { 	
	    float* buf = buffer.getSamples(ch);
	    Random r;
	    for (int i=0; i<size; ++i)
	    {
	      int offset = floor(maxSampleDelay * pow(r.nextFloat(), bias) + 0.5);
	      int readIdx = writeIdx - offset;
	      while (readIdx<0)
		readIdx += bufferSize;

	      circularBuffer[writeIdx] = buf[i];
	      buf[i] =
		circularBuffer[readIdx] * dryWetMix +
		buf[i] * (1 - dryWetMix);

	      writeIdx = (++writeIdx) % bufferSize;
	    }
	 }
     
  }
  
  ~SampleJitterPatch(){
    delete(circularBuffer);
  }

  
private:
  // Parameter ramping to reduce clicks.
  
  float oldVal[4];
  float ramp; // 0..1
  
  float getRampedParameterValue(PatchParameterId id){
    float val = getParameterValue(id);
    float result = val * ramp + oldVal[id] * (1-ramp);
    oldVal[id] = val;
    return result;
  }
};

#endif // OwlSim_SampleJitterPatch_hpp
