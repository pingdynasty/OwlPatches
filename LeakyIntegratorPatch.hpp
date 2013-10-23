#ifndef __LeakyIntegratorPatch_hpp__
#define __LeakyIntegratorPatch_hpp__

#include "SampleBasedPatch.hpp"

class LeakyIntegratorPatch : public SampleBasedPatch {
private:
// leaky integrator
// y[n] = (1-lambda)*x[n] + lambda*y[n-1]
// dc removal
// y[n] = lambda*y[n-1] + x[n] - x[n-1]
  float lambda, gain;
  float y1;
public:
  LeakyIntegratorPatch() : y1(0.0) {
    registerParameter(PARAMETER_A, "Lambda");    
    registerParameter(PARAMETER_A, "Gain");    
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
