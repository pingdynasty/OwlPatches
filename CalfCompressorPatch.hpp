/*
 
 LICENSE:
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 */

#ifndef __CalfCompressorPatch_hpp__
#define __CalfCompressorPatch_hpp__

#include "StompBox.h"

/**********************************************************************
 * Adapted from Calf Studio Gear
 * http://calf.sourceforge.net
 **********************************************************************/

#define FAKE_INFINITY (65536.0 * 65536.0)
#define IS_FAKE_INFINITY(value) (fabs(value-FAKE_INFINITY) < 1.0)
#include "calfprimitives.h"

namespace dsp {

  /**********************************************************************
   * GAIN REDUCTION by Thor Harald Johanssen
   **********************************************************************/
  class gain_reduction_audio_module {
  private:
    float linSlope, detected, kneeSqrt, kneeStart, linKneeStart, kneeStop;
    float compressedKneeStop, adjKneeStart, thres;
    float attack, release, threshold, ratio, knee, makeup, detection, stereo_link, meter_out, meter_comp;
    uint32_t srate;
    inline float output_level(float slope) const;
    inline float output_gain(float linSlope, bool rms) const;
  public:
    gain_reduction_audio_module();
    void set_params(float att, float rel, float thr, float rat, 
		    float kn,  float mak, float det, float stl);
    void update_curve();
    void process(float &left, float &right, const float *det_left = NULL, const float *det_right = NULL);
    void activate();
    void set_sample_rate(uint32_t sr);
  };

  gain_reduction_audio_module::gain_reduction_audio_module(){
    srate           = 0;
    linSlope        = 0.f;
    attack          = -1;
    release         = -1;
    detection       = -1;
    stereo_link     = -1;
    threshold       = -1;
    ratio           = -1;
    knee            = -1;
    makeup          = -1;
  }

  void gain_reduction_audio_module::activate(){
    float l, r;
    l = r = 0.f;
    process(l, r, 0, 0);
  }

  void gain_reduction_audio_module::update_curve(){
    float linThreshold = threshold;
    float linKneeSqrt = sqrt(knee);
    linKneeStart = linThreshold / linKneeSqrt;
    adjKneeStart = linKneeStart*linKneeStart;
    float linKneeStop = linThreshold * linKneeSqrt;
    thres = log(linThreshold);
    kneeStart = log(linKneeStart);
    kneeStop = log(linKneeStop);
    compressedKneeStop = (kneeStop - thres) / ratio + thres;
  }

  void gain_reduction_audio_module::process(float &left, float &right, const float *det_left, const float *det_right){
    if(!det_left) {
      det_left = &left;
    }
    if(!det_right) {
      det_right = &right;
    }
    // this routine is mainly copied from thor's compressor module
    // greatest sounding compressor I've heard!
    bool rms = (detection == 0);
    bool average = (stereo_link == 0);
    float attack_coeff = min(1.f, 1.f / (attack * srate / 4000.f));
    float release_coeff = min(1.f, 1.f / (release * srate / 4000.f));
    float absample = average ? (fabs(*det_left) + fabs(*det_right)) * 0.5f : max(fabs(*det_left), fabs(*det_right));
    if(rms) absample *= absample;
    dsp::sanitize(linSlope);
    linSlope += (absample - linSlope) * (absample > linSlope ? attack_coeff : release_coeff);
    float gain = 1.f;
    if(linSlope > 0.f) {
      gain = output_gain(linSlope, rms);
    }
    left *= gain * makeup;
    right *= gain * makeup;
    detected = rms ? sqrt(linSlope) : linSlope;
  }

  float gain_reduction_audio_module::output_level(float slope) const {
    return slope * output_gain(slope, false) * makeup;
  }

  float gain_reduction_audio_module::output_gain(float linSlope, bool rms) const {
    //this calculation is also thor's work
    if(linSlope > (rms ? adjKneeStart : linKneeStart)) {
      float slope = log(linSlope);
      if(rms) 
	slope *= 0.5f;
      float gain = 0.f;
      float delta = 0.f;
      if(IS_FAKE_INFINITY(ratio)) {
	gain = thres;
	delta = 0.f;
      } else {
	gain = (slope - thres) / ratio + thres;
	delta = 1.f / ratio;
      }
      if(knee > 1.f && slope < kneeStop) {
	gain = hermite_interpolation(slope, kneeStart, kneeStop, kneeStart, compressedKneeStop, 1.f, delta);
      }
      return exp(gain - slope);
    }
    return 1.f;
  }

  void gain_reduction_audio_module::set_sample_rate(uint32_t sr) {
    srate = sr;
  }

  void gain_reduction_audio_module::set_params(float att, float rel, float thr, float rat, float kn, float mak, float det, float stl)
  {
    // set all params
    attack          = att;
    release         = rel;
    threshold       = thr;
    ratio           = rat;
    knee            = kn;
    makeup          = mak;
    detection       = det;
    stereo_link     = stl;
  }

};


class CalfCompressorPatch : public Patch {
private:
  dsp::gain_reduction_audio_module compressor;
  float params[8];
public:
  CalfCompressorPatch(){
    /**
       parameter     default         min          max   step
       attack        20,             0.01,        2000, 0
       release       250,            0.01,        2000, 0
       threshold     0.125,          0.000976563, 1,    0
       ratio         2,              1,           20,   21
       makeup        2,              1,           64,   0
       knee          2.828427125,    1,           8,    0
       detection     0,              0,           1,    0 (RMS or Peak)
       stereo link   0,              0,           1,    0 (Average or Maximum)
    */
    params[0] = 20.f; // attack
    params[1] = 250.f; // release
    params[2] = 0.125f; // threshold
    params[3] = 2.f; // ratio
    params[4] = 2.828427125f; // knee
    params[5] = 2.f; // makeup
    params[6] = 0.f; // detection
    params[7] = 0.f; // stereo_link
    compressor.set_sample_rate(getSampleRate());
    compressor.activate();
    compressor.set_params(params[0], params[1], params[2], params[3], 
			  params[4], params[5], params[6], params[7]);
    registerParameter(PARAMETER_A, "Attack");
    registerParameter(PARAMETER_B, "Release");
    registerParameter(PARAMETER_C, "Threshold");
    registerParameter(PARAMETER_D, "Ratio");
  }

  void processAudio(AudioBuffer &buffer){
    params[0] = getParameterValue(PARAMETER_A) * (2000-0.01) + 0.01; // attack
    params[1] = getParameterValue(PARAMETER_B) * (2000-0.01) + 0.01; // release
    params[2] = getParameterValue(PARAMETER_C) * (1-0.000976563) + 0.000976563; // threshold
    params[3] = getParameterValue(PARAMETER_D) * (20-1) + 1; // ratio
    
    compressor.set_params(params[0], params[1], params[2], params[3], 
			  params[4], params[5], params[6], params[7]);
    compressor.update_curve();

    int size = buffer.getSize();
    float* lins = buffer.getSamples(0);
    float* rins = buffer.getChannels() > 1 ? buffer.getSamples(1) : buffer.getSamples(0);
    for(int i=0; i<size; ++i){
      // cycle through samples
      float outL = 0.f;
      float outR = 0.f;
      float inL = lins[i];
      float inR = rins[i];
      float leftAC = inL;
      float rightAC = inR;
      compressor.process(leftAC, rightAC);
      outL = leftAC;
      outR = rightAC;
      // mix
      // outL = outL * *params[param_mix] + Lin * (*params[param_mix] * -1 + 1);
      // outR = outR * *params[param_mix] + Rin * (*params[param_mix] * -1 + 1);
      // send to output
      lins[i] = outL;
      rins[1] = outR;
    } // cycle through samples
  }
};

#endif // __CalfCompressorPatch_hpp__
