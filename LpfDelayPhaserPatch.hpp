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


#ifndef __LpfDelayPhaserPatch_hpp__
#define __LpfDelayPhaserPatch_hpp__

#include "StompBox.h"
#include "CircularBuffer.hpp"
#define REQUEST_BUFFER_SIZE 32768

class LpfDelayPhaserPatch : public Patch {    
private:
  CircularBuffer delayBuffer;
  float time, olddelaySamples, dSamples;
public:    
  LpfDelayPhaserPatch() : x1(0.0f), x2(0.0f), y1(0.0f), y2(0.0f),
			  _lfoPhase( 0.f ), depth( 1.f ),
			  feedback( .7f ),_zm1( 0.f ),
              olddelaySamples (0.0f){
    AudioBuffer* buffer = createMemoryBuffer(1, REQUEST_BUFFER_SIZE);
    delayBuffer.initialise(buffer->getSamples(0), buffer->getSize());
    registerParameter(PARAMETER_A, "Delay", "Delay time");
    registerParameter(PARAMETER_B, "Feedback", "Delay loop feedback");
    registerParameter(PARAMETER_C, "Fc", "Filter cutoff frequency");
    registerParameter(PARAMETER_D, "Dry/Wet", "Dry/wet mix");
    registerParameter(PARAMETER_E, "DepthPedal", "DepthPedal");
    setCoeffs(getLpFreq()/getSampleRate(), 0.6f) ;
    setRange( 440.f, 1600.f );
    getRate( .5f );
  }
  ~LpfDelayPhaserPatch() {}
        
  void initLpf (){        
    for (int i=0 ; i<3 ; i++){
      pa[i] = a[i];
      pb[i] = b[i];
    }        
  }
    
  void setCoeffs(float normalizedFrequency, float Q) {
    // Compute the filters coefficients a[i] and b[i];
    float omega, c, alpha;
    omega = 2*M_PI*normalizedFrequency ;
    c = cosf(omega) ;
    alpha = sinf(omega)/(2*Q);        
    b[0]=(1-c)/2;
    b[1]=1-c;
    b[2]=b[0];
    a[0]=1+alpha;
    a[1]=-2*c;
    a[2]=1-alpha;
  }
    
  // mapping between knob and normalized frequency
  float getLpFreq(){
    float f;
    f = getParameterValue(PARAMETER_C);
    // param_B = 0    <-> f=0.001
    // param_B = 1    <-> f=0.5
    // return powf(10, -3*(1-f))/2.1;
    return (f*f*f/4)+0.0001;
  }
    
  bool paramChange(){
    if (a[0]-pa[0]+a[1]-pa[1]+a[2]-pa[2]+b[0]-pb[0]+b[1]-pb[1]+b[2]-pb[2] == 0.0)
      return false;
    else
      return true;
  }
    
  void setRange( float fMin, float fMax ){ // Hz
    _dmin = fMin / (getSampleRate()/2.f);
    _dmax = fMax / (getSampleRate()/2.f);
  }
    
  float getRate( float rate ){ // cps
    _lfoInc = 2.f * M_PI * (rate / getSampleRate());
    return _lfoInc * 1000.f;
  }
    
  void process(int numSamples, float* input, float* output){
    // process a block of more than 2 samples. Basic implementation without coeffs interpolation.        
    if(paramChange()) {
      output[0] = (b[0]*input[0]+b[1]*x1+b[2]*x2-a[1]*y1-a[2]*y2)/a[0] ;
      output[1] = (b[0]*input[1]+b[1]*input[0]+b[2]*x1-a[1]*output[0]-a[2]*y1)/a[0] ;
      for (int i=2; i<numSamples; i++){
	output[i] = (b[0]*input[i]+b[1]*input[i-1]+b[2]*input[i-2]-a[1]*output[i-1]-a[2]*output[i-2])/a[0] ;
      }
      // store values for next block
      x1 = input[numSamples-1];
      x2 = input[numSamples-2];
      y1 = output[numSamples-1];
      y2 = output[numSamples-2];
    } else { // we then need to interpolate the coefficients
      int N=numSamples-1;
      output[0] = (pb[0]*input[0]+pb[1]*x1+pb[2]*x2-pa[1]*y1-pa[2]*y2)/pa[0] ;
      output[1] = (pb[0]*input[1]+pb[1]*input[0]+pb[2]*x1-pa[1]*output[0]-pa[2]*y1)/pa[0]*(N-1)/N ;
      output[1] += (b[0]*input[1]+b[1]*input[0]+b[2]*x1-a[1]*output[0]-a[2]*y1)/a[0] / N;            
            
      float a1, a2, b0, b1, b2;
      for (int i=2; i<numSamples; i++){
	a1 = a[1]/a[0]*i+pa[1]/pa[0]*(N-i);
	a2 = a[2]/a[0]*i+pa[2]/pa[0]*(N-i);
	b0 = b[0]/a[0]*i+pb[0]/pa[0]*(N-i);
	b1 = b[1]/a[0]*i+pb[1]/pa[0]*(N-i);
	b2 = b[2]/a[0]*i+pb[2]/pa[0]*(N-i);
                
	output[i] = (b0*input[i]+b1*input[i-1]+b2*input[i-2]-a1*output[i-1]-a2*output[i-2])/N ;
      }
            
      // store values for next block
      x1 = input[numSamples-1];
      x2 = input[numSamples-2];
      y1 = output[numSamples-1];
      y2 = output[numSamples-2];
      pa[0]=a[0];
      pa[1]=a[1];
      pa[2]=a[2];
      pb[0]=b[0];
      pb[1]=b[1];
      pb[2]=b[2];
            
    }
  }
    
  void processAudio(AudioBuffer &buffer){        
    float *y = new float[getBlockSize()];
    float z;
    setCoeffs(getLpFreq(), 0.8f);
    rate = 0.01f;

    float delayTime = getParameterValue(PARAMETER_A); // get delay time value    
    float feedback  = getParameterValue(PARAMETER_B); // get feedback value
    float wetDry    = getParameterValue(PARAMETER_D); // get gain value
    depth           = getParameterValue(PARAMETER_E);

    if(abs(time - delayTime) < 0.01)
      delayTime = time;
    else
      time = delayTime;
        
    float delaySamples = delayTime * (delayBuffer.getSize()-1);
    int size = buffer.getSize();
    float* x = buffer.getSamples(0);
    process(size, x, y);     // low pass filter for delay buffer
    float d  = _dmin + (_dmax-_dmin) * ((sin( _lfoPhase ) + 1.f)/2.f);
        
    _lfoPhase += rate;
    if( _lfoPhase >= M_PI * 2.f )
      _lfoPhase -= M_PI * 2.f;
        
    //update filter coeffs
    for( int i=0; i<6; i++ )
      _alps[i].Delay( d );
        
    for (int n = 0; n < size; n++){
        
      //linear interpolation for delayBuffer index
      dSamples = olddelaySamples + (delaySamples - olddelaySamples) * n / size;
        
      y[n] = y[n] + feedback * delayBuffer.read(dSamples);
      y[n] = (1.f - wetDry) * x[n] + wetDry * y[n];  //crossfade for wet/dry balance
      delayBuffer.write(y[n]);
            
      //calculate output
      z = _alps[0].Update(_alps[1].Update(_alps[2].Update(_alps[3].Update(_alps[4].Update(_alps[5].Update(y[n] + _zm1 * (feedback*0.1)))))));
      _zm1 = z;
            
      x[n] = y[n] + z * depth;
    }
    olddelaySamples = delaySamples;

	delete[] y;
  }
    
private:
    
  float a[3] ; // ai coefficients
  float b[3] ; // bi coefficients
  float pa[3] ; // previous ai coefficients
  float pb[3] ; // previous bi coefficients
  float x1, x2, y1, y2 ; // state variables to compute samples
    
  class AllpassDelay{
        
  public:
    AllpassDelay()
      : _a1( 0.f )
      , _zm1( 0.f )
    {}
        
    void Delay( float delay ){ //sample delay time
      _a1 = (1.f - delay) / (1.f + delay);
    }
        
    float Update( float inSamp ){
      float y = inSamp * -_a1 + _zm1;
      _zm1 = y * _a1 + inSamp;
            
      return y;
    }
  private:
    float _a1, _zm1;
  };
    
  AllpassDelay _alps[6];
    
  float _dmin, _dmax; //range
  float _lfoPhase;
  float _lfoInc;
  float depth, rate, feedback;    
  float _zm1;
//   CircularBuffer<float, bufsize> delayBuffer;    
};

#endif


////////////////////////////////////////////////////////////////////////////////////////////////////

