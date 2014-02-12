//
//  BiasedDelayPatch.hpp
//
//  A basic delay effect with sample bias modulation.
//  This can produce great nonlinear saturation effects, however
//  its behaviour is highly dependent on the audio source.
//
//  Created by martind on 15/06/2013.
//  http://github.com/dekstop/OwlSim/tree/test/Source/Patches
//
//  Parameter assignment:
//  - A: delay time
//  - B: feedback
//  - C: bias: low..high
//  - D: dry/wet mix
//
//  TODO:
//  - see SimpleDelayPatch.hpp
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

#ifndef OwlSim_BiasedDelayPatch_hpp
#define OwlSim_BiasedDelayPatch_hpp

#include "StompBox.h"

class BiasedDelayPatch : public Patch {
  
  const float MIN_DELAY; // in seconds
  const float MAX_DELAY;
  
  const float MIN_BIAS;
  const float MED_BIAS;
  const float MAX_BIAS;

  AudioBuffer* delayBuffer;
  unsigned int delayBufIdx;
  unsigned int oldDelayInSamples;
  
public:
  BiasedDelayPatch() : MIN_DELAY(0.01), MAX_DELAY(2), MIN_BIAS(0.8), MED_BIAS(1), 
    MAX_BIAS(1.5), delayBuffer(NULL), delayBufIdx(0) {
    registerParameter(PARAMETER_A, "Delay");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "Bias");
    registerParameter(PARAMETER_D, "Dry/Wet");

    delayBuffer = createMemoryBuffer(2, MAX_DELAY * getSampleRate());
  }

  void processAudio(AudioBuffer &buffer){

    unsigned int delayInSamples = getDelayInSamples(PARAMETER_A);
    float feedback = getParameterValue(PARAMETER_B);
    float bias = getBiasExponent(PARAMETER_C);
    float dryWetMix = getParameterValue(PARAMETER_D);
    
    int bufSize = buffer.getSize();
    int delayBufSize = delayBuffer->getSize();

    for (int ch = 0; ch<buffer.getChannels(); ++ch)
    {
      float* buf = buffer.getSamples(ch);
      float* delayBuf = delayBuffer->getSamples(ch);

      unsigned int writeIdx = delayBufIdx;
      int readIdx = delayBufIdx - delayInSamples;
      while (readIdx<0) readIdx += delayBufSize;
      int oldReadIdx = delayBufIdx - oldDelayInSamples;
      while (oldReadIdx<0) oldReadIdx += delayBufSize;

      for (int i=0; i<bufSize; ++i)
      {
        float delaySample = linearBlend(delayBuf[oldReadIdx], delayBuf[readIdx], (float)i/bufSize);
        float v = buf[i] + delaySample * feedback;
        v = applyBias(v, bias);
        delayBuf[writeIdx] = min(1, max(-1, v)); // Guard: hard range limits.
        buf[i] = linearBlend(buf[i], delaySample, dryWetMix);
    
        writeIdx = (++writeIdx) % delayBufSize;
        readIdx = (++readIdx) % delayBufSize;
      }
    }
    delayBufIdx = (delayBufIdx + bufSize) % delayBufSize;
    oldDelayInSamples = delayInSamples;
  }
  
private:
  
  unsigned int getDelayInSamples(PatchParameterId id){
    unsigned int minDelayInSamples = getSampleRate() * MIN_DELAY;
    return minDelayInSamples + getParameterValue(id) * (delayBuffer->getSize() - minDelayInSamples);
  }
  
  // Mapping p1 parameter ranges so that:
  // - full-left (0) is "low bias"
  // - centre (0.5) is "no bias"
  // - full-right (1.0) is "high bias"
  float getBiasExponent(PatchParameterId id){
    float p1 = 1 - getParameterValue(id);
    if (p1 < 0.5)
    { // min .. med
      p1 = p1 * 2; // [0..1] range
      return p1*p1 * (MED_BIAS-MIN_BIAS) + MIN_BIAS;
    } else
    { // med .. max
      p1 = (p1 - 0.5) * 2; // [0..1] range
      return p1*p1 * (MAX_BIAS-MED_BIAS) + MED_BIAS;
    }
  }
  
  float applyBias(float v, float bias){
    return
      powf(fabs(v), bias) * // bias
      (v < 0 ? -1 : 1);    // sign
  }
  
  // Fade from a to b, over mix range [0..1]
  float linearBlend(float a, float b, float mix){
    return a * (1 - mix) + b * mix;
  }
};

#endif // OwlSim_SimpleBiasedDelayPatch_hpp
