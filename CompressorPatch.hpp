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
    float yL_prev;
    float compressorGain;
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
        //        float threshold =  -60 + (getParameterValue(PARAMETER_A) * 60);
        float a = getParameterValue(PARAMETER_A);
        float b = getParameterValue(PARAMETER_B);
        if(b > a)
        {
            a = 0.5*b;
        }
        float threshold =  40*(a-1);
        float ratio = 1/(1 + (5*b) * (5*b));
        float attack = 2 + (getParameterValue(PARAMETER_C)*100);
        float release = 2 + (getParameterValue(PARAMETER_D)*300);
        if(attack < release)
        {
            release = attack;
        }
        float alphaAttack = expf(-1/(0.001 * samplerate * attack));
        float alphaRelease= expf(-1/(0.001 * samplerate * release));
        float makeupGain = (1*ratio-1) * threshold * 0.5; // Auto Gain Calculation
        float kneeWidth = 2.5;
        float numChan = buffer.getChannels();
        float monoScale = 1/numChan;
        float** sampBuf;
        float sample;
        
        for(int c = 0; c < numChan; c++)
        {
            sampBuf[c] = buffer.getSamples(c);
        }
        
        int size = buffer.getSize();
        for(int i=0; i<size; i++)
        {
            sample = 0;
            for(int c = 0; c < numChan; c++)
            {
                sample += sampBuf[c][i]*monoScale;
            }
            compressorGain = compressor(sample, threshold, ratio, alphaAttack, alphaRelease, makeupGain, kneeWidth);
            for(int c = 0; c < numChan; c++)
            {
                sampBuf[c][i] *= compressorGain;
            }
        }
    }
    
    float compressor(float sample, float threshold, float ratio, float attack, float release, float makeupGain, float kneeWidth)
    {
        float x_g, x_l, y_g, y_l, c;
        
        
        //Level detection- estimate level using peak detector
        if (fabs(sample) < 0.000001)
            x_g =-120;
        else
            x_g = 20*log10f(fabs(sample));
        
        // Apply second order interpolation soft knee
        if ((float)(2* fabs(x_g-threshold)) <= kneeWidth)
            //            y_g = x_g + (1*ratio -1) * ((x_g-threshold+kneeWidth)*(x_g-threshold+kneeWidth)) / (4*kneeWidth);     // Quadratic Interpolation
            y_g = threshold - kneeWidth*0.5 + (x_g-threshold+kneeWidth*0.5)*(1+ratio)*0.5;                          // Linear Interpolation
        else if ((2*(x_g-threshold)) > kneeWidth)
            y_g = threshold + (x_g - threshold) * ratio;
        else
            y_g = x_g;
        
        x_l = x_g - y_g;
        
        //Ballistics- smoothing of the gain
        if (x_l > yL_prev)
            y_l = attack * yL_prev + (1 - attack ) * x_l;
        else
            y_l = release * yL_prev + (1 - release) * x_l;
        c = powf(10,(makeupGain - y_l)*0.05);
        yL_prev = y_l;
        return c;
    }
};

#endif   // __CompressorPatch_hpp__
