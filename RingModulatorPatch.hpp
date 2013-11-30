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


#pragma once

#include "StompBox.h"
#ifndef TWOPI 
#define TWOPI 6.28318530717959
#endif
namespace RingModulator {

	
	
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
		virtual float getSampleForPhase() { return sin(phase); }
		
		
		
		float getSample(float freq) {
			this->frequency = freq;
			return getSample();
		}
		
		float getSample() {
			// wrap and increment phase
			phase += frequency*(TWOPI / 44100.f);//TWO_PI_BY_SAMPLERATE;
			if(phase>=TWOPI) phase -= TWOPI;
			return getSampleForPhase();
		}
		
		
	
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


};



#define ABS(X) (X>0?X:-X)
class RingModulatorPatch : public Patch {
public:

	RingModulator::SinOscillator osc;
	RingModulator::SinOscillator lfo;


	RingModulatorPatch(){
		
		registerParameter(PARAMETER_A, "Mix");
		registerParameter(PARAMETER_B, "Frequency");
		registerParameter(PARAMETER_C, "Mult");
		registerParameter(PARAMETER_D, "LFO");
		registerParameter(PARAMETER_E, "Pedal");
		osc.frequency = 82.405;
		lfo.frequency = 0.3;
	}

#define MAX(A,B) ((A>B)?A:B)

	
	void processAudio(AudioBuffer& buffer){
		float mix = getParameterValue(PARAMETER_A);
		float oneMinusMix = 1 - mix;

		float mult = getParameterValue(PARAMETER_C);
		float freq = getParameterValue(PARAMETER_B) * mult *9000;

		float EXP = getParameterValue(PARAMETER_E)*9000;
		if(EXP>0.1) { // allow for the fact that it never goes to 0
			freq += EXP - 0.1;
		}


		int size = buffer.getSize();
		float* x = buffer.getSamples(0);


		lfo.frequency = getParameterValue(PARAMETER_D)*20;
		if(lfo.frequency==0) lfo.phase = 0;


		mult *= freq/2.f;
		for(int i=0; i<size; ++i) {
			osc.frequency = freq - (1+lfo.getSample())*mult;
			float oscSample = osc.getSample();
			x[i] = x[i] * oneMinusMix + x[i] * oscSample *mix;

		}
	}
};

