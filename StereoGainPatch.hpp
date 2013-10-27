#ifndef __StereoGainPatch_h__
#define __StereoGainPatch_h__

#include "StompBox.h"

class StereoGainPatch : public Patch {
public:
  void processAudio(AudioBuffer &buffer){
    assert_param(buffer.getChannels() > 1);
    float gainL = getParameterValue(PARAMETER_A)*2;
    float gainR = getParameterValue(PARAMETER_B)*2;
    int size = buffer.getSize();
    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    for(int i=0; i<size; ++i){
	left[i] = gainL*left[i];
	right[i] = gainR*right[i];
    }
  }
};

#endif // __StereoGainPatch_h__
