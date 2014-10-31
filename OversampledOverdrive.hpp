#ifndef __OversampledOverdrive_hpp__
#define __OversampledOverdrive_hpp__

#define OS_FL 4 //length of the filter
#define OS_L 4 //oversampling factor

// Coefficients for Butterworth filter
// [B A]=butter(3,0.1); #for Fs=48000, cutoff=19200
//upsampling filter coefficients
// float Bu[OS_FL] = {  0.00289819463372143, 0.00869458390116429, 0.00869458390116429, 0.00289819463372143};
// float Au[OS_FL] = { 2.374094743709352, -1.929355669091215, 0.532075368312092, 0 };//removed the leading 1 and added a trailing 0, signs are inverted in order to have only additions
//downsampling filter coefficients
// float Bd[OS_FL] = {  0.00289819463372143, 0.00869458390116429, 0.00869458390116429, 0.00289819463372143};
// float Ad[OS_FL] = { 2.374094743709352, -1.929355669091215, 0.532075368312092, 0 };//removed the leading 1 and added a trailing 0, signs are inverted in order to have only additions

// Coefficients for elliptic filter
// [b a]=ellip(3,2,70,0.1); #for Fs=48000, cutoff=19200
//upsampling filter coefficients
float Bu[OS_FL] = { 0.00198291863379792 , 0.00260151378660307,   0.00260151378660307  , 0.00198291863379791};
float Au[OS_FL] = {2.699187468127642, -2.501953456902938, 0.793597123934494, 0};//removed the leading 1 and added a trailing 0, signs are inverted in order to have only additions
//downsampling filter coefficients
float Bd[OS_FL] = { 0.00198291863379792 , 0.00260151378660307,   0.00260151378660307  , 0.00198291863379791};
float Ad[OS_FL] = {2.699187468127642, -2.501953456902938, 0.793597123934494, 0};//removed the leading 1 and added a trailing 0, signs are inverted in order to have only additions


class OversampledOverdrive {
  public:
    OversampledOverdrive();
    OversampledOverdrive(int factor);
    ~OversampledOverdrive();
    float processSample(float sample);
    float processSample2(float sample);
  private:
    void init();
    float oversampledFunction(float oversampledSample);
	  float xu[OS_L]; //upsampled input signal -- output of the upsampling filter -- input to the oversampledFunction
	  int xup; //index in xu
	  float yu[OS_L];//oversampled output signal -- output of the oversampledFunction -- input to the downsampling filter
	  int yup; //index in yu
	  float yd[OS_L];//outupt of the donwsampling filter -- the output of the whole oversampling has to be taken from here (1 sample of output per each 1 sample of input to the oversampling)
	  int ydp; //index in ydp
} ;
OversampledOverdrive::OversampledOverdrive(){
  init();
}
OversampledOverdrive::OversampledOverdrive(int factor){ //factor currently ignored
  init();
}
void OversampledOverdrive::init(){
  yup=0;
	ydp = 0;
  xup = 0; 
  for(int i=0;i<OS_L;i++){
    xu[i] = 0;
    yd[i] = 0;
    yu[i] = 0;
  } 
}
OversampledOverdrive::~OversampledOverdrive(){}
 
  float OversampledOverdrive::processSample(float x){
    x*=4; //compensate for the gain of the filter
	  float xB;
    float yTemp;
	  for (int m = 0; m<OS_L; m++) {
      //oversampling
      //the oversampling filter requires at most one multiplication for the inputs, as the input is something like [x 0 0 0], so here we save a few multiplies
		  // if (m + 1 <= OS_FL) //this check is not needed as long as the length of the filter is the same as the oversampling factor
			  xB = x*Bu[m];
		  // else
			  // xB = 0;
		  float xTemp = (Au[0] * xu[xup] + Au[1] * xu[(xup + 1) & 3] + //output of the upsampling filter
			  Au[2] * xu[(xup + 2) & 3] + Au[3] * xu[(xup + 3) & 3]) + xB;
		  xup = (xup - 1 + 4) & 3;
		  xu[xup] = xTemp;
		  yu[yup] = oversampledFunction(xTemp); //applies the waveshaper or whatever
      //downsampling      
		  float ydB = Bd[0] * yu[yup] + Bd[1] * yu[(yup + 1) & 3] + Bd[2] * yu[(yup + 2) & 3]
			  + Bd[3] * yu[(yup + 3) & 3];
		  yup = (yup - 1 + 4) & 3;
		  yTemp = (Ad[0] * yd[ydp] + Ad[1] * yd[(ydp + 1) & 3] + //output of the downsampling filter
			  Ad[2] * yd[(ydp + 2) & 3] + Ad[3] * yd[(ydp + 3) & 3]) + ydB;
		  ydp = (ydp - 1 + 4) & 3;
		  yd[ydp] = yTemp;
	  }
	  return(yTemp); //the output of the downsampling can be whatever sample, we pick the last one 
  }
  inline float OversampledOverdrive::oversampledFunction(float x){ 
    return x * ( 27 + x*x ) / ( 27 + 9*x*x ); //You can replace this with your favourite waveshaping function
  }
#endif // __OversampledOverdrive_hpp__