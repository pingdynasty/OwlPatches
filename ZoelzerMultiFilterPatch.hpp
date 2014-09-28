#ifndef __ZoelzerMultiFilterPatch_hpp__
#define __ZoelzerMultiFilterPatch_hpp__

#include "StompBox.h"

#define ZOELZER_LOWPASS_FILTER_MODE   0
#define ZOELZER_HIGHPASS_FILTER_MODE  1
#define ZOELZER_BANDPASS_FILTER_MODE  2
#define ZOELZER_NOTCH_FILTER_MODE     3
#define ZOELZER_PEAK_FILTER_MODE      4
#define ZOELZER_LOWSHELF_FILTER_MODE  5
#define ZOELZER_HIGHSHELF_FILTER_MODE 6
#define ZOELZER_MODES                 7
/**
http://www.musicdsp.org/showArchiveComment.php?ArchiveID=37
http://www.earlevel.com/main/2013/10/13/biquad-calculator-v2/

Type : biquad IIR
References : Udo Zoelzer: Digital Audio Signal Processing (John Wiley & Sons, ISBN 0 471 97226 6), Chris Townsend, Nigel Redmon
*/

#include "BiquadFilter.hpp"

class ZoelzerMultiFilterPatch : public Patch {
private:
  BiquadFilter filter;
  float coeffs[5];
public:
  ZoelzerMultiFilterPatch(){
    registerParameter(PARAMETER_A, "Mode");
    registerParameter(PARAMETER_B, "Frequency");
    registerParameter(PARAMETER_C, "Resonance");
    registerParameter(PARAMETER_D, "Gain");
  }
  void processAudio(AudioBuffer &buffer){
    int mode = getParameterValue(PARAMETER_A)*ZOELZER_MODES;
    float omega = (M_PI/2 - 0.01)*getParameterValue(PARAMETER_B) + 0.00001; // Frequency
    float K = tan(omega);
    float Q = getParameterValue(PARAMETER_C) * 10 + 0.1; // Resonance
    float gain = getParameterValue(PARAMETER_D);
    float V = abs(gain-0.5)*60 + 1; // Gain
    float norm;
    /* coeffs[b0, b1, b2, a1, a2] */
    switch(mode){
    case ZOELZER_LOWPASS_FILTER_MODE:
      norm = 1 / (1 + K / Q + K * K);
      coeffs[0] = K * K * norm;
      coeffs[1] = 2 * coeffs[0];
      coeffs[2] = coeffs[0];
      coeffs[3] = 2 * (K * K - 1) * norm;
      coeffs[4] = (1 - K / Q + K * K) * norm;
      break;
    case ZOELZER_HIGHPASS_FILTER_MODE:
      norm = 1 / (1 + K / Q + K * K);
      coeffs[0] = 1 * norm;
      coeffs[1] = -2 * coeffs[0];
      coeffs[2] = coeffs[0];
      coeffs[3] = 2 * (K * K - 1) * norm;
      coeffs[4] = (1 - K / Q + K * K) * norm;
      break;      
    case ZOELZER_BANDPASS_FILTER_MODE:
      norm = 1 / (1 + K / Q + K * K);
      coeffs[0] = K / Q * norm;
      coeffs[1] = 0;
      coeffs[2] = -coeffs[0];
      coeffs[3] = 2 * (K * K - 1) * norm;
      coeffs[4] = (1 - K / Q + K * K) * norm;
      break;
    case ZOELZER_NOTCH_FILTER_MODE:
      norm = 1 / (1 + K / Q + K * K);
      coeffs[0] = (1 + K * K) * norm;
      coeffs[1] = 2 * (K * K - 1) * norm;
      coeffs[2] = coeffs[0];
      coeffs[3] = coeffs[1];
      coeffs[4] = (1 - K / Q + K * K) * norm;
      break;
    case ZOELZER_PEAK_FILTER_MODE:
      if (gain >= 0.5) {
	norm = 1 / (1 + 1/Q * K + K * K);
	coeffs[0] = (1 + V/Q * K + K * K) * norm;
	coeffs[1] = 2 * (K * K - 1) * norm;
	coeffs[2] = (1 - V/Q * K + K * K) * norm;
	coeffs[3] = coeffs[1];
	coeffs[4] = (1 - 1/Q * K + K * K) * norm;
      }
      else {
	norm = 1 / (1 + V/Q * K + K * K);
	coeffs[0] = (1 + 1/Q * K + K * K) * norm;
	coeffs[1] = 2 * (K * K - 1) * norm;
	coeffs[2] = (1 - 1/Q * K + K * K) * norm;
	coeffs[3] = coeffs[1];
	coeffs[4] = (1 - V/Q * K + K * K) * norm;
      }
      break;
    case ZOELZER_LOWSHELF_FILTER_MODE:
      if (gain >= 0.5) {
	norm = 1 / (1 + M_SQRT2 * K + K * K);
	coeffs[0] = (1 + sqrt(2*V) * K + V * K * K) * norm;
	coeffs[1] = 2 * (V * K * K - 1) * norm;
	coeffs[2] = (1 - sqrt(2*V) * K + V * K * K) * norm;
	coeffs[3] = 2 * (K * K - 1) * norm;
	coeffs[4] = (1 - M_SQRT2 * K + K * K) * norm;
      } else {
	norm = 1 / (1 + sqrt(2*V) * K + V * K * K);
	coeffs[0] = (1 + M_SQRT2 * K + K * K) * norm;
	coeffs[1] = 2 * (K * K - 1) * norm;
	coeffs[2] = (1 - M_SQRT2 * K + K * K) * norm;
	coeffs[3] = 2 * (V * K * K - 1) * norm;
	coeffs[4] = (1 - sqrt(2*V) * K + V * K * K) * norm;
      }
      break;
    case ZOELZER_HIGHSHELF_FILTER_MODE:
      if (gain >= 0.5) {
	norm = 1 / (1 + M_SQRT2 * K + K * K);
	coeffs[0] = (V + sqrt(2*V) * K + K * K) * norm;
	coeffs[1] = 2 * (K * K - V) * norm;
	coeffs[2] = (V - sqrt(2*V) * K + K * K) * norm;
	coeffs[3] = 2 * (K * K - 1) * norm;
	coeffs[4] = (1 - M_SQRT2 * K + K * K) * norm;
      } else {
	norm = 1 / (V + sqrt(2*V) * K + K * K);
	coeffs[0] = (1 + M_SQRT2 * K + K * K) * norm;
	coeffs[1] = 2 * (K * K - 1) * norm;
	coeffs[2] = (1 - M_SQRT2 * K + K * K) * norm;
	coeffs[3] = 2 * (K * K - V) * norm;
	coeffs[4] = (V - sqrt(2*V) * K + K * K) * norm;
      }
      break;
    }
    filter.setCoefficents(coeffs);
    int size = buffer.getSize();
    float* samples = buffer.getSamples(0);
    filter.process(samples, size);
//     for(int i=0; i<size; ++i)
//       samples[i] = filter.compute(samples[i]);
  }
};

#endif // __ZoelzerMultiFilterPatch_hpp__
