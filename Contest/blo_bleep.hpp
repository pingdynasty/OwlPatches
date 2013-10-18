/*
 * little_blo_bleep
 *
 * use bit pattern of an integer parameter as gain to create glitchy fx
 *
 * motivation >> every integer has a unique bit pattern >> unique glitchy gain pattern!
 *
 */


#include "StompBox.h"

class little_blo_bleep : public Patch {

  int current_sample; // currently processed sample
  float blo_bleep; // our bloopity bleepity gain :)

  int num_bits; // number of bits in int >> should be 32, but we will calculate it to be safe.
  int num_bits_less1;
  int num_shifts; // number of bits the pattern has been shifted so far
  int pattern;
  int bpm;
  int maxBpm;
    
  static const int extract = 1; // to extract the right most bit ie our glitchy gain

  public:

    little_blo_bleep () {
      current_sample = 0;
      num_bits = sizeof (int) * 8;
      num_bits_less1 = num_bits - 1;
      num_shifts = 0;
        maxBpm = 10000;
        
      registerParameter(PARAMETER_A, "Pattern");
      registerParameter(PARAMETER_B, "BPM");
    }
    
    

    void processAudio(AudioBuffer &buffer)
  {

      int size   = buffer.getSize ();
      
      

      // 
      // do we have to get these every cycle? 
      // have to cos the user could change during performance...
      //
      pattern = getParameterValue (PARAMETER_A) * maxBpm ; // pick the current integer as our unique bit pattern
      bpm     = getParameterValue (PARAMETER_B) * maxBpm ; // our bpm. whats the max of PARAMETER_B? that will be max bpm too.
      float bps = bpm / 60.0; // beats per second
      int samples_per_beat = (int) (getSampleRate() / bps + 0.5); // we bit shift the pattern every beat

      

      // apply blo_bleep to input buffer!

    for(int ch = 0; ch<buffer.getChannels(); ++ch)
    {
      float* buf = buffer.getSamples(ch);
      
      for (int i = 0; i < size; ++i) 
        {
          
          if (++current_sample >= samples_per_beat) 
	    {
              current_sample = 0;
              blo_bleep = (pattern >> num_shifts) & extract; // extract the right most bit >> bloopity, bleepity!
              if (++num_shifts >= num_bits_less1) num_shifts = 1;
            } 
           buf[i] = blo_bleep * buf[i]; // 1 or 0 gain based on extracted bit pattern, bloppity bleepity!
        }
       
    }

  }

};
