#ifndef __ConnyPatch_hpp__
#define __ConnyPatch_hpp__

#include "StompBox.h"

/* Conny Patch
 
 Made by me, Johan Larsby
 
 Conny was a dog I used to know. He was some sort of mongrel breed that no-one could really pin out what it was. Mostly we think he was a poodle/bulldog mix. But there had to be more in there.
 Anyways I wanted to make a distortion that reminds of him, and what better then to mix two different distortions into something new?
 
 The first dist is a variant of a classic waveshaper. I'm sure a gazillion variants can be found around the internet. I borrowed inspiration from www.musicdsp.org
 
 The second dist is really an adaption of a one-line music program that you can read more about here (http://www.larsby.com/johan/?p=1036) but instead of incrementing "t" I use the input sound after some int'ing. There will also be some bit-crunch since.
 
 The A param controls the amount of the first dist.
 The B param controls the amount of the second dist.
 The C param adds gain to dist2 since I thought it was not LOUD enough
 The D param is a wet/dry param.
 
 */

class ConnyPatch : public Patch {
public:
  ConnyPatch(){
      registerParameter(PARAMETER_A, "Dist 1");
      registerParameter(PARAMETER_B, "Dist 2");
      registerParameter(PARAMETER_C, "Gain");
      registerParameter(PARAMETER_D, "Dry/Wet");
  }

    /* The clip function works to sanitize the data if it's to big (could happen) it's cut down to size*/
    float clip(float in)
    {
        if (in > 1.0)
            return 1.0;
        else if (in < -1.0)
            return 1.0;
        return in;
    }
    
    void processAudio(AudioBuffer &buffer)
    {
        int size = buffer.getSize();
        
        //Dist 1's parameter, needed to sanitize by *0.9 or the filter would be quiet. also the waveshaper wants a specific range.
        float paramA = getParameterValue(PARAMETER_A)*1.0;
        //---float dist1_amount = 2.0*paramA/(1.0-paramA);
        float dist1_amount = paramA;
        
        //Dist 2's parameter, since it's a bit shift distortion we needed to move the value to an int
        float paramB = getParameterValue(PARAMETER_B);
        int paramB_int = int(paramB*5.0);
        
        // Gain, you can make the sound 100% louder then it was, but never quiter.
        float paramC = (getParameterValue(PARAMETER_C)*2.0)+1.0;
        
        //Dry/wet.
        float paramD = getParameterValue(PARAMETER_D);
       
    for(int ch = 0; ch<buffer.getChannels(); ++ch)
    { 
        //Getting the number of samples in the inputstream
        float* buf = buffer.getSamples(ch);
        
        //main loop, let's have some fun

        for(int i=0; i<size; ++i)
        {
            //unless we have a silent stream, let's do some processing.
            if(buf[i]!=0.0)
            {
                // "classic" Waveshaper
                float dist_1 =  (1.0+paramA) * ((buf[i] - paramA) * (((pow (buf[i],buf[i])))*paramA)*abs(buf[i]));
                
                // "modern" waveshaper
                int t= (buf[i]+1.0)*10000;
                float dist_2 = (t&(-t>>paramB_int)) | ((t<<paramB_int)&(t>>(paramB_int*2))) ;
                //moving back to float
                dist_2 =  (dist_2/10000.0)-1.0;
                
                // this line add's togheter the dist's and the dry signal depending on param d. it also add's gain to dist_2
                buf[i] = clip(paramD*(dist_1+(dist_2*paramC)) + ((1.0-paramD)*buf[i] ));
            }
        }
    }
        
       
    }
    
};

#endif // __ConnyPatch_hpp__
