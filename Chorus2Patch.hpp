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

/* created by Dave Moffat, the OWL team 2014 */


////////////////////////////////////////////////////////////////////////////////////////////////////

//#ifndef __Chorus2Patch_hpp__
//#define __Chorus2Patch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#include "math.h"
#include "ToneFilter.hpp"

//#define CHORUS2_BUFFER_SIZE 32768
#define CHORUS2_BUFFER_SIZE 2205
#ifndef TWOPI
#define TWOPI 6.28318530717959
#endif
#ifndef M_PI
#define M_PI 3.141592653589793238468
#endif
#ifndef TWOPI_BY_SAMPLERATE
#define TWOPI_BY_SAMPLERATE 0.00014247585731
#endif
#define __Chorus2Patch_hpp__


class Chorus2Patch : public Patch {
    
private:
    CircularBuffer v1Buf, v2Buf, v3Buf;
//    AudioBuffer* v1B, v2B, v3B;
//    unsigned int delayTime;
    ToneFilter tf;
    int bufferSize = 0;
    int prevDelay[3] = {0};

    float delayTime = 0;
    float phase[3] = {0};
    float fs;
//    float v1Phi = M_PI/2, v2Phi = M_PI/2, v3Phi = M_PI/2;
//    int wP = 0;
//    float invFS;

//    float gain = 1;
//    float gain_ = 1;
//    int n;
//    float prevX[2] = {0,0};
//    float prevY[2] = {0,0};
//    int p = 0;
//    int delay;
    
    
    
public:
    Chorus2Patch() {
        registerParameter(PARAMETER_A, "Rate");
        registerParameter(PARAMETER_B, "Depth");
        registerParameter(PARAMETER_C, "Tone");
        registerParameter(PARAMETER_D, "Mix");
        AudioBuffer* buffer = createMemoryBuffer(1, CHORUS2_BUFFER_SIZE);

        bufferSize = buffer-> getSize();
        v1Buf.initialise(buffer->getSamples(0), bufferSize);
        v2Buf.initialise(buffer->getSamples(0), bufferSize);
        v3Buf.initialise(buffer->getSamples(0), bufferSize);
        fs = getSampleRate();
        
    }
    float phaseCalc(float rate, int voiceIndex) {
        phase[voiceIndex] += rate;
        if ( phase[voiceIndex] >= 1.0 ) {
            phase[voiceIndex] -= 1.0;
        }
        return sinf(phase[voiceIndex]*(2*M_PI));    //sine function
    };
//    float interpDelay(float delayValue, int index, int sampleIndex) {
//        prevDelay[index] += (delayValue - prevDelay[index]) * sampleIndex / bufferSize;
//        return prevDelay[index];
//
//    };
    void processAudio(AudioBuffer &buffer) {
        float depth, tone, mix, _delayTime, chorusSum, rate;
        float toneSample;
        float delaySamples;

        _delayTime = getParameterValue(PARAMETER_A);
        depth = getParameterValue(PARAMETER_B);
        tone = getParameterValue(PARAMETER_C);
        mix = getParameterValue(PARAMETER_D);
        tf.setTone(tone);

        delayTime = 0.01*_delayTime + 0.99*delayTime;
        delaySamples = (delayTime * (bufferSize-1));
        rate = depth*3+2;

        float* buf = buffer.getSamples(0);
        for (int i = 0 ; i < buffer.getSize(); i++) {
            toneSample = tf.processSample(buf[i],0);
            v1Buf.write(toneSample);
            v2Buf.write(toneSample);
            v3Buf.write(toneSample);
//            chorusSum = v1Buf.read(delaySamples) + v2Buf.read(delaySamples*0.7) + v3Buf.read(delaySamples*0.4);
            chorusSum = v1Buf.readInterp(phaseCalc(rate,0)*delaySamples) + v2Buf.readInterp(phaseCalc(rate*0.7,1)*delaySamples) + v3Buf.readInterp(phaseCalc(rate*0.4,2)*delaySamples);
            buf[i] = buf[i] * (1-mix) + chorusSum * (mix);
        }

        
        
        
    }
};





////////////////////////////////////////////////////////////////////////////////////////////////////
