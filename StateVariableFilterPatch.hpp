#ifndef __StateVariableFilterPatch_hpp__
#define __StateVariableFilterPatch_hpp__

#include "SampleBasedPatch.hpp"

/**
 * State Variable Filter
 * http://musicdsp.org/showArchiveComment.php?ArchiveID=23
 * Type : 12db resonant low, high or bandpass
 * References : Effect Design Part 1, Jon Dattorro, J. Audio Eng. Soc., Vol 45, No. 9, 1997 September
 * Notes : 
 * Digital approximation of Chamberlin two-pole low pass. 
 * Easy to calculate coefficients, easy to process algorithm.
*/
class StateVariableFilterPatch : public SampleBasedPatch {
private:
  float low, band;
  float f, q;
  float ratio;
  float gain;
public:
  StateVariableFilterPatch() : low(0), band(0) {
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Resonance");
    registerParameter(PARAMETER_C, "Low/High");
    registerParameter(PARAMETER_D, "Gain");
}
  void prepare(){
    float fc;
    fc = getParameterValue(PARAMETER_A);
    q = getParameterValue(PARAMETER_B);
    ratio = getParameterValue(PARAMETER_C);
    gain = getParameterValue(PARAMETER_D)*2;
    fc /= 2;
    f = sin(M_PI * fc);
    q = 1 - q;
  }
  float processSample(float sample){
    low = low + f * band;
    float high = q * sample - low - q*band;
    band = f * high + band;
    return gain*(low*(1.0-ratio) + high*ratio);
  }
};

#endif /* __StateVariableFilterPatch_hpp__ */
