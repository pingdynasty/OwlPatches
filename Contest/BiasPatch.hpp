//
//  BiasPatch.hpp
//
//  A saturation/distortion effect: adjust amplitude bias while retaining high peaks.
//  Variable low/no/high sample bias; this works best on normalised signals.
//
//  Created by martind on 14/06/2013.
//  http://github.com/dekstop/OwlSim/tree/test/Source/Patches
//
//  Parameter assignment:
//  - A: bias: low..high
//  - B:
//  - C:
//  - D: dry/wet mix
//  - Push-button:
//
//  TODO:
//  - adjust to varying signal amplitude (don't just kill quiet signals at low bias values)
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

#ifndef __BiasPatch_hpp__
#define __BiasPatch_hpp__

#include "StompBox.h"

class BiasPatch : public Patch {

  const float MIN_BIAS;
  const float MED_BIAS;
  const float MAX_BIAS;

public:
  BiasPatch() : MIN_BIAS(0.1), MED_BIAS(1), MAX_BIAS(6), ramp(0.1) 
  {
    registerParameter(PARAMETER_A, "Bias");
    registerParameter(PARAMETER_D, "Dry/Wet");
    memset(oldVal, 0, sizeof(oldVal));
  }
  void processAudio(AudioBuffer &buffer)
  {
    float bias = getBias(1 - getRampedParameterValue(PARAMETER_A));
    float dryWetMix = getRampedParameterValue(PARAMETER_D);

    int size = buffer.getSize();
    
	for(int ch = 0; ch<buffer.getChannels(); ++ch)
	 {
	     float* buf = buffer.getSamples(ch);
	     for (int i=0; i<size; ++i)
	     {
		 float v =
		      powf(fabs(buf[i]), bias) * // bias
			(buf[i] < 0 ? -1 : 1);    // sign
		      buf[i] =
			v * dryWetMix +
			buf[i] * (1 - dryWetMix);
	      }
		     
	 }
  }

private:

  // Mapping p1 parameter ranges so that:
  // - full-left (0) is "low bias"
  // - centre (0.5) is "no bias"
  // - full-right (1.0) is "high bias"
  float getBias(float p1){
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
  
  // Parameter ramping to reduce clicks.

  float oldVal[4];
  float ramp; // 0..1

  float getRampedParameterValue(PatchParameterId id) {
    float val = getParameterValue(id);
    float result = val * ramp + oldVal[id] * (1-ramp);
    oldVal[id] = val;
    return result;
  }
};

#endif // __BiasPatch_hpp__
