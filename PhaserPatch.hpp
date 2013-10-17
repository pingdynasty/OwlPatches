#ifndef __PhaserPatch_hpp__
#define __PhaserPatch_hpp__
/*
  class: Phaser
  implemented by: Ross Bencina <rossb@kagi.com>
  date: 24/8/98
 
  Phaser is a six stage phase shifter, intended to reproduce the
  sound of a traditional analogue phaser effect.
  This implementation uses six first order all-pass filters in
  series, with delay time modulated by a sinusoidal.
 
  This implementation was created to be clear, not efficient.
  Obvious modifications include using a table lookup for the lfo,
  not updating the filter delay times every sample, and not
  tuning all of the filters to the same delay time.
 
  Thanks to:
  The nice folks on the music-dsp mailing list, including...
  Chris Towsend and Marc Lindahl
 
  ...and Scott Lehman's Phase Shifting page at harmony central:
  http://www.harmony-central.com/Effects/Articles/Phase_Shifting/
 
*/

#include "StompBox.h"

class PhaserPatch : public Patch {    
public:
  //initialise to some usefull defaults...
  PhaserPatch()  : _lfoPhase( 0.f ), depth( 1.f ), feedback( .7f ), _zm1( 0.f ){
    registerParameter(PARAMETER_A, "Rate", "Phaser speed");
    registerParameter(PARAMETER_B, "Depth", "Depth of modulation");
    registerParameter(PARAMETER_C, "Feedback", "Amount of feedback");
    Range( 440.f, 1600.f );
    Rate( .5f );
  }
    
  void Range( float fMin, float fMax ){ // Hz
    _dmin = fMin / (getSampleRate()/2.f);
    _dmax = fMax / (getSampleRate()/2.f);
  }
    
  float Rate( float rate ){ // cps
    _lfoInc = 2.f * M_PI * (rate / getSampleRate());
    return _lfoInc * 1000.f;
  }    
    
  void processAudio(AudioBuffer &buffer) {
        
    int size  = buffer.getSize();
    float y;
        
    rate      = Rate(getParameterValue(PARAMETER_A));
    depth     = getParameterValue(PARAMETER_B);
    feedback  = getParameterValue(PARAMETER_C);
        
    //calculate and update phaser sweep lfo...
    float d  = _dmin + (_dmax-_dmin) * ((sin( _lfoPhase ) + 1.f)/2.f);
        
    _lfoPhase += rate;
    if( _lfoPhase >= M_PI * 2.f )
      _lfoPhase -= M_PI * 2.f;
        
    //update filter coeffs
    for( int i=0; i<6; i++ )
      _alps[i].Delay( d );
      
      
      for (int ch = 0; ch<buffer.getChannels(); ++ch) {
          
            float* buf  = buffer.getSamples(ch);
            for (int i = 0; i < size; i++) {
              //calculate output
              y = _alps[0].Update(_alps[1].Update(_alps[2].Update(_alps[3].Update(_alps[4].Update(
                                                      _alps[5].Update( buf[i] + _zm1 * feedback ))))));
              _zm1 = y;
                
              buf[i] = buf[i] + y * depth;
                
            }
      }
  }
    
private:
    
  class AllpassDelay{
        
  public:
    AllpassDelay()
      : _a1( 0.f )
      , _zm1( 0.f )
    {}
        
    void Delay( float delay ){ //sample delay time
      _a1 = (1.f - delay) / (1.f + delay);
    }
        
    float Update( float inSamp ){
      float y = inSamp * -_a1 + _zm1;
      _zm1 = y * _a1 + inSamp;
            
      return y;
    }
  private:
    float _a1, _zm1;
  };
    
  AllpassDelay _alps[6];
    
  float _dmin, _dmax; //range
  float _lfoPhase;
  float _lfoInc;
  float depth, rate, feedback;
    
  float _zm1;
};

#endif /* __PhaserPatch_hpp__ */
