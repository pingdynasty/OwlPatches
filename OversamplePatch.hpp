/*
This patch does not do anything, only upsamples and downsamples the input signal. Used for computational cost measurements.
*/

#ifndef __OversamplePatch_hpp__
#define __OversamplePatch_hpp__
// #include "Oversample.hpp"
#include "StompBox.h"
#include "BiquadFilter.h"


/**
   Implements 4x oversampling
*/
class Oversampler {
private:
  BiquadFilter upfilter;
  BiquadFilter downfilter;
  float* oversampled;
public:
  Oversampler(int blocksize) : upfilter(2), downfilter(2) {
    /*   [b,a] = ellip(2, 2, 70, 19200/(48000*4/2)) */
//     static float coeffs[] = { 5.14483600142e-02, 1.01958385114e-01, 5.14483600142e-02, -1.35468949025e+00, 6.12586787851e-01 };
    /* Convert series second-order sections to direct form
     * output_precision(6)
     * [b,a] = ellip(4, 2, 70, 19200/(48000*4/2))
     * [sos,g] = tf2sos(b,a)
     * b0          b1          b2          a0         a1           a2
     * 1.0000000   1.4157500   1.0000000   1.0000000  -1.5499654   0.8890431  first biquad
     * 1.0000000   0.0467135   1.0000000   1.0000000  -1.6359692   0.7189533  second biquad
     */
    static float coeffs[] = { 
      /* [b,a] = ellip(4, 3, 40, 19200/(48000*4/2)); [sos,g] = tf2sos(b,a) */
      1.00000000000e+00, -5.19191279323e-02, 1.00000000000e+00, -1.58391500480e+00, 9.32046186768e-01,
      1.00000000000e+00, -1.29724486402e+00, 1.00000000000e+00, -1.65074162038e+00, 7.48880762740e-01
    };
    upfilter.setCoefficents(coeffs);
    // two filters: same coefficients, different state variables
    downfilter.setCoefficents(coeffs);
    oversampled = (float*)malloc(blocksize*4*sizeof(float));
  }
  ~Oversampler(){
    free(oversampled);
  }
  float* upsample(float* buf, int sz){
    float* p = oversampled;
    for(int i=0; i<sz; ++i){
      *p++ = buf[i];
      *p++ = 0;
      *p++ = 0;
      *p++ = 0;
    }
    upfilter.process(oversampled, sz<<2);
    return oversampled;
  }
  float* downsample(float* buf, int sz){
    downfilter.process(oversampled, sz<<2);
    float* p = oversampled;
    for(int i=0; i<sz; ++i){
      buf[i] = *p;
      p += 4;
    }
    return buf;
  }
};

class OversamplePatch : public Patch {
private:
  Oversampler os;
public:
  OversamplePatch() : os(getBlockSize()) {
  }
  void processAudio(AudioBuffer &buffer){    
    int size = buffer.getSize();
    float* buf = buffer.getSamples(0);
    float* upsampled = os.upsample(buf, size);
    for(int i = 0; i < size*4; i++) {
      upsampled[i] = oversampledFunction(upsampled[i]);
    }
    os.downsample(buf, size);
  }
  float oversampledFunction(float x){ 
    return x * ( 27 + x*x ) / ( 27 + 9*x*x ); 
  }
};

#endif // __OversamplePatch_hpp__
