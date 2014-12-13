/*Copyright (c) 2014 Giulio Moro All rights reserved. giuliomoro@yahoo.it
Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the source is acknowledged.*/

/* A class that computes and applies the coefficients of a BigMuff-style tone circuit 
using a second-order IIR filter. It has two data channels which share the filter coefficients*/
#ifndef __ToneFilter_hpp__
#define __ToneFilter_hpp__

class ToneFilter {
  public:
    ToneFilter();
    ToneFilter(float newFs);
    void setTone(float newTone);
    void setFs(float newFs);
    float processSample(float newX); //defaults to the first channel
    float processSample(float newX, int ch);
    float processSample2(float newX, int ch);
  private:
    void init(float newFs);
    void updateCoefficients(); //computes the filter coefficients
    float B[3];
    float A[3];
    float tone;//tone control, must be between 0 and 1. Defaults to 0.5.
    float fs;//sampling frequency, defaults to 48000
    float x[2][4]; //current and previous input samples, two channels
    float y[2][4];//current and previous output samples, two channels
};

ToneFilter::ToneFilter(){
  init(48000);
}
ToneFilter::ToneFilter(float newFs){
  init(newFs);
}

void ToneFilter::init(float newFs){
  for(int n=0;n<3;n++){
    B[n]=0;
    A[n]=0;
    for(int m=0;m<2;m++){
      x[m][n]=0;
      y[m][n]=0;
    }
  }
  setTone(0.5);
  setFs(newFs);
}
float ToneFilter::processSample(float newX){
  return processSample(newX, 0);
}
/*
float ToneFilter::processSample(float newX, int ch){ //Bucket-brigade style memory copy
  x[ch][0]=newX;
  y[ch][0]=x[ch][0]*B[0]+x[ch][1]*B[1]+x[ch][2]*B[2] - (A[1]*y[ch][1] + A[2]*y[ch][2]);
  y[ch][2]=y[ch][1];
  y[ch][1]=y[ch][0];
  x[ch][2]=x[ch][1];
  x[ch][1]=x[ch][0];
  return y[ch][0];
}*/
float ToneFilter::processSample(float newX, int ch){ // circular buffer with pointer wrapped with "&"
  static int pointers[2]={0,0};
  int p=pointers[ch];
  x[ch][p]=newX;
  y[ch][p]=x[ch][p]*B[0]+x[ch][(p+1)&3]*B[1]+x[ch][(p+2)&3]*B[2] - (A[1]*y[ch][(p+1)&3] + A[2]*y[ch][(p+2)&3]);
  float output=y[ch][p];
  pointers[ch]=(p-1+4)&3;
  return output;
} 
void ToneFilter::setTone(float newTone){ //float tone must be a float between 0 and 1 (no check is performed)
  tone=newTone;
  updateCoefficients();
}
void ToneFilter::setFs(float newFs){
  fs=newFs;
  updateCoefficients();
}
void ToneFilter::updateCoefficients(){/* Computes the filter coefficients for a bigMuff-style tone circuit, adpated from Matlab code by Guillaume Le Nost */
  // Values from schematics
  const float pot = 100e3; // tone pot
  const float cb = 3.3e-9; // C9  +/- 5%
  const float rb = 27e3 ; // R5 // better than 27e3 (nominal value)
  const float r1 = pot * (1-tone);
  const float rt = 27e3; // R8
  const float ct = 10e-9; // C8  better than 10e-9. Better than 0.9*10e-9 for Ton=0.5
  float r2 = pot * (tone) ;

  // Equation from circuit analysis (H(s))
  float b2 = ct*cb*r2*rt;
  float b1 = cb*(r1+r2+rt);
  float b0 = 1+ r1/rb;
  float a2 = ct*rt*cb*(r1+r2);
  float a1 = ct*rt*(1+(r1+r2)/rb) + cb*(r1+r2+rt);
  float a0 = 1+(r1+r2+rt)/rb;

  // Bilinear Transform
  float c = 2*fs; // c=2/T
  float cc = c*c; // cc=c^2;
  float value=0.5-tone;
  float gain=3*( value>=0 ? value: -value ); //gain=3*(abs(0.5-tone));
  gain=powf(10,(gain/20)); //from dB to linear
  float A0=a0 + a1*c + a2*cc;
  A[0]= 1;
  A[1] = (2*a0 - 2*a2*cc)/A0;
  A[2] = (a0 - a1*c + a2*cc)/A0;
  
  float gainOverA0=gain/A0;
  B[0] = (b0 + b1*c + b2*cc)*gainOverA0;
  B[1]= (2*b0 - 2*b2*cc)*gainOverA0;
  B[2] = (b0 - b1*c + b2*cc)*gainOverA0;
}
//~ int main(){return 0;}

#endif // __ToneFilter_hpp__

