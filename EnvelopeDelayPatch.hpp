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

//#ifndef __EnvelopeDelayPatch_hpp__
//#define __EnvelopeDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#include "math.h"

#define ENVELOPEDELAY_BUFFER_SIZE 32768
#ifndef LPF_10HZ_B
#define LPF_10HZ_B 0.0003561
#define LPF_10HZ_A 0.9993
#endif
#define __EnvelopeDelayPatch_hpp__



class EnvelopeDelayPatch : public Patch {
    
private:
    CircularBuffer v0Buf, v1Buf, v2Buf;
    int bufferFlag = 0;
    int changeBuffer = -1;
    
    float delayTime = 0;
    float _thresh = 0;
    float feedback[3] = {0.05, 0.3, 0.85};
    float prevZCRSample = 0;
    float pxSample = 0, pySample = 0;
    float thresh[2] = {0.1, 0.01};
    
public:
    EnvelopeDelayPatch() {
        registerParameter(PARAMETER_A, "Configuration");
        registerParameter(PARAMETER_B, "Thresholds");
        registerParameter(PARAMETER_C, "Time");
        registerParameter(PARAMETER_D, "Mix");
        
        AudioBuffer* buffer = createMemoryBuffer(1, ENVELOPEDELAY_BUFFER_SIZE);
        v0Buf.initialise(buffer->getSamples(0), ENVELOPEDELAY_BUFFER_SIZE);
        v1Buf.initialise(buffer->getSamples(0), ENVELOPEDELAY_BUFFER_SIZE);
        v2Buf.initialise(buffer->getSamples(0), ENVELOPEDELAY_BUFFER_SIZE);
    }
    
    int detectZCR(float sample){
        if((sample < 0 && prevZCRSample > 0)||(sample > 0 && prevZCRSample < 0))
            return 1;
        else
            return 0;
    };
    
    void setBufferFlag(float sample){
        float env = fabs(LPF_10HZ_B*sample + LPF_10HZ_B*pxSample + LPF_10HZ_A*pySample) * _thresh;
        if(bufferFlag == 0  && env < thresh[0]){
            changeBuffer = 1;
        }
        else if(bufferFlag == 2 && env > thresh[1]){
            changeBuffer = 1;
        }
        else{
            if (env < thresh[1]){
                changeBuffer = 2;
            }
            else if (env > thresh[0]){
                changeBuffer = 0;
            }
        }
    };
    
    void setConf(float conf){
        if (conf < 0.3){
            feedback[0] = 0.05;
            feedback[1] = 0.30;
            feedback[2] = 0.85;

        }
        else if (conf > 0.7){
            feedback[0] = 0.85;
            feedback[1] = 0.30;
            feedback[2] = 0.05;

        }
        else{
            feedback[0] = 0.3;
            feedback[1] = 0.3;
            feedback[2] = 0.3;
        }
    };

    
    
    void processAudio(AudioBuffer &buffer) {
        float conf, _time, mix, _mix;
        int delaySamples;

        conf = getParameterValue(PARAMETER_A);
        _thresh = getParameterValue(PARAMETER_B) * 4;
        _time = getParameterValue(PARAMETER_C);
        mix = getParameterValue(PARAMETER_D);
        _mix = 1-mix;
        setConf(conf);

        float* buf = buffer.getSamples(0);
        for (int i = 0 ; i < buffer.getSize(); i++) {
            delayTime = 0.01*_time + 0.99*delayTime;
            delaySamples = (int)(delayTime * (ENVELOPEDELAY_BUFFER_SIZE-1));
            setBufferFlag(buf[i]);
            if(changeBuffer >= 0){
                if(detectZCR(buf[i])){
                    bufferFlag = changeBuffer;
                    changeBuffer = -1;
                }
                prevZCRSample = buf[i];
            }
            
            if(bufferFlag == 2)
                v2Buf.write(feedback[2] * (v2Buf.read(delaySamples) + buf[i]));
            else if(bufferFlag == 1)
                v1Buf.write(feedback[1] * (v1Buf.read(delaySamples) + buf[i]));
            else if(bufferFlag == 0)
                v0Buf.write(feedback[0] * (v0Buf.read(delaySamples) + buf[i]));
            
            buf[i] = mix * 0.5 * (v0Buf.read(delaySamples) + v1Buf.read(delaySamples) + v2Buf.read(delaySamples))+ _mix * buf[i];

        }
        
    }
};





////////////////////////////////////////////////////////////////////////////////////////////////////
