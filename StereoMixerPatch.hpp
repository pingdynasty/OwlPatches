#ifndef __StereoMixerPatch_h__
#define __StereoMixerPatch_h__

#include "StompBox.h"

class StereoMixerPatch : public Patch {
public:
  void processAudio(AudioBuffer &buffer){
//     assert_param(buffer.getChannels() > 1);
    float gainLL = getParameterValue(PARAMETER_A);
    float gainLR = getParameterValue(PARAMETER_B);
    float gainRL = getParameterValue(PARAMETER_C);
    float gainRR = getParameterValue(PARAMETER_D);
    int size = buffer.getSize();
    float* left = buffer.getSamples(0);
    float* right = buffer.getSamples(1);
    float l, r;
    for(int i=0; i<size; ++i){
      l = gainLL*left[i] + gainLR*right[i];
      r = gainRL*left[i] + gainRR*right[i];
      left[i] = l;
      right[i] = r;
    }
  }
};

#endif // __StereoMixerPatch_h__
