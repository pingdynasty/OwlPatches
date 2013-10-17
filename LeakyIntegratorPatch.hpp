#ifndef __LeakyIntegratorPatch_hpp__
#define __LeakyIntegratorPatch_hpp__

#include "SampleBasedPatch.hpp"

/*
 * Leaky integrator
 * y[n] = (1-lambda)*x[n] + lambda*y[n-1]
 */
class LeakyIntegratorPatch : public SampleBasedPatch {
private:
  float lambda, gain;
  float y1;
public:
  LeakyIntegratorPatch() : y1(0.0) {
    registerParameter(PARAMETER_A, "Lambda");
    registerParameter(PARAMETER_D, "Gain");
  }
  void prepare(){
    lambda = getParameterValue(PARAMETER_A);
    gain = getParameterValue(PARAMETER_D);
  }
  float processSample(float sample){
    y1 = lambda*y1 + (1-lambda)*sample;
    return gain*y1;
  }
};

#endif /* __LeakyIntegratorPatch_hpp__ */
