#ifndef __ResonantFilterPatch_hpp__
#define __ResonantFilterPatch_hpp__

#include "SampleBasedPatch.hpp"

/**
 * Resonant Low pass filter
 * http://www.musicdsp.org/archive.php?classid=3#29
 * References : Posted by Paul Kellett
 * Notes : 
 * This filter consists of two first order low-pass filters in
 * series, with some of the difference between the two filter
 * outputs fed back to give a resonant peak.
 * You can use more filter stages for a steeper cutoff but the
 * stability criteria get more complicated if the extra stages
 * are within the feedback loop.
 */
class ResonantFilterPatch : public SampleBasedPatch {
private:
  float buf0, buf1;
  float q, f, fb;
public:
  ResonantFilterPatch() :buf0(0), buf1(0) {
    registerParameter(PARAMETER_A, "Frequency");
    registerParameter(PARAMETER_B, "Resonance");
  }
  void prepare(){
    f = getParameterValue(PARAMETER_A);
    q = getParameterValue(PARAMETER_B);
    fb = q + q/(1.0 - f);
  }
  float processSample(float sample){
    //for each sample...
    buf0 = buf0 + f * (sample - buf0 + fb * (buf0 - buf1));
    buf1 = buf1 + f * (buf0 - buf1);
    return buf1;
  }
};

#endif /* __ResonantFilterPatch_hpp__ */
