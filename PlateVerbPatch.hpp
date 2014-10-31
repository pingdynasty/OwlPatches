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
 
 
 Modified from the Freeverb patch for OWL, by Dave Moffat, the OWL team 2014
 */


////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once
#include "ReverbHeader.h"
#include "StompBox.h"
#define undenormalise(sample) (sample)

const int	plate_numcombs		= 6;
const int	plate_numallpasses1	= 4;
const int	plate_numallpasses2	= 2;

const int plate_combtuningL1	 = 266;
const int plate_combtuningL2	 = 2974;
const int plate_combtuningL3	 = 1913;
const int plate_combtuningL4	 = 1996;
const int plate_combtuningL5	 = 1990;
const int plate_combtuningL6	 = 187;

const int plate_allpasstuningL1	= 142;
const int plate_allpasstuningL2	= 107;
const int plate_allpasstuningL3	= 379;
const int plate_allpasstuningL4	= 277;
const int plate_allpasstuningL5	= 1800;
const int plate_allpasstuningL6	= 2656;

class PlateVerbPatch : public Patch {
public:
    PlateVerbPatch(){
        gain = fixedgain;
        registerParameter(PARAMETER_A, "Mix");
        registerParameter(PARAMETER_B, "Room Size");
        registerParameter(PARAMETER_C, "Damp");
        registerParameter(PARAMETER_D, "");
        
        // Tie the components to their buffers
        combL[0].setbuffer(bufcombL1,plate_combtuningL1);
        combL[1].setbuffer(bufcombL2,plate_combtuningL2);
        combL[2].setbuffer(bufcombL3,plate_combtuningL3);
        combL[3].setbuffer(bufcombL4,plate_combtuningL4);
        combL[4].setbuffer(bufcombL5,plate_combtuningL5);
        combL[5].setbuffer(bufcombL6,plate_combtuningL6);
        allpassL[0].setbuffer(bufallpassL1,plate_allpasstuningL1);
        allpassL[1].setbuffer(bufallpassL2,plate_allpasstuningL2);
        allpassL[2].setbuffer(bufallpassL3,plate_allpasstuningL3);
        allpassL[3].setbuffer(bufallpassL4,plate_allpasstuningL4);
        allpassL[4].setbuffer(bufallpassL5,plate_allpasstuningL5);
        allpassL[5].setbuffer(bufallpassL6,plate_allpasstuningL6);
        
        // Set default values
        allpassL[0].setfeedback(0.5f);
        allpassL[1].setfeedback(0.5f);
        allpassL[2].setfeedback(0.5f);
        allpassL[3].setfeedback(0.5f);
        allpassL[4].setfeedback(0.5f);
        allpassL[5].setfeedback(0.5f);
        
        setwet(initialwet);
        setroomsize(initialroom);
        setdry(initialdry);
        setdamp(initialdamp);
        
        // Buffer will be full of rubbish - so we MUST mute them
        mute();
    }
    
    void mute()
    {
        for (int i=0;i<plate_numcombs;i++)
            combL[i].mute();
        for (int i=0;i<plate_numallpasses1+plate_numallpasses2;i++)
            allpassL[i].mute();
    }
    
    
    
    void setroomsize(float value)
    {
        roomsize = (value*scaleroom) + offsetroom;
        for(int i=0; i<plate_numcombs; i++)
            combL[i].setfeedback(roomsize);
    }
    
    void setdamp(float value)
    {
        damp = value*scaledamp;
        for(int i=0; i<plate_numcombs; i++)
            combL[i].setdamp(damp);
    }
    
    void setwet(float value)
    {
        wet = value*scalewet;
    }

    
    void setdry(float value)
    {
        dry = value*scaledry;
    }

    
    void	update() {
        // Recalculate internal values after parameter change
        //    int i;
    }
    
    float	gain;
    float	roomsize;
    float	damp;
    float	wet;
    float	dry;
    
    
    // The following are all declared inline
    // to remove the need for dynamic allocation
    // with its subsequent error-checking messiness
    
    // Comb filters
    comb	combL[plate_numcombs];
    
    // Allpass filters
    allpass	allpassL[plate_numallpasses1+plate_numallpasses2];
    
    // Buffers for the combs
    float	bufcombL1[plate_combtuningL1];
    float	bufcombL2[plate_combtuningL2];
    float	bufcombL3[plate_combtuningL3];
    float	bufcombL4[plate_combtuningL4];
    float	bufcombL5[plate_combtuningL5];
    float	bufcombL6[plate_combtuningL6];
    
    
    // Buffers for the allpasses
    float	bufallpassL1[plate_allpasstuningL1];
    float	bufallpassL2[plate_allpasstuningL2];
    float	bufallpassL3[plate_allpasstuningL3];
    float	bufallpassL4[plate_allpasstuningL4];
    float	bufallpassL5[plate_allpasstuningL5];
    float	bufallpassL6[plate_allpasstuningL6];
    
    void processAudio(AudioBuffer& buffer){
        float _mix = getParameterValue(PARAMETER_A);
        float _roomsize = getParameterValue(PARAMETER_B);
        float _damp = getParameterValue(PARAMETER_C);
        //    float _gain = getParameterValue(PARAMETER_D);
        setdry(1.0f-_mix);
        setwet(_mix);
        setroomsize(_roomsize);
        setdamp(_damp);
        float outL,input;
        
        float* inputL = buffer.getSamples(0);
        int numsamples = buffer.getSize();
        while(numsamples-- > 0)
        {
            outL = 0;
            input = (*inputL) * gain;
            
            // Feed through allpasses in series
            for(int i=0; i<plate_numallpasses1; i++)
                outL = allpassL[i].process(outL);
			
            // Accumulate comb filters in parallel
            for(int i=0; i<plate_numcombs; i++)
                outL += combL[i].process(input);
			
            // Feed through allpasses in series
            for(int i=0; i<plate_numallpasses2; i++)
                outL = allpassL[plate_numallpasses1+i].process(outL);
			
            // Calculate output MIXING with anything already there
            *inputL = outL*wet + *inputL*dry;
            // Increment sample pointers, allowing for interleave (if any)
            inputL += 1;
        }
    }
};


////////////////////////////////////////////////////////////////////////////////////////////////////
