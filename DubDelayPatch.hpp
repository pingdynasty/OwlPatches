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

//#ifndef __DubDelayPatch_hpp__
//#define __DubDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#include "math.h"
#include "ToneFilter.hpp"

//#define REQUEST_BUFFER_SIZE 262144
#define REQUEST_BUFFER_SIZE 32768

#define MAXDELAYS 2.5
#define MINDELAYS 0.05

class DubDelayPatch : public Patch {
    
private:
    CircularBuffer delayBuffer;
//    unsigned int delayTime;
    ToneFilter tf;
    // float x[2] = {0};
    // float y[2] = {0};
    // float a[3] = {0};
    // float b[3] = {0};
    float fs;
    float delayTime = 0;
    float tone = 0;
    
public:
    DubDelayPatch() {
        registerParameter(PARAMETER_A, "Time");
        registerParameter(PARAMETER_B, "Feedback");
        registerParameter(PARAMETER_C, "Tone");
        registerParameter(PARAMETER_D, "Wet");
//        static float delayTime = 0;
//        static float tone = 0;
        AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
        delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
        fs = getSampleRate();
        
    }
    void processAudio(AudioBuffer &buffer) {
//        fs = getSampleRate();

        float feedback, tone, wet, _delayTime, _tone, delaySamples;


        _delayTime = getParameterValue(PARAMETER_A);
        feedback = 2*getParameterValue(PARAMETER_B);
        _tone = getParameterValue(PARAMETER_C);
        wet = getParameterValue(PARAMETER_D);

        tone = 0.05*_tone + 0.95*tone;
        tf.setTone(tone);

        float* buf = buffer.getSamples(0);
        for (int i = 0 ; i < buffer.getSize(); i++) {
            delayTime = 0.01*_delayTime + 0.99*delayTime;
            delaySamples = delayTime * (delayBuffer.getSize()-1);
            // buf[i] = (buf[i] * (1-wet) ) + wet * delayBuffer.read(delaySamples);
            buf[i] += wet * delayBuffer.read(delaySamples);
            // filtered = tf.processSample(feedback * buf[i],0)
            delayBuffer.write(dist(tf.processSample(feedback * buf[i],0)));
        }
    }
    float dist(float x){ 		// Overdrive curve
        if (x<-3)
            return -1;
        else if (x>3)
            return 1;
        else
            return x * ( 27 + x*x ) / ( 27 + 9*x*x );
    }

};





////////////////////////////////////////////////////////////////////////////////////////////////////
