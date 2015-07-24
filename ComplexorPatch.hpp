#ifndef __ComplexorPatch_hpp__
#define __ComplexorPatch_hpp__

#include "StompBox.h"

/*
 * Complexor
 * Chaotic waveshaper and dynamics processor based on iterated logistic map
 * http://en.wikipedia.org/wiki/Logistic_map
 * http://en.wikipedia.org/wiki/Feigenbaum_constants
 * http://mathworld.wolfram.com/LogisticMap.html
 */
class ComplexorPatch : public Patch {
private:
  const float minR = 2.0;
  const float maxR = 4.0;
  const float maxI = 20.0;
public:

  ComplexorPatch(){
    registerParameter(PARAMETER_A, "Input");
    registerParameter(PARAMETER_B, "Iterations");
    registerParameter(PARAMETER_C, "Chaos");
    registerParameter(PARAMETER_D, "Dry/Wet");
  }

  /* the logistic map */
  float lf(float x, float r){
    return r*x*(1-x);
  }

  float ilf(float nf, float x, float r){
    unsigned int n = floor(nf);
    nf = nf - n;
    while(n--)
      x = lf(x, r);
    float xn = lf(x, r);
    return (x*(1.0-nf) + xn*nf)/2;
  }

  float processSample(float x, float iterations, float r){
    if(x < 0.0)
      return -ilf(iterations, -x, r);
    else
      return ilf(iterations, x, r);
  }

  void processAudio(AudioBuffer &buffer){
    float gain = getParameterValue(PARAMETER_A);
    gain = gain*gain*2.0;
    float iterations = getParameterValue(PARAMETER_B);
    float r = getParameterValue(PARAMETER_C)*(maxR-minR) + minR;
    iterations = iterations*iterations*maxI;
    int size = buffer.getSize();
    FloatArray left = buffer.getSamples(LEFT_CHANNEL);
    FloatArray right = buffer.getSamples(RIGHT_CHANNEL);
    float wet = getParameterValue(PARAMETER_D);
    for(int i=0; i<size; i++){
      left[i] = processSample(gain*left[i], iterations, r) * wet + left[i]*(1-wet);
      right[i] = processSample(gain*right[i], iterations, r) * wet + right[i]*(1-wet);
    }
  }
};

#endif // __ComplexorPatch_hpp__
