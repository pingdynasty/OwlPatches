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


#ifndef __VidhaPatch_hpp__
#define __VidhaPatch_hpp__

#include "StompBox.h"

////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 
 Port of Vidha plugin by ArtyFX - http://openavproductions.com/artyfx/#vidha
 
 */

////////////////////////////////////////////////////////////////////////////////////////////////////


/** Widener
 *  A stereo width effect, which implements a mid-side width control, and
 *  right channel invert option.
 **/
class Widener // : Effect
{
public:
    Widener(int sr) :
    samplerate( sr ),
    
    /// dezipper init
    w(10.0f / (sr * 0.02)),
    a(0.70f),
    b(1.0f / (1.0f - a)),
    g1(0.0f),
    g2(0.0f)
    {
        
        width = 0.5;
        invertRight = false;
    }
    
    float getValue()
    {
        return width;
    }
    
    void setValue(float v)
    {
        if ( v < 0.f ) v = 0.f;
        if ( v > 1.f ) v = 1.f;
        
        // width parameters ranges from 0 (mono), to 2 (very-wide)
        width = v*3;
    }
    
    void setInvert( bool i )
    {
        invertRight = i;
    }
    float getInvert()
    {
        return invertRight;
    }
    
    int getNumInputs() { return 2; }
    int getNumOutputs(){ return 2; }
    
    void process (long count, float* inL, float* inR, float* outL, float* outR)
    {

        /// smoothing algo is a lowpass, to de-zip movement
        /// x^^4 approximates linear volume increase for human ears
        g1 += w * (width - g1 - a * g2 - 1e-20f);
        g2 += w * (b * g1 - g2 + 1e-20f);
        float widthDeZip = g2;
        
        
        // do mid-side width calculations
        float tmp = 1 / max( 1 + widthDeZip , 2.f );
        float mid = 1 * tmp;
        
        float side = widthDeZip * tmp;
        
        
        for (int i = 0; i < count; i++)
        {
            // apply mid / side coefficents
            float m = ( *inL + *inR ) * mid;
            float s = ( *inL - *inR ) * side;
            
            // write output
            *outL = m - s;
            
            *outR = m + s;
            
            if ( invertRight )
            {
                *outR = *outR * -1;
            }
            
            // move buffer pointers to next sample
            inL++;
            inR++;
            outL++;
            outR++;
            }

    }
    
private:
    int samplerate;
    
    float width;
    bool  invertRight;
    
    /// dezipper filter state
    float w, a, b, g1, g2;
};

////////////////////////////////////////////////////////////////////////////////////////////////////


class VidhaPatch : public Patch {
public:
    
  float sampleRate;
  Widener *wideMS;
  bool invert;
    
  VidhaPatch(){
      
    sampleRate = 48000;
    wideMS = new Widener(sampleRate);
    invert = false;
      
    registerParameter(PARAMETER_A, "Width");
    registerParameter(PARAMETER_B, "Invert");

  }
    
  void processAudio(AudioBuffer &buffer){
      
      if (PARAMETER_B < 0.5f) {invert = false;} else {invert = true;}
   
      float size = buffer.getSize();
      float* audioL = buffer.getSamples(0);
      float* audioR = buffer.getSamples(1);
      wideMS->setValue(getParameterValue(PARAMETER_A));
      wideMS->setInvert(getParameterValue(PARAMETER_B));
      wideMS->process(size, audioL, audioR, audioL, audioR);
      
      
  }
};

#endif // __Vidha_Patch__
