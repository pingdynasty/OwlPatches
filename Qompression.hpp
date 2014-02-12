////////////////////////////////////////////////////////////////////////////////////////////////////
// 2014-01-16 - blondinou - first version
////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __QompressionPatch_hpp__
#define __QompressionPatch_hpp__

#include "StompBox.h"

#define SENS_RATE 0.4f
#define SENS_OFFSET 0.15f
#define OFFSET_RATE 0.5f

class QompressionPatch : public Patch {
public:
	QompressionPatch() : a(0.0f), b(1.0f)
	{
		registerParameter(PARAMETER_A, "Sensitivity");
		registerParameter(PARAMETER_B, "Compression");
		//registerParameter(PARAMETER_C, "Morph");
		//registerParameter(PARAMETER_D, "Level");

		sampleRate = getSampleRate();
		for (int i = 0; i < 6; i++) { knobs[i] = 0.f; }
	}

	void processAudio(AudioBuffer &buffer) {
		if (parametersChanged()) {
			// update filter factors if knobs were moved
			updateFactors();
		}

		// apply filter
		//float level = knobs[PARAMETER_D];
		int size = buffer.getSize();
		for (int ch = 0; ch < buffer.getChannels(); ++ch) {
			float* buf = buffer.getSamples(ch);
			float sig, val;
			for (int i = 0; i < size; i++) {
				if (a != 0.0f && b != 1.0f) {
					// compression
					sig = 1.0f;
					val = buf[i];
					if (val < 0.0f) {
						sig = -1.0f;
					}

					buf[i] = sig * a * val * val + b * val;

					if (buf[i] > 1.0f) {
						buf[i] = 1.0f;
					} else if (buf[i] < -1.0f) {
						buf[i] = -1.0f;
					}
				}
			}
		}
	}

private:
	float knobs[6];
	float a;
	float b;
	double sampleRate;

	bool parametersChanged() {
		return getParameterValue(PARAMETER_A) != knobs[PARAMETER_A]
			|| getParameterValue(PARAMETER_B) != knobs[PARAMETER_B];
	}

	void updateFactors() {
		// update 
		knobs[PARAMETER_A] = getParameterValue(PARAMETER_A);
		knobs[PARAMETER_B] = getParameterValue(PARAMETER_B);
		
		// update samplerate
		double sampleRate = getSampleRate();
		if (sampleRate == 0) {
			sampleRate = 44100.0;
		}

		// compression : interpolate trinomial
		if (knobs[PARAMETER_A] > 0.0f) {
			float s = SENS_OFFSET + knobs[PARAMETER_A] * SENS_RATE;
			float o = s * (1 + knobs[PARAMETER_B] * OFFSET_RATE);

			float s2 = s*s;
			b = (o - s2) / (s - s2);
			a = 1 - b;
		} else {
			a = 0.0f;
			b = 1.0f;
		}
	}
};

#endif // __QompressionPatch_hpp__
