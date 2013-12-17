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


#ifndef __TestTonePatch_hpp__
#define __TestTonePatch_hpp__

#include "StompBox.h"

class TestTonePatch : public Patch {
private:
  float leftpos, rightpos;
public:
  TestTonePatch() : leftpos(0.0f), rightpos(0.0f) {
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Amplitude");
    registerParameter(PARAMETER_C, "");
    registerParameter(PARAMETER_D, "");
  }
  void processAudio(AudioBuffer &buffer) {
    float frequency = getParameterValue(PARAMETER_A) * 10000;
    float amplitude = getParameterValue(PARAMETER_B) * 2;
    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    float linc = frequency/getSampleRate();
    float rinc = 1000/getSampleRate();
    int size = buffer.getSize();
    for(int n = 0; n<size; n++){
      left[n] = sinf(2*M_PI*leftpos) * amplitude;
      right[n] = sinf(2*M_PI*rightpos) * 0.5;
      if((leftpos += linc) > 1.0f)
	leftpos -= 1.0f;
      if((rightpos += rinc) > 1.0f)
	rightpos -= 1.0f;
    }
  }
};

#endif   // __TestTonePatch_hpp__
