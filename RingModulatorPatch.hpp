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
		float phase=0; // current phase of oscillator
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
	RingModulator::SinOscillator oscX;
	RingModulator::SinOscillator oscY;
	RingModulator::SinOscillator lfo;
	RingModulatorPatch(){
		registerParameter(PARAMETER_A, "Frequency");
		registerParameter(PARAMETER_B, "Multiply");
		registerParameter(PARAMETER_C, "LFO");
		registerParameter(PARAMETER_D, "Dry/Wet");
		registerParameter(PARAMETER_E, "+Frequency");
		oscX.frequency = 82.405f;
		oscY.frequency = 82.405f;
		lfo.frequency = 0.3f;
	}
	void processAudio(AudioBuffer& buffer){
		float dry = getParameterValue(PARAMETER_D);
		float wet = 1-dry;
		float mult = getParameterValue(PARAMETER_B);
		float parameterC = getParameterValue(PARAMETER_C);
		float freq = 0;
		bool cross = false;
		if (parameterC>0.95){
			cross = true;
		}
		else {
			freq = (1 - parameterC) * mult * 9000;
		}
		float EXP = getParameterValue(PARAMETER_E)*9000;
		if(EXP>0.1) { // allow for the fact that it never goes to 0
			freq += EXP - 0.1f;
		}
		int size = buffer.getSize();
		float* x = buffer.getSamples(0);
		float* y = buffer.getSamples(1);
		lfo.frequency = getParameterValue(PARAMETER_C)*20;
		if(lfo.frequency==0) lfo.phase = 0;
		mult *= freq/2.f;
		float oscSampleX;
		float oscSampleY;
		for(int i=0; i<size; ++i) {
			if (cross){
				oscSampleX = y[i];
				oscSampleY = x[i];
			}
			else {
				float lfoSample = lfo.getSample();
				oscX.frequency = freq - (1 + lfoSample)*mult;
				oscY.frequency = freq - (1 - lfoSample)*mult;//the lfo modulates osc0(left) and osc1(right) with opposite polarity
				oscSampleX = oscX.getSample();
				oscSampleY = oscY.getSample();
			}
			x[i] = x[i] * dry + x[i] * oscSampleX * wet;
			y[i] = y[i] * dry + y[i] * oscSampleY * wet;
		}
	}
};
