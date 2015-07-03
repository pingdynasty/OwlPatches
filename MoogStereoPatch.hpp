////////////////////////////////////////////////////////////////////////////////////////////////////

/*
 
 DESCRIPTION:
 Moog Ladder filter, formed by a cascade of four one pole sections, and a "Oberheim style" multiplexer.
 The multiplexer allows to design a LPF or HPF filter (or anything in between ? test at your own risks !)
 
 This implementation follows closely the one proposed by
 Antti Huovilainen and Vesa Välimäki
 in the paper:
 NEW APPROACHES TO DIGITAL SUBTRACTIVE SYNTHESIS
 Helsinki University of Technology Laboratory of Acoustics and Audio Signal Processing
 
 With the additon of a Drive parameter to excite a bit more the non linear component.
 
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

/* created by the OWL team 2014 modified by Silvere Letellier for stereo capabilities*/

////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef __MoogStereoPatch_hpp__
#define __MoogStereoPatch_hpp__

#include "StompBox.h"
#include <math.h>

/**
 * Moog Ladder filter class
 */

enum fType
{
    LPF=0,
    HPF
};

class MoogLadder
{
public:
    MoogLadder();
    ~MoogLadder();
    fType type ;
    void setType(fType t) ;
    void setCoeffs(float w0) ; // for LPF, HPF
    void process(int numSamples, float* buffer, float w0, float res, float drive, float masterGain) ;
    float processLadder(float input,float x1, float y1);
    void setMutiplexer();
    float nonLinear(float x); // tanh approx
    
private:
    float A, B, C, D, E ; // multiplexer coeffs
    float a1, b0, b1 ; // one pole section coeffs
    float pw0; // previous w0
    float pres; // previous res
    float pmasterGain; // previous master Gain
    float in1, out1, out2, out3, out4;
    float comp;
};
    
MoogLadder::MoogLadder(){
    in1 = 0.f;
    out1 = 0.f;
    out2 = 0.f;
    out3 = 0.f;
    out4 = 0.f;
    A = 0.f;
    B = 0;
    C = 0;
    D = 0;
    E = 0;
    pres=0;
    pw0=0.f;
    pmasterGain=0.f;
}

MoogLadder::~MoogLadder(){
}


void MoogLadder::setType(fType t){
    type = t;
}


void MoogLadder::setMutiplexer(){
    switch (type)
    {
        case HPF:
            A=1.0;
            B=-4.0;
            C=6.0;
            D=-4.0;
            E=1.0;
            comp=0.0;
            break;
        case LPF:
            A=0.0;
            B=0.0;
            C=0.0;
            D=0.0;
            E=1.0;
            comp=0.5;
            break;
        default:
            A=0.0;
            B=0.0;
            C=0.0;
            D=0.0;
            E=1.0;
            comp=0.5;
            break;
    }
}

void MoogLadder::setCoeffs(float w0){
    // w0 = 2pi * f/fc
    float g=1-expf(-w0);
    a1=1-g;
    b0=g/1.3;
    b1=g*0.23076923076;
}

float MoogLadder::processLadder(float input, float x1, float y1){
    float output = a1*y1 + b0*input + b1*x1;
    return output;
}

float MoogLadder::nonLinear(float x){
    // tanh approx
    if (x<-3)
        return -1;
    else if (x>3)
        return 1;
    else
        return x * ( 27 + x*x ) / ( 27 + 9*x*x );
}

void MoogLadder::process(int numSamples, float *buffer, float w0, float res, float drive, float masterGain){
    int N = numSamples-1;
    float w0i;
    float resi;
	float powBlock;
    float masterGaini;
    float a,b,c,d,e,output;
	
	w0i = w0;
	resi = res;
	masterGaini = masterGain;
	powBlock = powf(drive,0.3f);
	setCoeffs(w0i);
	
    for (int i=0;i<numSamples;i++){
		
        // compute sections
        float in = nonLinear(buffer[i]* drive);
        a=in-4*resi*(nonLinear(out4)-comp*in);
        b=processLadder(a, in1, out1);
        c=processLadder(b, out1, out2);
        d=processLadder(c,out2,out3);
        e=processLadder(d,out3,out4);
		
        // Multiplexer
        output = A*a + B*b + C*c + D*d + E*e ;
        buffer[i] = masterGaini * output / powBlock;
        // state variables update
        in1=a;
        out1=b;
        out2=c;
        out3=d;
        out4=e;

    }
    // state variables update 2
    pw0=w0;
    pres=res;
    pmasterGain=masterGain;
    
}


/**
 * Moog OWL Patch
 */
class MoogStereoPatch : public Patch {
public:
  MoogStereoPatch() {
    registerParameter(PARAMETER_A, "Cutoff");
    registerParameter(PARAMETER_B, "Resonance");
    registerParameter(PARAMETER_C, "Drive");
    registerParameter(PARAMETER_D, "Master");
    registerParameter(PARAMETER_E, "Cutoff Modulation");
    ladderL.setType(LPF);
    ladderL.setMutiplexer();
    ladderL.setCoeffs(0.f);
   
	ladderR.setType(LPF);
	ladderR.setMutiplexer();
	ladderR.setCoeffs(0.f);
  }

  void processAudio(AudioBuffer &buffer){
  float wn = 2*M_PI*getFrequency()/getSampleRate();
  float* bufL = buffer.getSamples(0);
  float* bufR = buffer.getSamples(1);
  ladderL.process(buffer.getSize(), bufL, wn, getQ(), getDrive(), getMasterGain());
  ladderR.process(buffer.getSize(), bufR, wn, getQ(), getDrive(), getMasterGain());
  }
    
private:
  MoogLadder ladderL; // Moog filterL
  MoogLadder ladderR; // Moog filterL

  float getFrequency() {
    float expr = 1 - getParameterValue(PARAMETER_E);
    float f = expr*getParameterValue(PARAMETER_A);
    // param_A = 0    <-> f=40 Hz;
    // param_A = 1    <-> f=20040 Hz;
      return 2*powf(10,3*f+1)+40;
  }
        
  float getQ(){
    float q = getParameterValue(PARAMETER_B);
    // param_B = 0    <-> Q=0.5
    // param_B = 1    <-> Q=1.1
    return q*1.1+0.1;
  }
    
  float getDrive(){
    return 1+80*getParameterValue(PARAMETER_C)*getParameterValue(PARAMETER_C);
  }
    
  float getMasterGain(){
    return getParameterValue(PARAMETER_D)*1.5;
  }
};

#endif // __MoogStereoPatch_hpp__
