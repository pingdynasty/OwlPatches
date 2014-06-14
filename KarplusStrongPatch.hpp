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
   created by the OWL team 2014 */


////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __KarplusStrongPatch_hpp__
#define __KarplusStrongPatch_hpp__

#include "StompBox.h"

// number of samples for delay line, 1000 gives us a min. possible freq of 44100/1000 = 29.4hz (sufficient)
#define NUM_SAMPLES (1500)
#define NUM_BUFFER (1<<16)
#define NOISETYPE_GAUSSIAN 1
#define NOISETYPE_RANDOM 2

typedef struct {
  int numSamps; // N
  float noise[NUM_SAMPLES];  // white noise x(n)
  float pluck[NUM_BUFFER]; // output y(n), up to 4 seconds
  float amp;
  float duration;
  int phase;
  float g; // filter gain/string tension/decay factor
  float b; // blend factor for drum mode, b = 1 then same as string, b = 0 harp-like, b - 0.5 snare
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
    registerParameter(PARAMETER_E, "Blend");
    initializeKarplus();
  }

  void processAudio(AudioBuffer &buffer){
    data.numSamps = getParameterValue(PARAMETER_A) * NUM_SAMPLES;
    // getSampleRate()/(getParameterValue(PARAMETER_A)*(1000-30)+30);
    // data.numSamps = SAMPLE_RATE/freqSlider->getValue();  // freq = SAMPLE_RATE / # samples in delay line
    data.amp = getParameterValue(PARAMETER_B);
    // data.amp = ampSlider->getValue();
    data.g = getParameterValue(PARAMETER_C)*(0.5-0.48)+0.48;
    // data.g = tensionSlider->getValue();
    data.duration = getParameterValue(PARAMETER_D) * NUM_BUFFER;
    // data.duration = durationSlider->getValue();
    data.b = getParameterValue(PARAMETER_E);
    // data.b = bSlider->getValue();

    if(isButtonPressed(PUSHBUTTON))
      data.noteOn = true;

    int size = buffer.getSize();
    // for(int ch=0; ch<buffer.getChannels(); ++ch){
    float* left = buffer.getSamples(0);
    float* right = buffer.getChannels() > 1 ? buffer.getSamples(1) : left;
    for(int i=0; i<size; ++i){
      // put your code here!

      if(data.noteOn){
	if(data.phase > (data.numSamps +  1)){
	  // if we have filled up our delay line, y(n) = g * (y(n-N) + y( n-(N+1) ))

	  float r = rand()/RAND_MAX;
	  if(data.b <= r){
	    // kick-like
	    data.pluck[data.phase] = -1 * data.g * ( data.pluck[data.phase-data.numSamps]
						     + data.pluck[data.phase - (data.numSamps + 1)] );
	  }else{
	    // harp-like
	    data.pluck[data.phase] = data.g * ( data.pluck[data.phase-data.numSamps]
						+ data.pluck[data.phase - (data.numSamps + 1)] );
	  }
	}else{
	  // computing the first N samples, y(n) = x(n)
	  // if(data.noiseType == NOISETYPE_GAUSSIAN)
	  data.pluck[data.phase] = data.noise[data.phase]; // use gaussian white noise
	  // if(data.noiseType == NOISETYPE_RANDOM)
	  //   data.pluck[data.phase] = rand()%100000/100000.;  // use random noise
	}
	left[i] = data.amp* data.pluck[data.phase];  //left channel
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
	data.noiseType = NOISETYPE_GAUSSIAN;

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

	for (int i = 0; i < NUM_SAMPLES; i++){
	  random = ((float)rand() / (float)RAND_MAX);
	  data.noise[i] = (2.f * ((random * c2) + (random * c2) + (random * c2)) - 3.f * (c2 - 1.f)) * c3;	  
	  if(fabs(data.noise[i]) > max)
	    max = fabs(data.noise[i]);
	}

	for (int i = 0; i < NUM_SAMPLES; i++){
	  //normalize the gain of our noise
	  data.noise[i] = data.noise[i] / max;
	}

}
};

#endif // __KarplusStrongPatch_hpp__
