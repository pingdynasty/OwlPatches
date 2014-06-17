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

#ifndef __CompressorPatch_hpp__
#define __CompressorPatch_hpp__

#include "StompBox.h"
#define REQUEST_BUFFER_SIZE 32768

class CompressorPatch : public Patch {
public:
    int samplerate = (float)getSampleRate();
    float yL_prev[2];
//    float *compressorGain;
    
    CompressorPatch()
    {
        registerParameter(PARAMETER_A, "Threshold");
        registerParameter(PARAMETER_B, "Ratio");
        registerParameter(PARAMETER_C, "Attack");
        registerParameter(PARAMETER_D, "Release");
    }
    void processAudio(AudioBuffer &buffer)
    {
        float compressorGain;
        float threshold =  -60 + (getParameterValue(PARAMETER_A) * 60);
        float b = 10*getParameterValue(PARAMETER_B);
        float ratio = 1 + b * b;
        float attack = 0.1 + (getParameterValue(PARAMETER_C)*100);
        float release = 0.1 + (getParameterValue(PARAMETER_D)*100);
        float alphaAttack = expf(-1/(0.001 * samplerate * attack));
        float alphaRelease= expf(-1/(0.001 * samplerate * release));
        
        for(int channel = 0; channel < buffer.getChannels(); channel++)
        {
            int size = buffer.getSize();
            float* singleChannel = buffer.getSamples(channel);
            for(int i=0; i<size; i++)
            {
                compressorGain = compressor(singleChannel[i], channel, threshold, ratio, alphaAttack, alphaRelease);
                singleChannel[i] *= compressorGain;
            }
        }
    }
    
    float compressor(float sample, int channel, float threshold, float ratio, float attack, float release)
    {
//        if(release < attack)
//            release = attack;
        float x_g, x_l, y_g, y_l, c;
        float kneeWidth = 5;
        float makeupGain = (1/ratio-1) * threshold / 2; // Auto Gain Calculation
        
        //Level detection- estimate level using peak detector
        if (fabs(sample) < 0.000001)
            x_g =-120;
        else
            x_g = 20*log10(fabs(sample));
        
        // Apply second order interpolation soft knee
        if ((2* fabs(x_g-threshold)) <= kneeWidth)
            y_g = x_g + (1/ratio -1) * powf(x_g-threshold+kneeWidth/2,2) / (2*kneeWidth);
        else if ((2*(x_g-threshold)) > kneeWidth)
            y_g = threshold + (x_g - threshold) / ratio;
        else
            y_g = x_g;
        
        x_l = x_g - y_g;
        
        //Ballistics- smoothing of the gain
        if (x_l > yL_prev[channel])
            y_l = attack * yL_prev[channel] + (1 - attack ) * x_l;
        else
            y_l = release * yL_prev[channel] + (1 - release) * x_l;
        c = powf(10,(makeupGain - y_l)/20);
        yL_prev[channel] = y_l;
        return c;
    }
};

#endif   // __CompressorPatch_hpp__
