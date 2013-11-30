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


/* created by the OWL team 2013 */


////////////////////////////////////////////////////////////////////////////////////////////////////


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
 *  Description:
 *
 *  VibroFlange.h, created by Marek Bereza on 19/11/2013.
 */


#pragma once
#include "StompBox.h"


// for vibrato, turn C and D all the way down


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
	virtual float getSampleForPhase() = 0;
	
	

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
	
	
protected:
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








class VibroFlangePatch: public Patch {
public:
	
	float buffer[8192];
	int inPos;
	int delay;
	float depth;

	SinOscillator *lfo;

	
	VibroFlangePatch() {
		lfo = new SinOscillator();
		lfo->frequency = 0.5;
		depth = 0;
		inPos = 0;

		delay = 256;

		memset(buffer, 0, 8192*4);
		

		registerParameter(PARAMETER_A, "Speed");
		registerParameter(PARAMETER_B, "Depth");
		registerParameter(PARAMETER_C, "Flange");
		registerParameter(PARAMETER_D, "Feedback");


	}
	


	virtual void processAudio(AudioBuffer &audio) {

		float* y = audio.getSamples(0);
		int size = audio.getSize();


		depth = getParameterValue(PARAMETER_B);
		lfo->frequency =  10 * getParameterValue(PARAMETER_A);
		float mix = getParameterValue(PARAMETER_C)*0.5;
		float feedback = getParameterValue(PARAMETER_D)*0.99;

		for(int i = 0; i < size; i++) {

			float in = y[i];
			inPos++;
			inPos %= delay;
			float fOutPos = inPos - delay / 2 + depth*delay*0.45*lfo->getSample();
			
			if(fOutPos<0) fOutPos += delay;
			
			
			
			float frac = fOutPos - (int)fOutPos;
			int nextPos = 1+(int)fOutPos;
			nextPos %= delay;
			float out = buffer[(int)fOutPos] * (1.f-frac) + buffer[nextPos] * frac;
			
			

			y[i] = out*(1-mix) + in*mix;
			buffer[inPos] = in + y[i]*feedback;
			
		}
	}
};




