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


////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __FourBandsEqPatch_hpp__
#define __FourBandsEqPatch_hpp__

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

  void copyCoeffs(BiquadDF1& other){
    memcpy(a, other.a, sizeof(float)*3);
    memcpy(b, other.b, sizeof(float)*3);
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

class FourBandsEq {
private:
    BiquadDF1 band1, band2, band3, band4; // filters
    float fn1, fn2, fn3, fn4; // cutoffs frequencies, normalized
public:
  FourBandsEq(double samplerate) {
    band1.initStateVariables();
    band1.setType(LSH);
    fn1=100/samplerate;
      
    band2.initStateVariables();
    band2.setType(PEQ);
    fn2=250/samplerate;
      
    band3.initStateVariables();
    band3.setType(PEQ);
    fn3=1500/samplerate;
      
    band4.initStateVariables();
    band4.setType(PEQ);
    fn4=4000/samplerate;
  }

  void setCoeffs(float a, float b, float c, float d){
    // update filter coefficients
    band1.setCoeffs(fn1, Q_BUTTERWORTH, a);
    band2.setCoeffs(fn2, Q_BUTTERWORTH, b);
    band3.setCoeffs(fn3, Q_BUTTERWORTH, c);
    band4.setCoeffs(fn4, Q_BUTTERWORTH, d);
  }

  void copyCoeffs(FourBandsEq& other){
    band1.copyCoeffs(other.band1);
    band2.copyCoeffs(other.band2);
    band3.copyCoeffs(other.band3);
    band4.copyCoeffs(other.band4);
  }

  void process(int numSamples, float* buf){      
    // process
    band1.process(numSamples, buf);
    band2.process(numSamples, buf);
    band3.process(numSamples, buf);
    band4.process(numSamples, buf);
  }
};

/**
 * Stereo parametric EQ OWL Patch
 */
class FourBandsEqPatch : public Patch {
private:
  FourBandsEq eqL;
  FourBandsEq eqR;
public:
  FourBandsEqPatch() : eqL(getSampleRate()), eqR(getSampleRate()) {
    registerParameter(PARAMETER_A, "Low");
    registerParameter(PARAMETER_B, "Lo-Mid");
    registerParameter(PARAMETER_C, "Hi-Mid");
    registerParameter(PARAMETER_D, "High");      
  }    

  void processAudio(AudioBuffer &buffer){
    float a = getDbGain(PARAMETER_A);
    float b = getDbGain(PARAMETER_B);
    float c = getDbGain(PARAMETER_C);
    float d = getDbGain(PARAMETER_D);
    eqL.setCoeffs(a, b, c, d);
    eqR.copyCoeffs(eqL);
 
    // process
    float numSamples = buffer.getSize();
    float* bufL = buffer.getSamples(0);
    float* bufR = buffer.getSamples(1);
    eqL.process(numSamples, bufL);
    eqR.process(numSamples, bufR);
  }
    
private:
    
  float getDbGain(PatchParameterId id){
    float linGain = getParameterValue(id);
    // linGain = 0    <-> -15 dB
    // linGain = 0.5  <-> 0dB
    // linGain = 1    <-> 15dB
    return (linGain-0.5)*30;
  }
};

#endif // __FourBandsEqPatch_hpp__


////////////////////////////////////////////////////////////////////////////////////////////////////
