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


#ifndef __ParametricEqPatch_hpp__
#define __ParametricEqPatch_hpp__

/**
 * Biquad Parametric EQ filter class
 */
class Biquad1 {
public:
  Biquad1() {}
  ~Biquad1() {}
    
  void initStateVariables(){
        x1=0.f;
        x2=0.f;
        y1=0.f;
        y2=0.f;
    }
    
  void setCoeffsPEQ(float normalizedFrequency, float Q, float dbGain) {
    // Compute the filters coefficients a[i] and b[i];
    float omega, c, alpha, d, gamma;
    omega = 2*M_PI*normalizedFrequency ;
    c = cosf(omega) ;
    alpha = sinf(omega)/(2*Q);
    d = powf(10,dbGain/40);
    gamma = alpha*powf(10,fabsf(dbGain)/40);
      
    a[0] = 1+gamma/d;
    a[1] = -2*c/a[0];
    a[2] = (1-gamma/d)/a[0];
    b[0] = (1+gamma*d)/a[0];
    b[1] = a[1];
    b[2] = (1-gamma*d)/a[0];
    a[0] = 1.0;
  }
    
  void process(int numSamples, float* input, float* out){
    // process a block of more than 2 samples. Basic implementation without coeffs interpolation.
    out[0] = b[0]*input[0]+b[1]*x1+b[2]*x2-a[1]*y1-a[2]*y2 ;
    out[1] = b[0]*input[1]+b[1]*input[0]+b[2]*x1-a[1]*out[0]-a[2]*y1 ;
    for(int i=2; i<numSamples; i++){
      out[i] = b[0]*input[i]+b[1]*input[i-1]+b[2]*input[i-2]-a[1]*out[i-1]-a[2]*out[i-2] ;
    }
      
    // store values for next block
    x1 = input[numSamples-1];
    x2 = input[numSamples-2];
    y1 = out[numSamples-1];
    y2 = out[numSamples-2];
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
    
private:
  float a[3] ; // ai coefficients
  float b[3] ; // bi coefficients
  float x1, x2, y1, y2 ; // state variables to compute samples
};

/**
 * Parametric EQ OWL Patch
 */
class ParametricEqPatch : public Patch {
public:
  ParametricEqPatch() {
    registerParameter(PARAMETER_A, "Freq", "Freq");
    registerParameter(PARAMETER_B, "Q", "Q");
    registerParameter(PARAMETER_C, "");
    registerParameter(PARAMETER_D, "Gain", "Gain");
    registerParameter(PARAMETER_E, "FreqPedal", "FreqPedal");
    peqL.initStateVariables();
    peqR.initStateVariables();
  }    

  void processAudio(AudioBuffer &buffer){
    // update filter coefficients
    float fn = getFrequency()/getSampleRate();
    float Q = getQ();
    float g = getDbGain();
    peqL.setCoeffsPEQ(fn, Q, g) ;
    peqR.setCoeffsPEQ(fn, Q, g) ;
      
    // process
    int size = buffer.getSize();
    float* left = buffer.getSamples(0);
    peqL.process(size, left);
    float* right = buffer.getSamples(1);
    peqR.process(size, right);
  }
    
private:
  Biquad1 peqL ; // PEQ filter
  Biquad1 peqR ; // PEQ filter

  float getFrequency() {
    float f = getParameterValue(PARAMETER_A)+getParameterValue(PARAMETER_E)/2;
    // param_A = 0    <-> f=50;
    // param_A = 1    <-> f=10050;
//      return powf(10,3*f+1)+40;
      return (f*8000)+50;
  }
        
  float getQ(){
    float q = getParameterValue(PARAMETER_B);
    // param_B = 0    <-> Q=0.5
    // param_B = 1    <-> Q=10
    return q*9.5+0.5;
  }
    
  float getDbGain(){
    float linGain = getParameterValue(PARAMETER_D);
    // linGain = 0    <-> -15 dB
    // linGain = 0.5  <-> 0dB
    // linGain = 1    <-> 15dB
    return (linGain-0.5)*30;
  }
};

#endif // __ParametricEqPatch_hpp__
