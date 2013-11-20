#ifndef __FourBandsEqPatch_hpp__
#define __FourBandsEqPatch_hpp__

#include "StompBox.h"
#include <math.h>

/*
 * 4 bands EQ Patch.
 * Controls :
 * ParameterA = Gain of first band (lowShelf)
 * ParameterB = Gain of second band (PEQ)
 * ParameterC = Gain of third band (PEQ)
 * ParameterD = Gain of fourth band (High Shelf)
 *
 * Basic version without coefficients smoothing
 * G Le Nost, for the Owl team
 */


/****************************************************************************************************
 * Biquad Parametric EQ filter class
 */

enum filterType {
    PEQ, // Parametric EQ
    HSH, // High Shelf
    LSH  // Low SHelf
};
#define Q_BUTTERWORTH   0.707

class BiquadDF1 {
public:
    BiquadDF1() {}
    ~BiquadDF1() {}
    
    void initStateVariables(){
        x1=0.f;
        x2=0.f;
        y1=0.f;
        y2=0.f;
    }
    
    // function used for PEQ, HSH, LSH
    void setCoeffs(float normalizedFrequency, float Q, float dbGain){
        
        float alpha, c, omega, d, e, gamma, beta ;
        
        omega = 2*M_PI*normalizedFrequency ;
        c = cosf(omega) ;
        alpha = sinf(omega)/(2*Q);
        d = powf(10,dbGain/40.f);
        gamma = alpha*powf(10,fabsf(dbGain)/40.f);
        e = powf(10,fabsf(dbGain)/20.f);
        beta = 2*alpha*powf(e,0.5f);
        
        switch (fType)
        {
            case PEQ: // Parametric EQ
                a[0]=1+gamma/d;
                a[1]=-2*c/a[0];
                a[2]=(1-gamma/d)/a[0];
                b[0]=(1+gamma*d)/a[0];
                b[1]=a[1];
                b[2]=(1-gamma*d)/a[0];
                a[0]=1;
                break;
                
            case HSH: // High Shelf
                if (dbGain >0){
                    a[0]=2*(1+alpha);
                    a[1]=-4*c/a[0];
                    a[2]=2*(1-alpha)/a[0];
                    b[0]=((1+e)-(1-e)*c+beta)/a[0];
                    b[1]=2*((1-e)-(1+e)*c)/a[0];
                    b[2]=((1+e)-(1-e)*c-beta)/a[0];
                    a[0]=1;
                }
                else {
                    a[0]=(1+e)-(1-e)*c+beta;
                    a[1]=2*((1-e)-(1+e)*c)/a[0];
                    a[2]=((1+e)-(1-e)*c-beta)/a[0];
                    b[0]=2*(1+alpha)/a[0];
                    b[1]=-4*c/a[0];
                    b[2]=2*(1-alpha)/a[0];
                    a[0]=1;
                }
                break;
                
            case LSH: // Low Shelf
                if (dbGain >0){
                    a[0]=2*(1+alpha);
                    a[1]=-4*c/a[0];
                    a[2]=2*(1-alpha)/a[0];
                    b[0]=((1+e)+(1-e)*c+beta)/a[0];
                    b[1]=-(2*((1-e)+(1+e)*c))/a[0];
                    b[2]=((1+e)+(1-e)*c-beta)/a[0];
                    a[0]=1;
                    
                }
                else {
                    a[0]=(1+e)+(1-e)*c+beta;
                    a[1]=-2*((1-e)+(1+e)*c)/a[0];
                    a[2]=((1+e)+(1-e)*c-beta)/a[0];
                    b[0]=(2*(1+alpha))/a[0];
                    b[1]=-4*c/a[0];
                    b[2]=2*(1-alpha)/a[0];
                    a[0]=1;
                }
                break;
        }
        
    }
    
    void process (int numSamples, float* buf){
        float out;
        for (int i=0;i<numSamples;i++){
            out = b[0]*buf[i]+b[1]*x1+b[2]*x2-a[1]*y1-a[2]*y2 ;
            y2 = y1;
            y1 = out;
            x2 = x1;
            x1 = buf[i];
            buf[i]=out;
        }
    }
    
    void setType (filterType typ){
        fType = typ;
    }
    
private:
    float a[3] ; // ai coefficients
    float b[3] ; // bi coefficients
    float x1, x2, y1, y2 ; // state variables to compute samples
    filterType fType;
};

/**
 * Parametric EQ OWL Patch
 */
class FourBandsEqPatch : public Patch {
public:
  FourBandsEqPatch() {
    registerParameter(PARAMETER_A, "Low", "Low");
    registerParameter(PARAMETER_B, "Lo-Mid", "Lo-Mid");
    registerParameter(PARAMETER_C, "Hi-Mid", "Hi-Mid");
    registerParameter(PARAMETER_D, "High", "High");
      
    band1.initStateVariables();
    band1.setType(LSH);
    fn1=100/getSampleRate();
    band1.setCoeffs(fn1, Q_BUTTERWORTH, getDbGain(PARAMETER_A));
      
    band2.initStateVariables();
    band2.setType(PEQ);
    fn2=250/getSampleRate();
    band2.setCoeffs(fn2, Q_BUTTERWORTH, getDbGain(PARAMETER_B));
      
    band3.initStateVariables();
    band3.setType(PEQ);
    fn3=1500/getSampleRate();
    band3.setCoeffs(fn3, Q_BUTTERWORTH, getDbGain(PARAMETER_C));
      
    band4.initStateVariables();
    band4.setType(PEQ);
    fn4=4000/getSampleRate();
    band4.setCoeffs(fn4, Q_BUTTERWORTH, getDbGain(PARAMETER_D));
  }    

  void processAudio(AudioBuffer &buffer){
    // update filter coefficients
    band1.setCoeffs(fn1, Q_BUTTERWORTH, getDbGain(PARAMETER_A));
    band2.setCoeffs(fn2, Q_BUTTERWORTH, getDbGain(PARAMETER_B));
    band3.setCoeffs(fn3, Q_BUTTERWORTH, getDbGain(PARAMETER_C));
    band4.setCoeffs(fn4, Q_BUTTERWORTH, getDbGain(PARAMETER_D));
      
    // process
      float numSamples = buffer.getSize();
    float* buf = buffer.getSamples(0);
    band1.process(numSamples, buf);
    band2.process(numSamples, buf);
    band3.process(numSamples, buf);
    band4.process(numSamples, buf);
  }
    
private:
    BiquadDF1 band1, band2, band3, band4; // filters
    float fn1, fn2, fn3, fn4; // cutoffs frequencies, normalized
    
    float getDbGain(PatchParameterId id){
        float linGain = getParameterValue(id);
        // linGain = 0    <-> -15 dB
        // linGain = 0.5  <-> 0dB
        // linGain = 1    <-> 15dB
        return (linGain-0.5)*30;
    }
};

#endif // __FourBandsEqPatch_hpp__
