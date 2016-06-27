////////////////////////////////////////////////////////////////////////////////////////////////////
// 2014-01-12 - blondinou - first version
// 2014-02-15 - blondinou - soft knobs
//                          move resonance modulation computation inside main loop to avoid clicks
//                          fix stereo
//                          added expression pedal
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PsycheFilterPatch_hpp__
#define __PsycheFilterPatch_hpp__

#include "StompBox.h"

#define PSYF_TWOPI 6.2831853071f

#define PSYF_MOD_SPEED 0.16f
#define PSYF_MOD_LEVEL 1.0f
#define PSYF_WET_DRY_COMPENSATION 1.5f
#define PSYF_RES_MIN 0.15f
#define PSYF_RES_MULT 0.3f
#define PSYF_KNOB_STEP	0.02f

class PsycheFilterPatch : public Patch {
public:
	PsycheFilterPatch() : 
		k(0.0f),
		p(0.0f),
		r(0.0f),
		oldX(0.0f),
		x(0.0f),
		buf0(0.0f),
		buf1(0.0f),
		buf2(0.0f),
		phase(0.0f)
	{
		registerParameter(PARAMETER_A, "Cutoff");
		registerParameter(PARAMETER_B, "Resonance");
		registerParameter(PARAMETER_C, "Modulation");
		registerParameter(PARAMETER_D, "Dry/Wet");

		sampleRate = getSampleRate();
		for (int i = 0; i < 6; i++) { knobs[i] = 0.f; }
		for (int i = 0; i < 2; i++) { oldBuf0[i] = 0.0f; oldBuf1[i] = 0.0f; oldBuf2[i] = 0.0f; buf3[i] = 0.0f; }
	}

	void processAudio(AudioBuffer &buffer) {
		// apply filter
		float level = knobs[PARAMETER_D];
		int size = buffer.getSize();
		for (int i = 0; i < size; i++) {
			if (parametersChanged()) {
				// update filter factors if knobs were moved
				updateFactors();
			}

			// modulate resonance
			float q = r;
			if (knobs[PARAMETER_C] > 0.0f) {
				phase += PSYF_MOD_SPEED * knobs[PARAMETER_C] / (float)size;
				if (phase >= 1.0f) {
					phase -= 1.0f;
				}
				q += q * PSYF_MOD_LEVEL * sinf(phase * PSYF_TWOPI);
			}

			for (int ch = 0; ch < buffer.getChannels(); ++ch) {
				float* buf = buffer.getSamples(ch);
				// apply filter
				x = buf[i] - q * buf3[ch];

				buf0 = x * p + oldX * p - k * buf0;
				buf1 = buf0 * p + oldBuf0[ch] * p - k * buf1;
				buf2 = buf1 * p + oldBuf1[ch] * p - k * buf2;
				buf3[ch] = buf2 * p + oldBuf2[ch] * p - k * buf3[ch];

				buf3[ch] -= (buf3[ch] * buf3[ch] * buf3[ch]) / 6.0f;

				oldX = x;
				oldBuf0[ch] = buf0;
				oldBuf1[ch] = buf1;
				oldBuf2[ch] = buf2;

				buf[i] = (1.0f - level) * buf[i] + level * buf3[ch] * PSYF_WET_DRY_COMPENSATION;
			}
		}
	}

private:
	float knobs[6];
	float k;
	float p;
	float r;
	float oldX;
	float oldBuf0[2];
	float oldBuf1[2];
	float oldBuf2[2];
	float x;
	float buf0;
	float buf1;
	float buf2;
	float buf3[2];
	float phase;
	double sampleRate;

	bool parametersChanged() {
		return getParameterValue(PARAMETER_A) != knobs[PARAMETER_A]
			|| getParameterValue(PARAMETER_B) != knobs[PARAMETER_B]
			|| getParameterValue(PARAMETER_C) != knobs[PARAMETER_C]
			|| getParameterValue(PARAMETER_D) != knobs[PARAMETER_D]
			|| getParameterValue(PARAMETER_E) != knobs[PARAMETER_E];
	}

	inline void updateKnobs() {
		// update knobs
		float diff = knobs[PARAMETER_A] - getParameterValue(PARAMETER_A);
		if (diff >= PSYF_KNOB_STEP) {
			knobs[PARAMETER_A] -= PSYF_KNOB_STEP;
		} else if (diff <= -PSYF_KNOB_STEP) {
			knobs[PARAMETER_A] += PSYF_KNOB_STEP;
		} else {
			knobs[PARAMETER_A] = getParameterValue(PARAMETER_A);
		}

		diff = knobs[PARAMETER_B] - getParameterValue(PARAMETER_B);
		if (diff >= PSYF_KNOB_STEP) {
			knobs[PARAMETER_B] -= PSYF_KNOB_STEP;
		} else if (diff <= -PSYF_KNOB_STEP) {
			knobs[PARAMETER_B] += PSYF_KNOB_STEP;
		} else {
			knobs[PARAMETER_B] = getParameterValue(PARAMETER_B);
		}

		diff = knobs[PARAMETER_C] - getParameterValue(PARAMETER_C);
		if (diff >= PSYF_KNOB_STEP) {
			knobs[PARAMETER_C] -= PSYF_KNOB_STEP;
		} else if (diff <= -PSYF_KNOB_STEP) {
			knobs[PARAMETER_C] += PSYF_KNOB_STEP;
		} else {
			knobs[PARAMETER_C] = getParameterValue(PARAMETER_C);
		}

		diff = knobs[PARAMETER_D] - getParameterValue(PARAMETER_D);
		if (diff >= PSYF_KNOB_STEP) {
			knobs[PARAMETER_D] -= PSYF_KNOB_STEP;
		} else if (diff <= -PSYF_KNOB_STEP) {
			knobs[PARAMETER_D] += PSYF_KNOB_STEP;
		} else {
			knobs[PARAMETER_D] = getParameterValue(PARAMETER_D);
		}

		diff = knobs[PARAMETER_E] - getParameterValue(PARAMETER_E);
		if (diff >= PSYF_KNOB_STEP) {
			knobs[PARAMETER_E] -= PSYF_KNOB_STEP;
		} else if (diff <= -PSYF_KNOB_STEP) {
			knobs[PARAMETER_E] += PSYF_KNOB_STEP;
		} else {
			knobs[PARAMETER_E] = getParameterValue(PARAMETER_E);
		}
	}

	void updateFactors() {
		updateKnobs();

		// compute filter factors
		float cutoff = knobs[PARAMETER_A];
		float resonance = PSYF_RES_MIN + PSYF_RES_MULT * knobs[PARAMETER_B];
		// use expression pedal if B & C =0
		if (knobs[PARAMETER_B] == 0.0f && knobs[PARAMETER_C] == 0.0f) {
			cutoff = knobs[PARAMETER_E];
		}
		cutoff = (cutoff * 4250.0f) + 250.0f;
		double sampleRate = getSampleRate();

		if (sampleRate == 0) {
			sampleRate = 44100.0;
		}

		float f = (cutoff + cutoff) / (float)sampleRate;
		p = f * (1.8f - 0.8f * f);
		k = p + p - 1.0f;
		
		float t = (1.f - p) * 1.386249f;
		float t2 = 12.0f + t * t;
		r = resonance * (t2 + 6.0f * t) / (t2 - 6.0f * t);
	}
};

#endif // __PsycheFilterPatch_hpp__
