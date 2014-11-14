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
 
 
 Simple Delay with a Tone setting similar to the Big Muff Tone stage.
 Created by the OWL team 2014 
 
*/


////////////////////////////////////////////////////////////////////////////////////////////////////



#ifndef __LpfDelayPatch_hpp__
#define __LpfDelayPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#include <math.h>

#define REQUEST_BUFFER_SIZE 32768

class ToneBiquad {

public:
    ToneBiquad() {}
    ~ToneBiquad() {}
    void init(){
        x1 = 0.f;
        x2 = 0.f;
        y1 = 0.f;
        y2 = 0.f;
    }
    void updateStateCoeffs(){
        for (int i=0; i<3; i++) {
            pa[i]=a[i];
            pb[i]=b[i];
        }
    }
    void setCoeffs(float fc, float fs) {
        // Compute the filters coefficients a[i] and b[i]
        float w = 2*M_PI*fc/fs;
        float C = cosf(w);
        float alpha = sin(w)/1.414f; // Q Butterworth
        
        // LPF 2nd order
        b[0] = (1-C)/2;
        b[1] = 1-C;
        b[2] = b[0];
        
        a[0] = 1+alpha;
        a[1] = -2*C;
        a[2] = 1-alpha;
    }
    float processSample(float input, int numSamples,int i){
        // process 1 sample at a time, with linear parameter interpolation between start and end of block
        float a1, a2, b0, b1, b2;
        a1 = a[1]/a[0]*i+pa[1]/pa[0]*(numSamples-i);
        a2 = a[2]/a[0]*i+pa[2]/pa[0]*(numSamples-i);
        b0 = b[0]/a[0]*i+pb[0]/pa[0]*(numSamples-i);
        b1 = b[1]/a[0]*i+pb[1]/pa[0]*(numSamples-i);
        b2 = b[2]/a[0]*i+pb[2]/pa[0]*(numSamples-i);
        float output = (b0*input+b1*x1+b2*x2-a1*y1-a2*y2)/numSamples ;
        // store values for biquad state
        x2 = x1;
        x1 = input;
        y2 = y1;
        y1 = output;
        return output;
    }
private:
    float a[3] ; // ai coefficients
    float b[3] ; // bi coefficients
    float pa[3] ; // ai coefficients
    float pb[3] ; // bi coefficients
    float x1, x2, y1, y2 ; // state variables to compute samples
};

class LpfDelayPatch : public Patch {

private:
    CircularBuffer delayBuffer;
    int delay;
    float alpha, dryWet;
    ToneBiquad filter;

public:
    LpfDelayPatch() : delay(0), alpha(0.04), dryWet(0.f)
    {
        registerParameter(PARAMETER_A, "Delay");
        registerParameter(PARAMETER_B, "Feedback");
        registerParameter(PARAMETER_C, "Cutoff");
        registerParameter(PARAMETER_D, "Dry/Wet");
        registerParameter(PARAMETER_E, "Cutoff Modulation");
        AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
        delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
        filter.init();
        filter.setCoeffs(getParameterValue(PARAMETER_C), getSampleRate()); // Tone
        filter.updateStateCoeffs();
    }
    void processAudio(AudioBuffer &buffer)
    {
        float delayTime, feedback, fc, dly;
        delayTime = 0.05+0.95*getParameterValue(PARAMETER_A);
        feedback  = getParameterValue(PARAMETER_B);
        fc = 2*powf(10,3*getParameterValue(PARAMETER_C)*(1-getParameterValue(PARAMETER_E))+1)+40;
        filter.setCoeffs(fc, getSampleRate());
        
        int32_t newDelay;
        newDelay = alpha*delayTime*(delayBuffer.getSize()-1) + (1-alpha)*delay; // Smoothing
        dryWet = alpha*getParameterValue(PARAMETER_D) + (1-alpha)*dryWet;       // Smoothing
        
        float* x = buffer.getSamples(0);
        int size = buffer.getSize();
        for (int n = 0; n < size; n++)
        {
            dly = (delayBuffer.read(delay)*(size-1-n) + delayBuffer.read(newDelay)*n)/size;
            delayBuffer.write(feedback * dly + x[n]);  //filter.processSample(x[n], size, n));
            x[n] = filter.processSample(dly, size, n)*dryWet + (1.f - dryWet) * x[n];  // dry/wet
        }
        delay=newDelay;
        filter.updateStateCoeffs();
    }
};

#endif   // __LpfDelayPatch_hpp__

