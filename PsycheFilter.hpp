////////////////////////////////////////////////////////////////////////////////////////////////////
// 2014-01-12 - blondinou - first version
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PsycheFilterPatch_hpp__
#define __PsycheFilterPatch_hpp__

#include "StompBox.h"
#include <math.h>

#define TWOPI 6.2831853071f

#define MOD_SPEED 0.14f
#define MOD_LEVEL 1.0f
#define WET_DRY_COMPENSATION 1.5f
#define RES_MIN 0.1f
#define RES_MULT 0.4f

class PsycheFilterPatch : public Patch {
public:
	PsycheFilterPatch() : 
		k(0.0f),
		p(0.0f),
		r(0.0f),
		oldX(0.0f),
		oldBuf0(0.0f),
		oldBuf1(0.0f),
		oldBuf2(0.0f),
		x(0.0f),
		buf0(0.0f),
		buf1(0.0f),
		buf2(0.0f),
		buf3(0.0f),
		phase(0.0f)
	{
		registerParameter(PARAMETER_A, "Resonance");
		registerParameter(PARAMETER_B, "Cutoff", "Cutoff frequency");
		registerParameter(PARAMETER_C, "Modulation", "Resonance modulation speed");
		registerParameter(PARAMETER_D, "Level", "Effect level");

		sampleRate = getSampleRate();
		for (int i = 0; i < 6; i++) { knobs[i] = 0.f; }
	}

	void processAudio(AudioBuffer &buffer) {
		if (parametersChanged()) {
			// update filter factors if knobs were moved
			updateFactors();
		}

		// modulation resonance
		float q = r;
		if (knobs[PARAMETER_C] > 0.0f) {
			phase += MOD_SPEED * knobs[PARAMETER_C];
			if (phase >= 1.0) {
				phase -= 1.0;
			}
			q += q * MOD_LEVEL * sinf(phase * TWOPI);
		}

		// apply filter
		float level = knobs[PARAMETER_D];
		int size = buffer.getSize();
		for (int ch = 0; ch < buffer.getChannels(); ++ch) {
			float* buf = buffer.getSamples(ch);
			for (int i = 0; i < size; i++) {
				x = buf[i] - q * buf3;

				buf0 = x * p + oldX * p - k * buf0;
				buf1 = buf0 * p + oldBuf0 * p - k * buf1;
				buf2 = buf1 * p + oldBuf1 * p - k * buf2;
				buf3 = buf2 * p + oldBuf2 * p - k * buf3;

				buf3 -= (buf3 * buf3 * buf3) / 6.f;

				oldX = x;
				oldBuf0 = buf0;
				oldBuf1 = buf1;
				oldBuf2 = buf2;

				buf[i] = (1.0f - level) * buf[i] + level * buf3 * WET_DRY_COMPENSATION;
			}
		}
	}

private:
	float knobs[6];
	float k;
	float p;
	float r;
	float oldX;
	float oldBuf0;
	float oldBuf1;
	float oldBuf2;
	float x;
	float buf0;
	float buf1;
	float buf2;
	float buf3;
	float phase;
	double sampleRate;

	bool parametersChanged() {
		return getParameterValue(PARAMETER_A) != knobs[PARAMETER_A]
			|| getParameterValue(PARAMETER_B) != knobs[PARAMETER_B]
			|| getParameterValue(PARAMETER_C) != knobs[PARAMETER_C]
			|| getParameterValue(PARAMETER_D) != knobs[PARAMETER_D];
	}

	void updateFactors() {
		// update 
		knobs[PARAMETER_A] = getParameterValue(PARAMETER_A);
		knobs[PARAMETER_B] = getParameterValue(PARAMETER_B);
		knobs[PARAMETER_C] = getParameterValue(PARAMETER_C);
		knobs[PARAMETER_D] = getParameterValue(PARAMETER_D);

		// compute filter factors
		float resonance = RES_MIN + RES_MULT * knobs[PARAMETER_A];
		float cutoff = knobs[PARAMETER_B];
		cutoff = (cutoff * 4250.f) + 250.f;
		double sampleRate = getSampleRate();

		if (sampleRate == 0) {
			sampleRate = 44100.0;
		}

		float f = (cutoff + cutoff) / (float)sampleRate;
		p = f * (1.8f - 0.8f * f);
		k = p + p - 1.f;
		
		float t = (1.f - p) * 1.386249f;
		float t2 = 12.f + t * t;
		r = resonance * (t2 + 6.f*t) / (t2 - 6.f * t);
	}
};

#endif // __PsycheFilterPatch_hpp__
