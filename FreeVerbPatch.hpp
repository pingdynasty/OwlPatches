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


////////////////////////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "StompBox.h"

/**     ___           ___           ___                         ___           ___     
 *     /__/\         /  /\         /  /\         _____         /  /\         /__/|    
 *    |  |::\       /  /::\       /  /::|       /  /::\       /  /::\       |  |:|    
 *    |  |:|:\     /  /:/\:\     /  /:/:|      /  /:/\:\     /  /:/\:\      |  |:|    
 *  __|__|:|\:\   /  /:/~/::\   /  /:/|:|__   /  /:/~/::\   /  /:/  \:\   __|__|:|    
 * /__/::::| \:\ /__/:/ /:/\:\ /__/:/ |:| /\ /__/:/ /:/\:| /__/:/ \__\:\ /__/::::\____
 * \  \:\~~\__\/ \  \:\/:/__\/ \__\/  |:|/:/ \  \:\/:/~/:/ \  \:\ /  /:/    ~\~~\::::/
 *  \  \:\        \  \::/          |  |:/:/   \  \::/ /:/   \  \:\  /:/      |~~|:|~~ 
 *   \  \:\        \  \:\          |  |::/     \  \:\/:/     \  \:\/:/       |  |:|   
 *    \  \:\        \  \:\         |  |:/       \  \::/       \  \::/        |  |:|   
 *     \__\/         \__\/         |__|/         \__\/         \__\/         |__|/   
 *
 *  Description: FreeVerb for the Owl
 // Written by Jezar at Dreampoint, June 2000
 // http://www.dreampoint.co.uk
 // This code is public domain
 *				 
 *  FreeVerbPatch.hpp, created by Marek Bereza on 25/06/2013.
 */

//#define undenormalise(sample) if(((*(unsigned int*)&sample)&0x7f800000)==0) sample=0.0f
#define undenormalise(sample) (sample)


class comb {
public:
  comb() {
    filterstore = 0;
    bufidx = 0;
  }

  void  setbuffer(float *buf, int size) {
    buffer = buf; 
    bufsize = size;
  }

  inline  float process(float input) {
    float output;
    output = buffer[bufidx];
//    undenormalise(output);
    filterstore = (output*damp2) + (filterstore*damp1);
//    undenormalise(filterstore);
    buffer[bufidx] = input + (filterstore*feedback);
    if(++bufidx>=bufsize) 
      bufidx = 0;
    return output;
  }

  void  mute() {
    for (int i=0; i<bufsize; i++)
      buffer[i]=0;
  }
	
  void  setdamp(float val) {
    damp1 = val; 
    damp2 = 1-val;
  }
	
  float getdamp() {
    return damp1;
  }
	
  void  setfeedback(float val) {
    feedback = val;
  }
	
  float getfeedback() {
    return feedback;
  }

private:
  float feedback;
  float filterstore;
  float damp1;
  float damp2;
  float *buffer;
  int   bufsize;
  int   bufidx;
};

class allpass {
public:
  allpass() {
    bufidx = 0;
  }
  void  setbuffer(float *buf, int size) {
    buffer = buf; 
    bufsize = size;
  }

  inline  float process(float input) {
    float output;
    float bufout;
  
    bufout = buffer[bufidx];
//    undenormalise(bufout);
  
    output = -input + bufout;
    buffer[bufidx] = input + (bufout*feedback);

    if(++bufidx>=bufsize) bufidx = 0;

    return output;
  }

  void  mute() {
    for (int i=0; i<bufsize; i++)
      buffer[i]=0;
  }
  void  setfeedback(float val) {
    feedback = val;
  }
  float getfeedback() {
    return feedback;
  }


  float feedback;
  float *buffer;
  int   bufsize;
  int   bufidx;
};





const int	numcombs		= 8;
const int	numallpasses	= 4;
const float	muted			= 0;
const float	fixedgain		= 0.015f;
const float scalewet		= 3;
const float scaledry		= 2;
const float scaledamp		= 0.4f;
const float scaleroom		= 0.28f;
const float offsetroom		= 0.7f;
const float initialroom		= 0.5f;
const float initialdamp		= 0.5f;
const float initialwet		= 1/scalewet;
const float initialdry		= 0;
const int	stereospread	= 23;

// These values assume 44.1KHz sample rate
// they will probably be OK for 48KHz sample rate
// but would need scaling for 96KHz (or other) sample rates.
// The values were obtained by listening tests.
const int combtuningL1		= 1116;
const int combtuningL2		= 1188;
const int combtuningL3		= 1277;
const int combtuningL4		= 1356;
const int combtuningL5		= 1422;
const int combtuningL6		= 1491;
const int combtuningL7		= 1557;
const int combtuningL8		= 1617;
const int allpasstuningL1	= 556;
const int allpasstuningL2	= 441;
const int allpasstuningL3	= 341;
const int allpasstuningL4	= 225;







class FreeVerbPatch : public Patch {
public:
  FreeVerbPatch(){
    gain = fixedgain;
    registerParameter(PARAMETER_A, "Mix");
    registerParameter(PARAMETER_B, "Room Size");
    registerParameter(PARAMETER_C, "Damp");
    // registerParameter(PARAMETER_D, "D");
    // registerParameter(PARAMETER_E, "E");


    // Tie the components to their buffers
    combL[0].setbuffer(bufcombL1,combtuningL1);
    combL[1].setbuffer(bufcombL2,combtuningL2);
    combL[2].setbuffer(bufcombL3,combtuningL3);
    combL[3].setbuffer(bufcombL4,combtuningL4);
    combL[4].setbuffer(bufcombL5,combtuningL5);
    combL[5].setbuffer(bufcombL6,combtuningL6);
    combL[6].setbuffer(bufcombL7,combtuningL7);
    combL[7].setbuffer(bufcombL8,combtuningL8);
    allpassL[0].setbuffer(bufallpassL1,allpasstuningL1);
    allpassL[1].setbuffer(bufallpassL2,allpasstuningL2);
    allpassL[2].setbuffer(bufallpassL3,allpasstuningL3);
    allpassL[3].setbuffer(bufallpassL4,allpasstuningL4);

    // Set default values
    allpassL[0].setfeedback(0.5f);
    allpassL[1].setfeedback(0.5f);
    allpassL[2].setfeedback(0.5f);
    allpassL[3].setfeedback(0.5f);
    setwet(initialwet);
    setroomsize(initialroom);
    setdry(initialdry);
    setdamp(initialdamp);

    // Buffer will be full of rubbish - so we MUST mute them
    mute();
  }


  void mute() {
		

    for (int i=0;i<numcombs;i++)
      {
	combL[i].mute();
      }
    for (int i=0;i<numallpasses;i++)
      {
	allpassL[i].mute();
      }
  }



  void setroomsize(float value)
  {
    roomsize = (value*scaleroom) + offsetroom;
    for(int i=0; i<numcombs; i++)
      {
	combL[i].setfeedback(roomsize);

      }
  }



  void setdamp(float value)
  {
    damp = value*scaledamp;
    for(int i=0; i<numcombs; i++)
      {
	combL[i].setdamp(damp);
      }
  }

	

  void setwet(float value) {
    wet = value*scalewet;
  }

	

  void setdry(float value) {
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
  comb	combL[numcombs];

  // Allpass filters
  allpass	allpassL[numallpasses];

  // Buffers for the combs
  float	bufcombL1[combtuningL1];
  float	bufcombL2[combtuningL2];
  float	bufcombL3[combtuningL3];
  float	bufcombL4[combtuningL4];
  float	bufcombL5[combtuningL5];
  float	bufcombL6[combtuningL6];
  float	bufcombL7[combtuningL7];
  float	bufcombL8[combtuningL8];

  // Buffers for the allpasses
  float	bufallpassL1[allpasstuningL1];
  float	bufallpassL2[allpasstuningL2];
  float	bufallpassL3[allpasstuningL3];
  float	bufallpassL4[allpasstuningL4];


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
			
	// Accumulate comb filters in parallel
	for(int i=0; i<numcombs; i++)
	  {
	    outL += combL[i].process(input);
	  }
			
	// Feed through allpasses in series
	for(int i=0; i<numallpasses; i++)
	  {
	    outL = allpassL[i].process(outL);
	  }
			
	// Calculate output MIXING with anything already there
	*inputL = outL*wet + *inputL*dry;
	// Increment sample pointers, allowing for interleave (if any)
	inputL += 1;
      }
  }
};


////////////////////////////////////////////////////////////////////////////////////////////////////
