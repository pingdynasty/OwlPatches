////////////////////////////////////////////////////////////////////////////////////////////////////
// 2014-01-18 - blondinou - first version
// 2014-02-15 - blondinou - soft knobs
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __TremoloPatch_hpp__
#define __TremoloPatch_hpp__

#include "StompBox.h"

#define TREM_TWOPI 6.2831853071f
#define TREM_HALFPI	1.5707963268f

#define TREM_SPEED_MIN	0.5f
#define TREM_SPEED_RATE	8.0f
#define TREM_DEPTH_MIN	0.35f
#define TREM_DEPTH_MAX	0.95f
#define TREM_CUTOFF_MIN	0.018f
#define TREM_CUTOFF_RATE	0.18f
#define TREM_OSC_SOFTNESS	0.01f
#define TREM_WET_COMPENSATE	1.5f
#define TREM_KNOB_STEP	0.02f

class TremoloPatch : public Patch {
public:
	TremoloPatch() : speed(0.0f), depth(1.0f), wave(0.0f), phase(0.0f), buf0(0.0f), buf1(0.0f), feedback(0.0f)
	{
		registerParameter(PARAMETER_A, "Speed");
		registerParameter(PARAMETER_B, "Depth");
		registerParameter(PARAMETER_C, "Wave");
		registerParameter(PARAMETER_D, "Filter");

		sampleRate = getSampleRate();
		for (int i = 0; i < 6; i++) { knobs[i] = 0.f; }
		updateFactors();
	}

	void processAudio(AudioBuffer &buffer) {
		float level = 0.0f;
		// apply effect
		int size = buffer.getSize();
		for (int i = 0; i < size; i++) {
			if (parametersChanged()) {
				// update filter factors if knobs were moved
				updateFactors();
			}
			// compute tremolo level
			level = getTremoloLevel();

			for (int ch = 0; ch < buffer.getChannels(); ++ch) {
				float* buf = buffer.getSamples(ch);
				buf[i] = buf[i] * (1 - depth * level);

				// compute bandpass filter
				if (filter > 0.0f) {
					cutoff = TREM_CUTOFF_MIN + level * TREM_CUTOFF_RATE;
					feedback = resonance + resonance / (1.0f - cutoff);
					buf0 += cutoff * (buf[i] - buf0 + feedback * (buf0 - buf1));
					buf1 += cutoff * (buf0 - buf1);
					buf[i] = buf[i] * (1 - filter) + TREM_WET_COMPENSATE * filter * (buf0 - buf1);
				}

				// clip signal
				if (buf[i] > 1.0f) {
					buf[i] = 1.0f;
				} else if (buf[i] < -1.0f) {
					buf[i] = -1.0f;
				}
			}
		}
	}

private:
	float knobs[6];
	float speed;
	float depth;
	float wave;
	float filter;
	float phase;
	float phaseInc;
	float triangle;
	float sine;
	float square;
	float saw;
	float cutoff;
	float resonance;
	float buf0;
	float buf1;
	float feedback;
	double sampleRate;

	inline bool parametersChanged() {
		return getParameterValue(PARAMETER_A) != knobs[PARAMETER_A]
			|| getParameterValue(PARAMETER_B) != knobs[PARAMETER_B]
			|| getParameterValue(PARAMETER_C) != knobs[PARAMETER_C]
			|| getParameterValue(PARAMETER_D) != knobs[PARAMETER_D];
	}

	inline void updateKnobs() {
		// update knobs
		float diff = knobs[PARAMETER_A] - getParameterValue(PARAMETER_A);
		if (diff >= TREM_KNOB_STEP) {
			knobs[PARAMETER_A] -= TREM_KNOB_STEP;
		} else if (diff <= -TREM_KNOB_STEP) {
			knobs[PARAMETER_A] += TREM_KNOB_STEP;
		} else {
			knobs[PARAMETER_A] = getParameterValue(PARAMETER_A);
		}

		diff = knobs[PARAMETER_B] - getParameterValue(PARAMETER_B);
		if (diff >= TREM_KNOB_STEP) {
			knobs[PARAMETER_B] -= TREM_KNOB_STEP;
		} else if (diff <= -TREM_KNOB_STEP) {
			knobs[PARAMETER_B] += TREM_KNOB_STEP;
		} else {
			knobs[PARAMETER_B] = getParameterValue(PARAMETER_B);
		}

		diff = knobs[PARAMETER_C] - getParameterValue(PARAMETER_C);
		if (diff >= TREM_KNOB_STEP) {
			knobs[PARAMETER_C] -= TREM_KNOB_STEP;
		} else if (diff <= -TREM_KNOB_STEP) {
			knobs[PARAMETER_C] += TREM_KNOB_STEP;
		} else {
			knobs[PARAMETER_C] = getParameterValue(PARAMETER_C);
		}

		diff = knobs[PARAMETER_D] - getParameterValue(PARAMETER_D);
		if (diff >= TREM_KNOB_STEP) {
			knobs[PARAMETER_D] -= TREM_KNOB_STEP;
		} else if (diff <= -TREM_KNOB_STEP) {
			knobs[PARAMETER_D] += TREM_KNOB_STEP;
		} else {
			knobs[PARAMETER_D] = getParameterValue(PARAMETER_D);
		}
	}

	inline void updateFactors() {
		updateKnobs();

		speed = TREM_SPEED_MIN + TREM_SPEED_RATE * knobs[PARAMETER_A];
		depth = TREM_DEPTH_MIN + knobs[PARAMETER_B] * (TREM_DEPTH_MAX - TREM_DEPTH_MIN);
		wave = knobs[PARAMETER_C];
		phaseInc = (float)(speed / sampleRate);
		filter = knobs[PARAMETER_D];

		// oscillator factors
		if (wave <= 0.333f) {
			sine = 3.0f * wave;
			triangle = 1.0f - sine;
			square = 0.0f;
			saw = 0.0f;
		} else if (wave <= 0.667f) {
			triangle = 0.0f;
			square = 3.0f * (wave - 0.333f); 
			sine = 1.0f - square;
			saw = 0.0f;
		} else if (wave < 1.0f) {
			triangle = 0.0f;
			sine = 0.0f;
			saw = 3.0f * (wave - 0.667f); 
			square = 1.0f - saw;
		} else if (wave == 1.0f) {
			triangle = 0.0f;
			sine = 0.0f;
			saw = 1.0f;
			square = 0.0f;
		}

		// filter factors
		resonance = 0.5f;
	}

	inline float getTremoloLevel() {
		// oscillator
		float a = 0.0f, b = 0.0f, c = 0.0f, d = 0.0f;
		if (triangle > 0.0f) {
			a = 1.0f - fabs(-1.0f + (2.0f * phase));
		}
		if (sine > 0.0f) {
			b = 0.5f + sinf(phase * TREM_TWOPI - TREM_HALFPI) / 2.0f;
		}
		if (square > 0.0f) {
			if (phase < 0.5f - TREM_OSC_SOFTNESS) {
				c = 0.0f;
			} else if (phase < 0.5f) {
				c = (phase - 0.5f + TREM_OSC_SOFTNESS)* (1 / TREM_OSC_SOFTNESS);
			} else if (phase < 1.0f - TREM_OSC_SOFTNESS) {
				c = 1.0f;
			} else {
				c = (1.0f - phase)* (1 / TREM_OSC_SOFTNESS);
			}
		}
		if (saw > 0.0f) {
			if (phase < 1.0f - TREM_OSC_SOFTNESS) {
				d = phase;
			} else {
				d = (1.0f - phase)* (1 / TREM_OSC_SOFTNESS);
			}
		}

		// update oscillator phase
		phase += phaseInc;
		if (phase >= 1.0f) {
			phase -= 1.0f;
		}

		// combines oscillators
		return a * triangle + b * sine + c * square + d * saw;
	}
};

#endif // __TremoloPatch_hpp__
