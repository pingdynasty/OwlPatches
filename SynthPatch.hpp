#pragma once

#include "StompBox.h"
#ifndef TWOPI 
#define TWOPI 6.2831853f
#endif
#ifndef SQRT2
#define SQRT2 1.414213562373095
#endif
namespace Synth {




  
  
  class BaseOscillator {
  public:
    
    float frequency;
    
    
    
    BaseOscillator() {
      setSampleRate(44100);
      frequency = 440;
    }
    
    void setSampleRate(float sampleRate) {
      TWO_PI_BY_SAMPLERATE = TWOPI / sampleRate;
    }
    
    
    
    // impelement this to make your own oscillator.
    virtual float getSampleForPhase() { return sin(phase); }
    
    
    
    float getSample(float freq) {
      this->frequency = freq;
      return getSample();
    }
    
    float getSample() {
      // wrap and increment phase
      phase += frequency*TWO_PI_BY_SAMPLERATE;
      if(phase>=TWOPI) phase -= TWOPI;
      return getSampleForPhase();
    }
    
    
  
    float TWO_PI_BY_SAMPLERATE; // constant for calculating phase increment
    float phase; // current phase of oscillator
    
  };
  
  
  // uber simple, not bandlimited
  class TriOscillator: public BaseOscillator {
  public:
    
    float getSampleForPhase() {
      if(phase<M_PI) return (-1.f + (2.f*phase/M_PI))*2.f;
      else return 2.f*(1.f - (2.f*(phase-M_PI)/M_PI));
    }
  };
  
  class SawOscillator: public BaseOscillator {
  public:
    
    float getSampleForPhase() {
      return (1.f - (phase/M_PI))*0.5f;
    }
  };
  
  
  class ReverseSawOscillator: public BaseOscillator {
  public:
    
    float getSampleForPhase() {
      return ((phase/M_PI)-1.f)*0.5f;
    }
  };
  
  
  class SquareOscillator: public BaseOscillator {
  public:
    
    float getSampleForPhase() {
      if(phase<M_PI) return 1.f;
      else return -1.f;
    }
  };
  
  
  class SinOscillator: public BaseOscillator {
  public:

    float getSampleForPhase() {

      return sin(phase);
    }
};

    class SquareSawOscillator: public BaseOscillator {
    private:
      float amt;
    public:
      float a, b;
      void setAmt(float amt) {
        this->amt = amt;
        a = 0.5 * (1-amt);
        b = amt;
      }
      float getSampleForPhase() {
        return (1.f - (phase/M_PI))*a + (phase<M_PI?1:-1)*b;
      }

    };
  








	class LPF {
	public:


    // by rbj
    static float fastSqrt(register float x)
    {
      
      if (x > 5.877471754e-39)
      {
        register float accumulator, xPower;
        register long intPart;
        register union {float f; long i;} xBits;
        
        xBits.f = x;
        
        intPart = ((xBits.i)>>23);   /* get biased exponent */
        intPart -= 127;  /* unbias it */
        
        x = (float)(xBits.i & 0x007FFFFF);   /* mask off exponent leaving 0x800000*(mantissa - 1) */
        x *= 1.192092895507812e-07;  /* divide by 0x800000 */
        
        accumulator =  1.0 + 0.49959804148061*x;
        xPower = x*x;
        accumulator += -0.12047308243453*xPower;
        xPower *= x;
        accumulator += 0.04585425015501*xPower;
        xPower *= x;
        accumulator += -0.01076564682800*xPower;
        
        if (intPart & 0x00000001)
        {
          accumulator *= SQRT2;  /* an odd input exponent means an extra sqrt(2) in the output */
        }
        
        xBits.i = intPart >> 1;  /* divide exponent by 2, lose LSB */
        xBits.i += 127;  /* rebias exponent */
        xBits.i <<= 23;  /* move biased exponent into exponent bits */
        
        return accumulator * xBits.f;
      }
      else
      {
        return 0.0;
      }
      
    }



	  float z;
	  float x;
	  float y;
	  float r;
	  float c;
	  LPF() {
	    z = x = y = r = c = 0;
	  }

	#define TWOPI_BY_SAMPLERATE 0.00014247585731f
	  void setCoeffs(float cutoff, float resonance) {
	    if(cutoff>11025) cutoff = 11025;
	      z=cos(TWOPI_BY_SAMPLERATE*cutoff);
	      c = 2 - 2*z;
	      float zzz = z-1;
	      zzz = zzz*zzz*zzz;
	      r = (SQRT2*fastSqrt(-zzz)+resonance*(z-1))/(resonance*(z-1));
	  }
	  // cutoff in hz/2 (min 10Hz/2), resonance 1 to 10
	  float process(float input) {
	      

	      x += (input - y)*c;
	      y += x;
	      x *= r;
	      return y; 
	  }
	};
};


#define ABS(X) (X>0?X:-X)
class SynthPatch : public Patch {
public:
  Synth::SquareSawOscillator osc;
 Synth::SquareSawOscillator osc2; 
 Synth::SquareSawOscillator osc3; 
  Synth::LPF inLpf, inLpf2, out1Lpf;
  bool lastRect;

int pos;
  float phase;
  float env;


float detune;

float filtEnv;
  SynthPatch(){
    filtEnv = 0;
    detune = 0;
    phase = 0;
    lastRect = 0;
    pos = 0;
    env = 0;

    osc.setAmt(0);
    osc2.setAmt(0);
    osc3.setAmt(0);
    inLpf.setCoeffs(100, 2);
    inLpf2.setCoeffs(100, 2);
    out1Lpf.setCoeffs(200, 0.5);
    registerParameter(PARAMETER_A, "Octave");
    registerParameter(PARAMETER_B, "Filter");
    registerParameter(PARAMETER_C, "Shape");
    registerParameter(PARAMETER_D, "Attack");

  }
float oct;
void reportPos(float pp) {
 
  if(env<0.3) return;
   // this is how many samples in a period
  float freq = 44100/pp;

  // reject any pitch changes if the frequency is out of range or
  // the volume is too low.
  if(freq<20) return;
  if(freq>1318) return;
 
  osc.frequency = freq;
  osc2.frequency = osc.frequency * (0.5+(0.5-oct)*0.02);
  osc3.frequency = osc.frequency * 0.5;

}
float trackVol(float in) {
  in *= 5;

  if(in<0) in *= -1;
  if(in>1) in = 1;
  if(in<0.1) in = 0;
  //in = Synth::LPF::fastSqrt(in);
  //return in;
  if(in>env) {
    env = in;
  } else {
    if(in>0.05) {
      env *= 0.9999;
    } else {
      env *= 0.9995;
    }
  }
  //env = env * 0.999 + in * 0.001;
  return env;
}
float filtEnvCoeff;
#define MAX(A,B) (A>B?A:B)

#define MIN(A,B) (A>B?B:A)

  float filt(float in) {
    if(in>filtEnv) {
      filtEnv+= 0.0005;// + filtEnvCoeff;
      if(filtEnv>10) filtEnv = 10;
    } else {
      filtEnv = in * 0.0005 + filtEnv * 0.9995;
    }
    return filtEnv*filtEnvCoeff;
  }



  void processAudio(AudioBuffer& buffer){
    
    out1Lpf.setCoeffs(2000, 3);
    float amt = getParameterValue(PARAMETER_C);

    osc.setAmt(amt);
    osc2.setAmt(amt);
    osc3.setAmt(amt);

    int size = buffer.getSize();
    float* x = buffer.getSamples(0);
    
    oct = getParameterValue(PARAMETER_A)*0.5;
    
    filtEnvCoeff = getParameterValue(PARAMETER_D);
    float filter = getParameterValue(PARAMETER_B);
    float oneMinusOct = 1-oct;
    
    float volMult = (1+filter)*2;
    float filtExp = filter*5;

    for(int i=0; i<size; ++i) {
      float in = x[i];		
      
      // pitch track
      float inFilt = inLpf.process(in);
     // inFilt = inLpf2.process(inFilt);
      bool rect = inFilt>0;		

      // the flip flops
      if(lastRect && !rect) {
        reportPos(pos);
        pos = 0;
      } else {
        pos++;
      }

      lastRect = rect;    





      float vol = trackVol(in);
      
      float cutoff = 100.f + (filter*8 + filt(vol*8)) * 1000.f;
      
      out1Lpf.setCoeffs(cutoff, 3);
      
      float out = osc.getSample() * oneMinusOct;
      out += (osc2.getSample() + osc3.getSample())*oct;

      x[i] = out1Lpf.process(out*0.8*vol);
      
    }
    
  }
};

