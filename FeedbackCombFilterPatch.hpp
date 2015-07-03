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

#ifndef __FeedbackCombFilter_h__
#define __FeedbackCombFilter_h__

#include "StompBox.h"
#include "JuceHeader.h"
class FeedbackCombFilter : public Patch {
public:
	FeedbackCombFilter():
		delayBuffer_(1,1)
	{
		registerParameter(PARAMETER_A, "Frequency");
		registerParameter(PARAMETER_B, "Spread");
		registerParameter(PARAMETER_C, "Feedback");
		registerParameter(PARAMETER_D, "Depth");
		delayBuffer_.setSize(2, delayBufferLength_);
		delayBuffer_.clear();
	}
	void processAudio(AudioBuffer &buffer){
		frequency_ = 10 * pow(40, getParameterValue(PARAMETER_A));
		spread_ = getParameterValue(PARAMETER_B);
		float newFeedback = getParameterValue(PARAMETER_C)*(1.0 / (numVoices_ - 1))*1.6; //this range allows to push above the limit but should be limited by the limiter below
		depth_ = getParameterValue(PARAMETER_D);
		for (int j = 0; j < numVoices_ - 1; j++){
			frequencies_[j] = frequency_*(1 + spread_*j);
			newDelays_[j] = 1 / frequencies_[j];
		}
		const int numSamples = buffer.getSize();          // How many samples in the buffer for this block?
		int dpw; // dpr = delay read pointer; dpw = delay write pointer
		float dpr;
		static float compression = 1;
		//for (int channel = 0; channel < buffer.getChannels(); ++channel)
		for (int channel = 0; channel < 1; ++channel)
		{
			// channelData is an array of length numSamples which contains the audio for one channel
			float* channelData = buffer.getSamples(channel);
			// delayData is the circular buffer for implementing delay on
			float* delayData = delayBuffer_.getSampleData(0);
			dpw = delayWritePosition_;
			for (int i = 0; i < numSamples; ++i)
			{
				//channelData[i] = rand() / (float)RAND_MAX * 2 - 1;
				float in = channelData[i];
				in /= numVoices_;
				float interpolatedSample = 0.0;
				for (int j = 0; j < numVoices_ - 1; ++j)
				{
					currentDelays_[j] = newDelays_[j] * 0.001 + currentDelays_[j] * 0.999;
					dpr = fmodf((float)dpw - (float)(currentDelays_[j] * getSampleRate()) + (float)delayBufferLength_,
						(float)delayBufferLength_);
					//--linear interpolation
					// Find the fraction by which the read pointer sits between two
					// samples and use this to adjust weights of the samples
					float fraction = dpr - floorf(dpr);
					int previousSample = (int)floorf(dpr);
					int nextSample = (previousSample + 1) % delayBufferLength_;
					interpolatedSample = fraction*delayData[nextSample]
						+ (1.0f - fraction)*delayData[previousSample];
					//linear interpolation--
					if (!(interpolatedSample < 1 && interpolatedSample >= -1)) //apply a simple limiter to the feedback loop
					{
						interpolatedSample = interpolatedSample>1 ? 0.9999 : -1;
						//interpolatedSample = rand() / (float)RAND_MAX * 2 - 1;
						compression *= 0.999;
					}
					compression = compression*0.9 + 0.1;
					feedback_ = newFeedback*0.1*compression + feedback_*0.9;

					// Store the output sample in the buffer, which starts by containing the input sample
					if (j == 0) { //if it's the first voice, overwrite the delay buffer
						delayData[dpw] = in;
					}
					delayData[dpw] += interpolatedSample*feedback_; //otherwise sum back to it the feedback
					channelData[i] += depth_ * interpolatedSample;
				}
				//channelData[i] *= 0.25;
				if (++dpw >= delayBufferLength_)
					dpw = 0;
			}
		}
		delayWritePosition_ = dpw;
	}
private:
	AudioSampleBuffer delayBuffer_;
	int delayWritePosition_=0;
	int delayBufferLength_=10000;
	float currentDelays_[4];
	float newDelays_[4];
	float frequencies_[4];
	int numVoices_ = 5;
	float frequency_;
	float depth_;
	float feedback_=0;
	float spread_;
};

#endif // _FeedbackCombFilter_h__


////////////////////////////////////////////////////////////////////////////////////////////////////
