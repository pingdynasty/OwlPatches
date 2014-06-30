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

/* created by Dave Moffat,
 the OWL team 2014 */


////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __PlateReverbPatch_hpp__
#define __PlateReverbPatch_hpp__

#include "StompBox.h"
#include <cmath>
#include <cstdlib>

#define REQUEST_BUFFER_SIZE 32768


class AllPassFilter
{
private:
    float delay, gain;
    float* buffer;
    int bufferLength, writePtr, readPtr1, readPtr2;
    float z1;
    
public:
    // delay times in milliseconds
    AllPassFilter(float delayTime, float feedbackGain, long samplingRate)
    {
        //OutputDebugString("start init allPass");
        gain = feedbackGain;
        delay = delayTime;
        
        bufferLength = (int)(delay * samplingRate / 1000.0f);
        while (!isPrime(bufferLength)) bufferLength++;
        buffer = new float[bufferLength];
        
        //zero out the buffer (create silence)
        for (int i = 0; i < bufferLength; i++)
            buffer[i] = 0.0f;
        
        writePtr = readPtr1 = readPtr2 = 0;
        z1 = 0.0f;
        //OutputDebugString("end init allPass");
    }
    
    ~AllPassFilter()
    {
        delete buffer;
    }
    
    // all values [0..1]
    inline float processInterpolated(float input, float delayLength, float diffuse)
    {
        // dynamic interpolated
        float offset = (bufferLength - 2.0f) * delayLength + 1.0f;
        readPtr1 = writePtr - (int)floorf(offset);
        
        if (readPtr1 < 0)
            readPtr1 += bufferLength;
        
        readPtr2 = readPtr1 - 1;
        if (readPtr2 < 0)
            readPtr2 += bufferLength;
        
        // interpolate, see paper: http://www.stanford.edu/~dattorro/EffectDesignPart2.pdf
        float frac = offset - (int)floorf(offset);
        float temp = buffer[readPtr2] + buffer[readPtr1] * (1-frac) - (1-frac) * z1;
        z1 = temp;
        
        buffer[writePtr] = diffuse * gain * temp + input;
        float output = temp - diffuse * gain * buffer[writePtr];
        
        if (++writePtr >= bufferLength)
            writePtr = 0;
        
        return output;
    }
    
    inline float process(float input, float diffuse)
    {
        float temp = buffer[readPtr1];
        buffer[readPtr1] = diffuse * gain * temp + input;
        float output = temp - diffuse * gain * buffer[readPtr1];
        
        if (++readPtr1 >= bufferLength)
            readPtr1 = 0;
        
        return output;
    }
    
    inline float process(float input)
    {
        float temp = buffer[readPtr1];
        buffer[readPtr1] = gain * temp + input;
        float output = temp - gain * buffer[readPtr1];
        
        if (++readPtr1 >= bufferLength)
            readPtr1 = 0;
        
        return output;
    }
    
    bool isPrime(int value)
    {
        bool answer = true;
        if (value == 0) value = 1;
        for (int i = 2; i <= sqrtf((float)value) ; i++)
        {
            if (value % i == 0)
            {
                answer = false;
                break;
            }
        }
        return answer;
    }
};

class CombFilter
{
private:
    float gain, minDamp;
    float* buffer;
    int readPtr1, readPtr2, writePtr;
    float z1;
    int bufferLengthDelay;
    float filterStore;
    
public:
    // delay times in milliseconds
    CombFilter(float delayTime, float minDamp, long samplingRate)
    {
        //OutputDebugString("start init combfilter");
        bufferLengthDelay = (int)(delayTime * samplingRate / 1000);
        while (!isPrime(bufferLengthDelay)) bufferLengthDelay++;
        buffer = new float[bufferLengthDelay];
        
        // Print out samples
        //File *file = new File("d:/delaytimes.txt");
        //String myXmlDoc = String((const int)bufferLengthDelay) << "\n";
        //file->appendText(myXmlDoc);
        
        //zero out the buffer (silence)
        for (int i = 0; i < bufferLengthDelay; i++)
            buffer[i] = 0.0f;
        
        writePtr = 0;
        
        //set read pointer to start of buffer
        readPtr1 = 0;
        z1 = filterStore = 0.0f;
        this->minDamp = minDamp;
        //OutputDebugString("end init combfilter");
    }
    
    ~CombFilter()
    {
        delete buffer;
    }
    
    // delayIntensity [0..1]
    inline float processInterpolated(float input, float damp, float feedback, float delay)
    {
        float offset = (bufferLengthDelay - 2) * delay + 1.0f;
        readPtr1 = writePtr - (int)floorf(offset);
        
        if (readPtr1 < 0)
            readPtr1 += bufferLengthDelay;
        
        readPtr2 = readPtr1 - 1;
        if (readPtr2 < 0)
            readPtr2 += bufferLengthDelay;
        
        // interpolate, see paper: http://www.stanford.edu/~dattorro/EffectDesignPart2.pdf
        float frac = offset - (int)floorf(offset);
        float output = buffer[readPtr2] + buffer[readPtr1] * (1-frac) - (1-frac) * z1;
        z1 = output;
        
        damp = minDamp * damp;
        filterStore =  output * (1.0f - damp) + filterStore * damp;
        buffer[writePtr] = input + (filterStore * feedback);
        
        if (++writePtr >= bufferLengthDelay)
            writePtr = 0;
        return output;
    }
    
    inline float process(float input, float damp, float feedback, float delay)
    {
        float offset = (bufferLengthDelay - 2) * delay + 1.0f;
        readPtr1 = writePtr - (int)floorf(offset);
        
        if (readPtr1 < 0)
            readPtr1 += bufferLengthDelay;
        
        float output = buffer[readPtr1];
        filterStore =  output * (1.0f - damp) + filterStore * damp;
        buffer[writePtr] = input + (filterStore * feedback);
        
        if (++writePtr >= bufferLengthDelay)
            writePtr = 0;
        return output;
    }
    
    bool isPrime(int value)
    {
        bool answer = true;
        if (value == 0) value = 1;
        for (int i = 2; i <= sqrtf((float)value) ; i++)
        {
            if (value % i == 0)
            {
                answer = false;
                break;
            }
        }
        return answer;
    }
};

class Filter
{
private:
    float f, k, p, scale, r, x;
    float y1, y2, y3, y4, oldx, oldy1, oldy2, oldy3;
    float fCutoff, fActualCutoff;
    float Pi;
    
    float sampleRateFactor;
    
public:
    
    Filter(float sampleRate)
    {
        Pi = 3.141592653f;
        y1 = y2 = y3 = y4 = oldx = oldy1 = oldy2 = oldy3 = 0.0f;
        
        if (sampleRate <= 0.0f) sampleRate = 44100.0f;
        
        sampleRateFactor= 44100.0f / sampleRate;
        if (sampleRateFactor > 1.0f)
            sampleRateFactor = 1.0f;
        
        fActualCutoff = -1.0f;
    }
    
    inline float process(float input, float fCutoff, float fRes, bool highPass)
    {
        if (fCutoff != fActualCutoff)
        {
            fActualCutoff = fCutoff;
            fCutoff *= sampleRateFactor;
            updateValues(fCutoff * fCutoff);
        }
        
        // Clip if bigger than 1.0f
        if (input > 1.0f) input = 1.0f;
        if (input < -1.0f) input = -1.0f;
        x = input;
        
        // Four cascaded onepole filters (bilinear transform)
        y1 = (x + oldx) * p - k * y1;
        y2 = (y1 + oldy1) * p - k * y2;
        
        // Save values
        oldx = x;
        oldy1 = y1;
        if (highPass) input = input - y2;
        else input = y2;
        return input;
    }
    
    void updateValues(float fCutoff)
    {
        // Filter section MOOG VCF
        // CSound source code, Stilson/Smith CCRMA paper.
        f = fCutoff;										// [0 - 1]
        k = 3.6f * f - 1.6f * f * f - 1.0f;					// (Empirical tunning) /// !!! original (convex)
        
        p = (k + 1.0f) * 0.5f;							    // scale [0, 1]
        scale = expf((1.0f - p) * 1.386249f);				// original
    }
};


class NoiseGenerator
{
public:
    int counter;
    float actualValue;
    float actualValueFiltered;
    float filterFactor;
    float filterFactorInversePlusOne;
    
    float invertedRandomMax;
    int randomPeriod;
    
    NoiseGenerator(int sampleRate)
    {
        counter = 0;
        actualValue = actualValueFiltered = 0.0f;
        
        // No sample rate conversion here
        filterFactor = 5000.0f;
        filterFactorInversePlusOne = 1.0f / (filterFactor + 1.0f);
        
        invertedRandomMax = 1.0f / RAND_MAX;
        randomPeriod = getNextRandomPeriod();
    }
    
    // returns a random value [0..1]
    inline float tickNoise()
    {
        return rand() * invertedRandomMax;
    }
    
    // returns a lp filtered random value [0..1]
    inline float tickFilteredNoise()
    {
        // Change random value from time to time
        if (counter++ % randomPeriod == 0) // modulo
        {
            actualValue = tickNoise();
            randomPeriod = getNextRandomPeriod();
        }
        actualValueFiltered =  (actualValueFiltered * filterFactor + actualValue) * filterFactorInversePlusOne;
        return actualValueFiltered;
    }
    
    inline int getNextRandomPeriod()
    {
        return (int)(tickNoise() * 22768.0f) + 22188;
    }
    
    inline float tickFilteredNoiseFast()
    {
        return tickNoise();
    }
};

//////////////////////////////////////////////////

class PlateReverbPatch : public Patch{
private:
	static const int DELAY_LINES_COMB = 5;
	static const int DELAY_LINES_ALLPASS = 6;
    
	static const int MAX_PRE_DELAY_MS = 1000;
	float* reflectionGains;
	float* reflectionDelays;
    
	CombFilter *combFiltersPreDelay;
    
	CombFilter **combFiltersL;
	CombFilter **combFiltersR;
	NoiseGenerator **noiseGeneratorDampL;
	NoiseGenerator **noiseGeneratorDampR;
	NoiseGenerator **noiseGeneratorDelayL;
	NoiseGenerator **noiseGeneratorDelayR;
	AllPassFilter **allPassFiltersL;
	AllPassFilter **allPassFiltersR;
    
	AllPassFilter *preAllPassFilter;
    
	Filter *lpFilter;
	Filter *hpFilter;
    
    
public:
    int sampleRate = (float)getSampleRate();
    float highCut = 0.5;
    float gain = 1;

    
//    float mix, roomSize, preDelay, lowCut;
    
	PlateReverbPatch()
	{
        
        registerParameter(PARAMETER_A, "Room Size");
        registerParameter(PARAMETER_B, "Pre Delay");
        registerParameter(PARAMETER_C, "Cut Off");
        registerParameter(PARAMETER_D, "Mix");
        
        
		createDelaysAndCoefficients(DELAY_LINES_COMB + DELAY_LINES_ALLPASS, 80.0f);
        
		combFiltersPreDelay = new CombFilter((float)MAX_PRE_DELAY_MS, 0.0f, sampleRate);
        
		combFiltersL = new CombFilter *[DELAY_LINES_COMB];
		combFiltersR = new CombFilter *[DELAY_LINES_COMB];
		noiseGeneratorDampL = new NoiseGenerator *[DELAY_LINES_COMB];
		noiseGeneratorDampR = new NoiseGenerator *[DELAY_LINES_COMB];
		noiseGeneratorDelayL = new NoiseGenerator *[DELAY_LINES_COMB];
		noiseGeneratorDelayR = new NoiseGenerator *[DELAY_LINES_COMB];
		float stereoSpreadValue = 0.005f;
		float stereoSpreadSign = 1.0f;
		for (int i = 0; i < DELAY_LINES_COMB; i++)
		{
			float stereoSpreadFactor = 1.0f + stereoSpreadValue;
			if (stereoSpreadSign > 0.0f)
			{
				combFiltersL[i] = new CombFilter(reflectionDelays[i] * stereoSpreadFactor, reflectionGains[i], sampleRate);
				combFiltersR[i] = new CombFilter(reflectionDelays[i], reflectionGains[i], sampleRate);
			}
			else
			{
				combFiltersL[i] = new CombFilter(reflectionDelays[i], reflectionGains[i], sampleRate);
				combFiltersR[i] = new CombFilter(reflectionDelays[i] * stereoSpreadFactor, reflectionGains[i], sampleRate);
			}
			stereoSpreadSign *= -1.0f;
			noiseGeneratorDampL[i] = new NoiseGenerator(sampleRate);
			noiseGeneratorDampR[i] = new NoiseGenerator(sampleRate);
			noiseGeneratorDelayL[i] = new NoiseGenerator(sampleRate);
			noiseGeneratorDelayR[i] = new NoiseGenerator(sampleRate);
		}
		preAllPassFilter = new AllPassFilter(15.0f,  0.68f, sampleRate);
        
		allPassFiltersL = new AllPassFilter *[DELAY_LINES_ALLPASS];
		allPassFiltersR = new AllPassFilter *[DELAY_LINES_ALLPASS];
		for (int i = 0; i < DELAY_LINES_ALLPASS; i++)
		{
			allPassFiltersL[i] = new AllPassFilter(reflectionDelays[i + DELAY_LINES_COMB - 1] * 0.21f,  0.68f, sampleRate);
			allPassFiltersR[i] = new AllPassFilter(reflectionDelays[i + DELAY_LINES_COMB - 1] * 0.22f,  0.68f, sampleRate);
		}
		lpFilter = new Filter(sampleRate);
		hpFilter = new Filter(sampleRate);
	}
    
	~PlateReverbPatch()
	{
		delete reflectionGains;
		delete reflectionDelays;
        
		delete combFiltersL;
		delete combFiltersR;
		delete noiseGeneratorDampL;
		delete noiseGeneratorDampR;
		delete noiseGeneratorDelayL;
		delete noiseGeneratorDelayR;
		delete allPassFiltersL;
		delete allPassFiltersR;
        
		delete lpFilter;
		delete hpFilter;
		delete preAllPassFilter;
	}
    
	// All input values [0..1]
	void processAudio(AudioBuffer &buffer)
	{
        float* sampleL = buffer.getSamples(0);
        float* sampleR = buffer.getSamples(1);
        int size = buffer.getSize();
        
        // Read sensor inputs and scale
		// --------------------------------------------------
        float roomDim = getParameterValue(PARAMETER_A);
        float roomSize = roomDim * roomDim;
        float preDelay= getParameterValue(PARAMETER_B);
        float lowCut = 0.7*getParameterValue(PARAMETER_C);
        float Mix = getParameterValue(PARAMETER_D);
        float damp = 1-Mix;
        float stereoWidth = 0.25 + 0.5 * roomSize;
        
		// Very diffuse verb
		// --------------------------------------------------
		roomSize = (1.0f - roomSize);
		roomSize *= roomSize * roomSize;
		roomSize = (1.0f - roomSize);
        float scaledDamp = 0.95f * damp * damp;
        float scaledRoomSize = roomSize * 0.97f;
        float wet1 = (stereoWidth * 0.5f + 0.5f);
        float wet2 = ((1.0f - stereoWidth) * 0.5f);
        
        float noise = noiseGeneratorDampL[0]->tickNoise();
        float outL = 0.0f;
        float outR = 0.0f;
        float revL, revR;
        for(int s = 0; s < size; s++){
            outL = outR = 0.0;
            revL = (sampleL[s] + sampleR[s]);
            
            // Add small noise -> avoid cpu spikes
            revL += noise * 0.000000000001f;
            
            // ----------------- Pre Delay ----------------------
            revL = combFiltersPreDelay->process(revL, 0.0f, 0.0f, preDelay * preDelay);
            
            // ----------------- Pre AllPass --------------------
            revL += preAllPassFilter->processInterpolated(revL, 0.01f + roomSize * 0.99f, 0.97f + noise * 0.03f) * 0.2f;
            
            // ----------------- Filters ------------------------
            revL *= 0.2f;
            revL = lpFilter->process(revL, highCut, 0.0f, false);
            revL = hpFilter->process(revL, 0.95f * lowCut + 0.05f, 0.0f, true);
            revR = revL;
            // ----------------- Comb Filter --------------------

            for (int i = 0; i < DELAY_LINES_COMB; i++)
            {
                outL += combFiltersL[i]->processInterpolated(revL, scaledDamp + 0.05f * noiseGeneratorDampL[i]->tickFilteredNoiseFast(), scaledRoomSize, 0.6f * scaledRoomSize + 0.395f + 0.012f * noiseGeneratorDelayL[i]->tickFilteredNoise() * roomSize);
                outR += combFiltersR[i]->processInterpolated(revR, scaledDamp + 0.05f * noiseGeneratorDampR[i]->tickFilteredNoiseFast(), scaledRoomSize, 0.6f * scaledRoomSize + 0.398f + 0.012f * noiseGeneratorDelayR[i]->tickFilteredNoise() * roomSize);
            }
            // ----------------- AllPass Filter ------------------
            for (int i = 0; i < DELAY_LINES_ALLPASS; i++)
            {
                outL = allPassFiltersL[i]->process(outL);
                outR = allPassFiltersR[i]->process(outR);
            }
            // ----------------- Write to output / Stereo --------
            // Noise not caused here
            
            sampleL[s] = sampleL[s] * damp + Mix * (outL * wet1 + outR * wet2);
            sampleR[s] = sampleR[s] * damp + Mix * (outR * wet1 + outL * wet2);
        }
	
    }
    
	void createDelaysAndCoefficients(int numlines, float delayLength)
	{
		reflectionDelays = new float[numlines];
		reflectionGains = new float[numlines];
        
		float volumeScale = (float)(-3.0 * delayLength / log10f(1.0f));
		for (int n = numlines - 1; n >= 0; n--)
		{
			reflectionDelays[numlines -1 - n] = delayLength / powf(2.0f, (float)n / numlines);
			reflectionGains[numlines -1 - n] = powf(10.0f, - (3.0f * reflectionDelays[numlines -1 - n]) / volumeScale);
		}
	}
};


#endif   // __PlateReverbPatch_hpp__
