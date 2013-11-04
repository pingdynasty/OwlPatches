#ifndef __SimpleDelayPatch_hpp__
#define __SimpleDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

#define REQUEST_BUFFER_SIZE 32768

class SimpleDelayPatch : public Patch {
    
private: 
    
    CircularBuffer delayBuffer;
    float delayTime, feedback, wetDry, olddelaySamples = 0, dSamples;
    
public:
    SimpleDelayPatch() : olddelaySamples(0.0f) {
        
        AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
        delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
        registerParameter(PARAMETER_A, "Delay");
        registerParameter(PARAMETER_B, "Feedback");
        registerParameter(PARAMETER_C, "Dry/Wet");
    }
    
    void processAudio(AudioBuffer &buffer) {
        
        const int size = buffer.getSize();          // samples in block
        
        delayTime          = getParameterValue(PARAMETER_A); // delay time
        feedback           = getParameterValue(PARAMETER_B); // delay feedback
        wetDry             = getParameterValue(PARAMETER_C); // wet/dry balance
        float delaySamples = delayTime * (delayBuffer.getSize()-1);
        
        for (int ch = 0; ch<buffer.getChannels(); ++ch) {   //for each channel
            
            float* buf = buffer.getSamples(ch);
            float y;
            
            for (int i = 0; i < size; ++i) {  //for each sample
                
                //linear interpolation for delaySamples
                dSamples = olddelaySamples + (delaySamples - olddelaySamples) * i / size;
                
                y = buf[i] + feedback * delayBuffer.read(dSamples);      //delay
                buf[i] = wetDry * y + (1.f - wetDry) * buf[i];           //wet/dry balance
                delayBuffer.write(buf[i]);
                
            }
        }
        
        olddelaySamples = delaySamples;
        
    }
    
};

#endif   // __SimpleDelayPatch_hpp__
