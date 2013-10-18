#ifndef __DroneBox_hpp__
#define __DroneBox_hpp__

#include "StompBox.h"

/*

 DroneBox OWL
 
 Sympathetic Resonance Generator
 A bank of four tuned comb filters / virtual strings which resonate in response to the audio input
 A (very) simplified version of my forthcoming plug-in DroneBox v3
 http://www.olilarkin.co.uk
 
 Ramping code from martind's OWL patches https://github.com/dekstop
 
 Parameters:
 
 A) Coarse pitch
 B) Fine pitch
 C) Decay time
 D) Dry/Wet mix
 
 
 TODO:
 
 - optimize for OWL hardware
 - push button to change tuning presets
 
 Copyright (C) 2013  Oliver Larkin
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>
 
*/

#define CLIP(a,lo, hi) ( (a)>(lo)?( (a)<(hi)?(a):(hi) ):(lo) )

const unsigned int BUF_SIZE = 1024; // = 4096 bytes per comb
const unsigned int BUF_MASK = 1023;
const float SR = 44100;
const float MAX_FBK = 0.999999;
const float SPMS = SR / 1000.;
const float MIN_DT_SAMPLES = 2.5;
const int NUM_COMBS = 4;
const float MIN_PITCH = 36.; // MIDI notenumber
const float MAX_PITCH = 60; // MIDI notenumber
const float PITCH_RANGE = MAX_PITCH - MIN_PITCH; // semitones
const float MIN_DECAY = 200.; // milliseconds
const float MAX_DECAY = 30000.; // milliseconds
const float DECAY_RANGE = MAX_DECAY - MIN_DECAY;

const float FREQ_RATIOS[NUM_COMBS] = {1., 1.5, 2., 3.}; 
//const float FREQ_RATIOS[NUM_COMBS] = {1., 1.01, 1.02, 1.03}; 


inline float wrap(float x, const float lo = 0., const float hi = 1.)
{
  while (x >= hi) x -= hi;
  while (x < lo)  x += hi - lo;
  
  return x;
}

inline float midi2CPS(float pitch, float tune = 440.)
{
  return tune * pow(2., (pitch - 69.) / 12.);
}

class PSmooth
{
private:
  float mA, mB;
  float mOutM1;
  
public:
  PSmooth(float coeff = 0.99, float initalValue = 0.)
  : mA(coeff)
  , mB(1. - mA)
  , mOutM1(initalValue)
  {
  }
  
  inline float process(float input)
  {
    mOutM1 = (input * mB) + (mOutM1 * mA);
    return mOutM1;
  }
};

class DCBlocker
{
private:
  float mInM1, mOutM1;
  float mC;
public:
  DCBlocker()
  : mInM1(0.)
  , mOutM1(0.)
  , mC(1. - ( 126. / SR))
  {
  }
  
  inline float process(float input)
  {
    mOutM1 = input - mInM1 + mC * mOutM1;
    mInM1 = input;
    return mOutM1;
  }
};

class DBCombFilter 
{
private:
  float mBuffer[BUF_SIZE];
  int mDTSamples;
  float mFbkScalar;
  int mWriteAddr;
  
public:
  DBCombFilter()
  : mDTSamples(0)
  , mFbkScalar(0.5)
  , mWriteAddr(0)
  {
    clearBuffer();
    setFreqCPS(440.);
    setDecayTimeMs(10.);
  }
  
  void clearBuffer()
  {
    memset(mBuffer, 0, BUF_SIZE * sizeof(float));
  }
  
  void setFreqCPS(float freqCPS)
  {
    mDTSamples = CLIP(SR/freqCPS, MIN_DT_SAMPLES, BUF_SIZE);
  }
  
  // call after setting the frequency
  void setDecayTimeMs(float decayTimeMs)
  {
    float fbk = pow(10, ((-60. / ( ( abs(decayTimeMs) * SPMS ) / mDTSamples)) / 20.));
    mFbkScalar = CLIP(fbk, 0., MAX_FBK);
  }
  
  inline float process(float input)
  {
    float readAddrF = ( (float) mWriteAddr ) - mDTSamples;
    readAddrF = wrap(readAddrF, 0., (float) BUF_SIZE);
    
    // Linear interpolation
    const int intPart = (int) readAddrF;
    const float fracPart = readAddrF-intPart;
    const float a = mBuffer[intPart & BUF_MASK];
    const float b = mBuffer[(intPart+1) & BUF_MASK];
    float output = a + (b - a) * fracPart;
    
    mBuffer[mWriteAddr++] = input + (output * mFbkScalar);
    mWriteAddr &= BUF_MASK;
    
    return output;
  }
};

class DroneBoxPatch : public Patch
{
private:
  DBCombFilter mCombs[NUM_COMBS];
  PSmooth mMixSmoother;
  DCBlocker mDCBlocker;
  float mOldValues[4];
  float mRamp;
  float mPrevCoarsePitch;
  float mPrevFinePitch;
  float mPrevDecay;
  
  inline float getRampedParameterValue(PatchParameterId id) 
  {
    float val = getParameterValue(id);
    float result = val * mRamp + mOldValues[id] * (1-mRamp);
    mOldValues[id] = val;
    return result;
  }
  
public:
  DroneBoxPatch()
  : mRamp(0.1)
  , mPrevCoarsePitch(-1.)
  , mPrevFinePitch(-1.)
  , mPrevDecay(-1.)
  {
    registerParameter(PARAMETER_A, "Coarse Pitch");
    registerParameter(PARAMETER_B, "Fine Pitch");
    registerParameter(PARAMETER_C, "Decay");
    registerParameter(PARAMETER_D, "Mix");

    mOldValues[0] = 0.; 
    mOldValues[1] = 0.;
    mOldValues[2] = 0.;
    mOldValues[3] = 0.;
    
    //printf("size of patch %ld bytes\n", sizeof(*this));
  }
  
 void processAudio(AudioBuffer &buffer)
  {
    const int size = buffer.getSize();
    const float coarsePitch = getRampedParameterValue(PARAMETER_A);
    const float finePitch = getRampedParameterValue(PARAMETER_B);
    const float decay = getRampedParameterValue(PARAMETER_C);
    const float mix = getRampedParameterValue(PARAMETER_D);
    
    if (coarsePitch != mPrevCoarsePitch || finePitch != mPrevFinePitch || decay != mPrevDecay)
    {
      const float freq = midi2CPS(MIN_PITCH + floor(mPrevCoarsePitch * PITCH_RANGE) + finePitch);
      
      for (int c = 0; c < NUM_COMBS; c++) 
      {
        mCombs[c].setFreqCPS(freq * FREQ_RATIOS[c]);
        mCombs[c].setDecayTimeMs(MIN_DECAY + (decay * DECAY_RANGE));
      }
      
      mPrevCoarsePitch = coarsePitch;
      mPrevFinePitch = finePitch;
      mPrevDecay = decay;
    }
      
	  for(int ch = 0; ch<buffer.getChannels(); ++ch)
	  {   
	    float* buf = buffer.getSamples(ch);
	    
	    for(int i = 0; i < size; i++)
	    {
	      float ips = buf[i];
	      float ops = 0.;
	      const float smoothMix = mMixSmoother.process(mix);
	      
	      for (int c = 0; c < NUM_COMBS; c++) 
	      {
		ops += mCombs[c].process(ips);
	      }
	      
	      buf[i] = mDCBlocker.process( ((ops * 0.1) * smoothMix) + (ips * (1.-smoothMix)) );
	    }
	  }
    
   
  }
  
};

#endif // __DroneBox_hpp__
