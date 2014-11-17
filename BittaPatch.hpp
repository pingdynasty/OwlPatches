 ////////////////////////////////////////////////////////////////////////////////////////////////////

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


/* created by the OWL team 2014 */


////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __BittaPatch_hpp__
#define __BittaPatch_hpp__

#include "StompBox.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

/* 
 
 Port of Bitta plugin by ArtyFX - http://openavproductions.com/artyfx/#bitta
 
 */

////////////////////////////////////////////////////////////////////////////////////////////////////


/** BitCrusher
 *  Bit-crushing and sample rate reduction.
 **/

class BitCrusher // : Effect
{
public:
    BitCrusher(int sr) :
    samplerate( sr )
    {
        
        bits = 16;
        rate = 1;
        
        m = 1 << (bits-1);
        count = 0;
        
        // only wet to start
        dryWet = 0.;
    }
    
    float getValue()
    {
        return bits;
    }
    
    void setValue(float v)
    {
        if ( v < 0.f ) v = 0.f;
        if ( v > 1.f ) v = 1.f;
        
        v = (v * 0.4) + 0.4; //scaling 'crush' value
        
        m = 1 + (1 << ((int) (((1-v)*15))) );
        
        //m = 1 << ( int(v*16.f) -1 );
    }

    void setDryWet( float dw )
    {
        dryWet = dw;
    }
    
    void process (float count, float* in, float* out)
    {

        for(int i = 0; i < count; i++)
        {
            float tmp = 0.f;
            
            count+=rate;
            
            if ( count >= 1)
            {
                count -= 1;
                
                tmp=(long int)( *in * m)/(float)m;
            }
            *out++ = (*in * (1-dryWet)) + (tmp * dryWet);
            in++;
        }

    }
    
private:
    int samplerate;
    
    int bits;
    float rate;
    
    long int m;
    float count;
    
    float dryWet;
};

////////////////////////////////////////////////////////////////////////////////////////////////////

class BittaPatch : public Patch {
    
public:
    
  float sampleRate;
  BitCrusher *bCrusher;
    
  BittaPatch(){
      
    sampleRate = 48000;
    bCrusher = new BitCrusher(sampleRate);
      
    registerParameter(PARAMETER_A, "Crush");
    registerParameter(PARAMETER_B, "Dry / Wet");

  }

  void processAudio(AudioBuffer &buffer){
      
      float size = buffer.getSize();
      float* audioBlock = buffer.getSamples(0);
      bCrusher->setValue(getParameterValue(PARAMETER_A));
      bCrusher->setDryWet(getParameterValue(PARAMETER_B));
      bCrusher->process(size, audioBlock, audioBlock);
      
  }
    
};

#endif // __BittaPatch_hpp__
