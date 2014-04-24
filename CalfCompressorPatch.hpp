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
#include "primitives.h"

namespace dsp {

  /**********************************************************************
   * GAIN REDUCTION by Thor Harald Johanssen
   **********************************************************************/
  class gain_reduction_audio_module {
  private:
    float linSlope, detected, kneeSqrt, kneeStart, linKneeStart, kneeStop;
    float compressedKneeStop, adjKneeStart, thres;
    float attack, release, threshold, ratio, knee, makeup, detection, stereo_link, bypass, mute, meter_out, meter_comp;
    float old_threshold, old_ratio, old_knee, old_makeup, old_bypass, old_mute, old_detection, old_stereo_link;
    mutable bool redraw_graph;
    uint32_t srate;
    bool is_active;
    inline float output_level(float slope) const;
    inline float output_gain(float linSlope, bool rms) const;
  public:
    gain_reduction_audio_module();
    void set_params(float att, float rel, float thr, float rat, float kn, 
		    float mak, float det, float stl, float byp, float mu);
    void update_curve();
    void process(float &left, float &right, const float *det_left = NULL, const float *det_right = NULL);
    void activate();
    void deactivate();
    int id;
    void set_sample_rate(uint32_t sr);
    float get_output_level();
    float get_comp_level();
  };

  gain_reduction_audio_module::gain_reduction_audio_module()
  {
    is_active       = false;
    srate           = 0;
    old_threshold   = 0.f;
    old_ratio       = 0.f;
    old_knee        = 0.f;
    old_makeup      = 0.f;
    old_detection   = 0.f;
    old_bypass      = 0.f;
    old_mute        = 0.f;
    linSlope        = 0.f;
    attack          = -1;
    release         = -1;
    detection       = -1;
    stereo_link     = -1;
    threshold       = -1;
    ratio           = -1;
    knee            = -1;
    makeup          = -1;
    bypass          = -1;
    mute            = -1;
    redraw_graph    = true;
  }

  void gain_reduction_audio_module::activate()
  {
    is_active = true;
    float l, r;
    l = r = 0.f;
    float byp = bypass;
    bypass = 0.0;
    process(l, r, 0, 0);
    bypass = byp;
  }

  void gain_reduction_audio_module::deactivate()
  {
    is_active = false;
  }

  void gain_reduction_audio_module::update_curve()
  {
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

  void gain_reduction_audio_module::process(float &left, float &right, const float *det_left, const float *det_right)
  {
    if(!det_left) {
      det_left = &left;
    }
    if(!det_right) {
      det_right = &right;
    }
    if(bypass < 0.5f) {
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
      // meter_out = std::max(fabs(left), fabs(right));;
      // meter_comp = gain;
      detected = rms ? sqrt(linSlope) : linSlope;
    }
  }

  float gain_reduction_audio_module::output_level(float slope) const {
    return slope * output_gain(slope, false) * makeup;
  }

  float gain_reduction_audio_module::output_gain(float linSlope, bool rms) const {
    //this calculation is also thor's work
    if(linSlope > (rms ? adjKneeStart : linKneeStart)) {
      float slope = log(linSlope);
      if(rms) slope *= 0.5f;

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
  void gain_reduction_audio_module::set_params(float att, float rel, float thr, float rat, float kn, float mak, float det, float stl, float byp, float mu)
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
    bypass          = byp;
    mute            = mu;
    // if(mute > 0.f) {
    //     meter_out  = 0.f;
    //     meter_comp = 1.f;
    // }
    
    if (fabs(threshold-old_threshold) + fabs(ratio - old_ratio) + fabs(knee - old_knee) + fabs(makeup - old_makeup) + fabs(detection - old_detection) + fabs(bypass - old_bypass) + fabs(mute - old_mute) > 0.000001f) {
      old_threshold = threshold;
      old_ratio     = ratio;
      old_knee      = knee;
      old_makeup    = makeup;
      old_detection = detection;
      old_bypass    = bypass;
      old_mute      = mute;
      redraw_graph  = true;
    }
  }

};


class CalfCompressorPatch : public Patch {
private:
  dsp::gain_reduction_audio_module compressor;
  float params[10];
public:
  CalfCompressorPatch(){
    params[0] = 0.1f; // attack
    params[1] = 100.0f; // release
    params[2] = 10.0f; // threshold
    params[3] = 2.0f; // ratio
    params[7] = 40000.0f; // knee
    params[4] = 1.0f; // makeup
    params[6] = 0.0f; // detection
    params[5] = 1.0f; // stereo_link
    params[8] = 0.0f; // bypass
    params[9] = 0.0f; // mu
    compressor.set_sample_rate(getSampleRate());
    compressor.activate();
    compressor.set_params(params[0], params[1], params[2], params[3], params[4], 
			  params[5], params[6], params[7], params[8], params[9]);
    registerParameter(PARAMETER_A, "Attack");
    registerParameter(PARAMETER_B, "Release");
    registerParameter(PARAMETER_C, "Threshold");
    registerParameter(PARAMETER_D, "Ratio");
  }

  void processAudio(AudioBuffer &buffer){

    params[0] = getParameterValue(PARAMETER_A);
    params[1] = getParameterValue(PARAMETER_B);
    params[2] = getParameterValue(PARAMETER_C);
    params[3] = getParameterValue(PARAMETER_D);

    compressor.set_params(params[0], params[1], params[2], params[3], params[4], 
			  params[5], params[6], params[7], params[8], params[9]);

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
      float Lin = lins[i];
      float Rin = rins[i];
      // in level
      // inR *= *params[param_level_in];
      // inL *= *params[param_level_in];
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
