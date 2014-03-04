#ifndef __ChorusPatch_hpp__
#define __ChorusPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"

#define REQUEST_BUFFER_SIZE 32768

class ChorusPatch : public Patch {
private:
    
    CircularBuffer delayBuffer;
    
    int32_t oldReadIndex1;
    int32_t oldReadIndex2;
    int32_t oldReadIndex3;
    float angle;
    float maxDelay;
public:
  ChorusPatch(){
     
    registerParameter(PARAMETER_A, "RATE");
      registerParameter(PARAMETER_B, "DEPTH");
        registerParameter(PARAMETER_C, "MIX");
      registerParameter(PARAMETER_D, "VOICES");
      
    
    AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
    delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
      angle = 0.f;
      maxDelay =  0.05 * delayBuffer.getSize();// 50ms, anything more and it will be a noticeable echo
      

  }
    
    
    
    
  void processAudio(AudioBuffer &buffer){
    
      int numVoices = 1;
      float twoVoices = 1.0/3;
      float threeVoices = 2 * (1.0/3);
      int32_t voiceWidth = 0;
      float rate, depth, mix, voicesParam;
      double sampleRate = getSampleRate();
      rate = getParameterValue(PARAMETER_A);
      depth = getParameterValue(PARAMETER_B) * maxDelay; // get the depth relative to the maximum delay
      mix   = getParameterValue(PARAMETER_C);
      voicesParam = getParameterValue(PARAMETER_D);

      float sineWave;
      float* x = buffer.getSamples(0);
      int size = buffer.getSize();
      if (voicesParam < twoVoices)
      {
          numVoices = 1;
      }
      if(voicesParam >= twoVoices && voicesParam < threeVoices)
      {
          numVoices = 2;
          voiceWidth = 20 * (threeVoices - voicesParam);
        
      }
      if(voicesParam >=threeVoices && voicesParam <= 1)
      {
          numVoices = 3;
          voiceWidth = 20 * (1.0 - voicesParam);
          
      }
      
      //================LFO=================//
      float inc = rate  * (2 * M_PI) *buffer.getSize()/sampleRate ; // The incrementing sine is calculate once per block
      sineWave =  sinf(angle) * depth/2 ;
      angle += inc;
      if (angle > 2 * M_PI)
      {
          angle -= 2 * M_PI;
      }
    //===============================//
      int32_t newReadIndex1 =  0;
      int32_t newReadIndex2 =  0;
      int32_t newReadIndex3 =  0;
      
      switch (numVoices)
      {
          case 1:
          {
              newReadIndex1 = sineWave;
              break;
          }
          case 2:
          {
              newReadIndex1 = sineWave;
              newReadIndex2 = sineWave + voiceWidth;
              if (newReadIndex2 > maxDelay)
              {
                  newReadIndex2 = maxDelay - (newReadIndex2 - maxDelay);
              }
              break;
          }
          case 3:
          {
              newReadIndex1 = sineWave;
              newReadIndex2 = sineWave + voiceWidth;
              if (newReadIndex2 > maxDelay)
              {
                  newReadIndex2 = maxDelay - (newReadIndex2 - maxDelay);
              }
              newReadIndex1 = sineWave;
              newReadIndex2 = sineWave + voiceWidth;
              if (newReadIndex2 > maxDelay)
              {
                  newReadIndex2 = maxDelay - (newReadIndex2 - maxDelay);
              }
              newReadIndex3 = sineWave - voiceWidth;
              if (newReadIndex3 < 0)
              {
                  newReadIndex3 = 0 + voiceWidth;
              }
              break;
          }
          default:
          {
              int32_t newReadIndex1 = sineWave;
              break;
          }

      }
      newReadIndex1 = sineWave;
      
      for (int n = 0; n < size; n++)
      {
                    delayBuffer.write(x[n]);
          if( abs(x[n]) > 1.0f)
          {
              float inputValue = x[n]; //clipping check
          }
          
          
          float voice1 = (delayBuffer.read(maxDelay/2 + oldReadIndex1)*(size-1-n) + delayBuffer.read(maxDelay/2 + newReadIndex1)*n)  * mix/size + (1.0 - mix) * x[n];
          float voice2 = (delayBuffer.read(maxDelay/2 + oldReadIndex2)*(size-1-n) + delayBuffer.read(maxDelay/2 + newReadIndex2)*n)  * mix/size + (1.0 - mix) * x[n];
          float voice3 = (delayBuffer.read(maxDelay/2 + oldReadIndex3)*(size-1-n) + delayBuffer.read(maxDelay/2 + newReadIndex3)*n)  * mix/size + (1.0 - mix) * x[n];
          x[n] = voice1+voice2+voice3 ;
          

          if( abs(x[n]) > 1.0f)
          {
              float outputValue = x[n];//clipping check
          }
          
      }
      
       oldReadIndex1 = newReadIndex1;
       oldReadIndex2 = newReadIndex2;
       oldReadIndex3 = newReadIndex3;
      // put your code here!
  }
};

#endif //  __ChorusPatch_hpp__