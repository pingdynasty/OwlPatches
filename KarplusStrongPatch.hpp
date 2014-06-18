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
/* 
   See http://www.lifeorange.com/SOFTWARE/audiocpptutorial.html
   Ported by the OWL team 2014
*/

////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __KarplusStrongPatch_hpp__
#define __KarplusStrongPatch_hpp__

#include "StompBox.h"

// number of samples for delay line, 1000 gives us a min. possible freq of 44100/1000 = 29.4hz (sufficient)
#define KP_NUM_SAMPLES (1500)
#define KP_NUM_BUFFER (1<<16)
#define KP_NOISETYPE_GAUSSIAN 1
#define KP_NOISETYPE_RANDOM 2

typedef struct {
  int numSamps; // N
  float noise[KP_NUM_SAMPLES];  // white noise x(n)
  float pluck[KP_NUM_BUFFER]; // output y(n)
  float amp;
  float duration;
  int phase;
  float g; // filter gain/string tension/decay factor
  bool noteOn;
  uint8_t noiseType;
} KarplusData;

class KarplusStrongPatch : public Patch {
private:
  KarplusData data;
public:
  KarplusStrongPatch(){
    registerParameter(PARAMETER_A, "Freq");
    registerParameter(PARAMETER_B, "Amp");
    registerParameter(PARAMETER_C, "Tension");
    registerParameter(PARAMETER_D, "Duration");
    initializeKarplus();
  }

  void processAudio(AudioBuffer &buffer){
    data.numSamps = getParameterValue(PARAMETER_A) * (KP_NUM_SAMPLES-8)+8;
    data.amp = getParameterValue(PARAMETER_B);
    data.g = getParameterValue(PARAMETER_C)*(0.5-0.48)+0.48;
    data.duration = getParameterValue(PARAMETER_D) * KP_NUM_BUFFER;

    if(isButtonPressed(PUSHBUTTON))
      data.noteOn = true;

    int size = buffer.getSize();
    float* left = buffer.getSamples(0);
    float* right = buffer.getChannels() > 1 ? buffer.getSamples(1) : left;
    for(int i=0; i<size; ++i){
      if(data.noteOn){
	if(data.phase > (data.numSamps +  1)){
	  // if we have filled up our delay line, y(n) = g * (y(n-N) + y( n-(N+1) ))
	  data.pluck[data.phase] = data.g * ( data.pluck[data.phase-data.numSamps]
					      + data.pluck[data.phase - (data.numSamps + 1)] );
	}else{
	  // computing the first N samples, y(n) = x(n)
	  if(data.noiseType == KP_NOISETYPE_GAUSSIAN)
	    data.pluck[data.phase] = data.noise[data.phase]; // use gaussian white noise
	  if(data.noiseType == KP_NOISETYPE_RANDOM)
	    data.pluck[data.phase] = rand()%100000/100000.;  // use random noise
	}
	left[i] = data.amp * data.pluck[data.phase];  // left channel
	right[i] = data.amp * data.pluck[data.phase];  // right channel
	if(data.phase >= data.duration){
	  // if we have reached the end of our duration
	  data.phase = 0;
	  data.noteOn = false;
	}else{
	  data.phase++;
	}
      }else{
	left[i] = right[i] = 0;
      }
    }
  }

  void initializeKarplus(){
	// initalize phase
	data.phase = 0;
	data.noteOn = false;
	data.noiseType = KP_NOISETYPE_GAUSSIAN;

	//generate white gaussian noise:
	// from http://www.musicdsp.org/showone.php?id=168

	/* Generate a new random seed from system time - do this once in your constructor */
	// srand(time(0));

	/* Setup constants */
	const static int q = 15;
	const static float c1 = (1 << q) - 1;
	const static float c2 = ((int)(c1 / 3)) + 1;
	const static float c3 = 1.f / c1;

	float max = 0;

	/* random number in range 0 - 1 not including 1 */
	float random = 0.f;

	for (int i = 0; i < KP_NUM_SAMPLES; i++){
	  random = ((float)rand() / (float)RAND_MAX);
	  data.noise[i] = (2.f * ((random * c2) + (random * c2) + (random * c2)) - 3.f * (c2 - 1.f)) * c3;	  
	  if(fabs(data.noise[i]) > max)
	    max = fabs(data.noise[i]);
	}

	for (int i = 0; i < KP_NUM_SAMPLES; i++){
	  //normalize the gain of our noise
	  data.noise[i] = data.noise[i] / max;
	}

}
};

#endif // __KarplusStrongPatch_hpp__
