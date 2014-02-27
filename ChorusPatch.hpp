#ifndef __ChorusPatch_hpp__
#define __ChorusPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

#define REQUEST_BUFFER_SIZE 32768

class ChorusPatch : public Patch {
private:
    CircularBuffer delayBuffer;
    int32_t oldReadIndex;
    
    float angle;
    float maxDelay;
public:
  ChorusPatch(){
     
    registerParameter(PARAMETER_A, "RATE");
      registerParameter(PARAMETER_B, "DEPTH");
        registerParameter(PARAMETER_C, "MIX");
      
    
    AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
    delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
      angle = 0.f;
      maxDelay =  0.05 * delayBuffer.getSize();

  }
    
    
    
    
  void processAudio(AudioBuffer &buffer){
    
      float rate, depth, mix;
      double sampleRate = getSampleRate();
      rate = getParameterValue(PARAMETER_A);
      depth = getParameterValue(PARAMETER_B) * maxDelay; // get the depth relative to the maximum delay
      mix   = getParameterValue(PARAMETER_C);
      float sineWave;
      
      float* x = buffer.getSamples(0);
      int size = buffer.getSize();
      
      float inc = rate  * (2 * M_PI) *buffer.getSize()/sampleRate ; // The incrementing sine is calculate once per block
      sineWave =  sinf(angle) * depth/2 ;
      angle += inc;
      if (angle > 2 * M_PI)
      {
          angle -= 2 * M_PI;
      }
      int32_t newReadIndex = sineWave;
    
      

      
      for (int n = 0; n < size; n++)
      {
                    delayBuffer.write(x[n]);
          if( abs(x[n]) > 1.0f)
          {
              float inputValue = x[n];
          }
          
          x[n] = (delayBuffer.read(maxDelay/2 + oldReadIndex)*(size-1-n) + delayBuffer.read(maxDelay/2 + newReadIndex)*n)  * mix/size + (1.0 - mix) * x[n];
          

          if( abs(x[n]) > 1.0f)
          {
              float outputValue = x[n];
          }
          
      }
      
       oldReadIndex = newReadIndex;
      
      // put your code here!
  }
};

#endif //  __ChorusPatch_hpp__