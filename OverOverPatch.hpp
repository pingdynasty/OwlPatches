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

/* created by the OWL team 2014 */


////////////////////////////////////////////////////////////////////////////////////////////////////
/*An overdrive with 4x oversampling and BigMuff-like tone control*/

#ifndef __OverOverPatch_hpp__
#define __OverOverPatch_hpp__
#include "OversampledOverdrive.hpp"
#include "ToneFilter.hpp"
#include "StompBox.h"
class OverOverPatch : public Patch {
private:
  OversampledOverdrive os;
  ToneFilter tf;
  float* buf[2];
public:
  OverOverPatch(): tf(getSampleRate()){
    registerParameter(PARAMETER_A, "Drive");
    registerParameter(PARAMETER_B, "Offset");
    registerParameter(PARAMETER_C, "Tone");
    registerParameter(PARAMETER_D, "Level");
    registerParameter(PARAMETER_E, "DrivePedal");
    os=OversampledOverdrive();
  }
  void processAudio(AudioBuffer &buffer){
    static float drive_=0;
    static float offset_=0;
    static float level_=0;
    static float tone_=0;
    float expr = 1 - getParameterValue(PARAMETER_E);
    float drive = expr*getParameterValue(PARAMETER_A);
    float offset = getParameterValue(PARAMETER_B);
    float tone = getParameterValue(PARAMETER_C);
    float level = getParameterValue(PARAMETER_D);
    float input;
    float output;
    offset /= 10.f;
    drive += 0.03f; //make sure we don't kill the signal when drive=0
    drive *= 40.f;
    level*= .5f;
    int size = buffer.getSize();
    float numCh=1; //Use only one channel until oversampling becomes less expensive. Should instead be numCh=buffer.getChannels();numCh= numCh<=2 ? numCh : 2; 
    for (int ch = 0; ch<numCh; ch++) { //get pointers to the buffers. 
      buf[ch] = buffer.getSamples(ch);
    }
    tf.setTone(tone_);
    for (int i = 0; i < size; i++) { //process each sample
      drive_=drive*0.001f + drive_*0.999f; //parameter smoothing
      offset_=offset*0.001f + offset_*0.999f; //parameter smoothing
      tone_=tone*0.001f + tone_*0.999f; //parameter smoothing
      level_=level*0.001f + level_*0.999f; //parameter smoothing
      for(int ch=0;ch<numCh; ch++) {
        input=buf[ch][i]; //average the inputs
        input = (input + offset_)*drive_;
        output = os.processSample(input);
        output=tf.processSample(output,0);
        buf[ch][i] = level_*output;
      }
    }
  }
};

#endif // __OverOverPatch_hpp__
