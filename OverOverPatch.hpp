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


#ifndef __OverOverPatch_hpp__
#define __OverOverPatch_hpp__

#include "StompBox.h"
#include "dri50.h"
#define OS_uLength 4 //length of the upsampling filter
#define OS_dLength 4 //length of the downsampling filter
#define OS_L 4 //oversampling factor
float Bu[OS_uLength] = { 0.00407663652767764, 0.01222990958303293, 0.01222990958303293, 0.00407663652767764 };
float Au[OS_uLength] = { 2.291080705367221, -1.81226293755023, 0.488569139961591, 0 };//removed the leading 1 and added a trailing 0, signs are inverted in order to have only additions
float Bd[OS_dLength] = { 0.0140997087690443, 0.0422991263071330, 0.0422991263071330, 0.0140997087690443 };
float Ad[OS_dLength] = { 1.873027248422390, -1.300326954651049, 0.314502036076304, 0 }; //removed the leading 1 and added a trailing 0, signs are inverted in order to have only additions
float gPValues[OS_uLength + OS_dLength * 2];
float gCoefficients[OS_uLength * 2 + OS_dLength * 2];

class OverOverPatch : public Patch {
public:
  OverOverPatch(){
    registerParameter(PARAMETER_A, "Drive");
    registerParameter(PARAMETER_B, "Offset");
    registerParameter(PARAMETER_C, "OversampleNL");
    registerParameter(PARAMETER_D, "Gain");
    registerParameter(PARAMETER_E, "DrivePedal");
  }
  void processAudio(AudioBuffer &buffer){
    float expr = 1 - getParameterValue(PARAMETER_E);
    float drive = expr*getParameterValue(PARAMETER_A);     // get input drive value
    float offset = getParameterValue(PARAMETER_B); 	   // get offset value
    float gain = getParameterValue(PARAMETER_D);           // get output gain value
	bool  os= getParameterValue(PARAMETER_C)> 0.5;           // get output gain value
    offset /= 10.f;
    drive += 0.03f;
    drive *= 40.f;
    gain/= 2.f;
    int size = buffer.getSize();
      for (int ch = 0; ch<1; ++ch) {     //for each channel
          float* buf = buffer.getSamples(ch);
          for (int i = 0; i < size; ++i) {                  //process each sample
			  float input = (buf[i] + offset)*drive;
			  float output;
			  if (os)
				  output = oversample4(input);
			  else
			      output = nonLinear(input);
			  buf[i] = gain*output;
          }
      }
  }
  float washTable(float a){//WAVESHAPER look up table of length WAVESHAPER_N must be declared in included files.
	  if (a>0.9999){
		  return WAVESHAPER[WAVESHAPER_N - 1];/*a must be limited between MAX_SIGNAL and MIN_SIGNAL, otherwise it is clipped*/
	  }
	  if (a<-1){
		  return WAVESHAPER[0];
	  }
	  return WAVESHAPER[(int)((a + 1) / 2.0*WAVESHAPER_N + .5)]; //nearest neighbour approximation
  }

  float oversample4(float y){
	  static float y2[OS_L];
	  static int y2pp = 0;
	  static float y2p[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	  static float y4[OS_L];
	  static float y4p[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	  static int y4pp = 0;
	  static float y4xp[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
	  static int y4xpp = 0;
	  float y2x;
	  for (int m = 0; m<OS_L; m++) {
		  int idx = m;
		  if (m + 1 <= OS_uLength)
			  y2x = y*Bu[m];
		  else
			  y2x = 0;
		  y2[idx] = (Au[0] * y2p[y2pp] + Au[1] * y2p[(y2pp + 1) & 7] +
			  Au[2] * y2p[(y2pp + 2) & 7] + Au[3] * y2p[(y2pp + 3) & 7]) + y2x;
		  y2pp = (y2pp - 1 + 8) & 7;
		  y2p[y2pp] = y2[idx];
		  y4xp[y4xpp] = nonLinear(y2[idx]); //applies the waveshaper
		  y4xp[y4xpp] = y2[idx];
		  float y4x = Bd[0] * y4xp[y4xpp] + Bd[1] * y4xp[(y4xpp + 1) & 7] + Bd[2] * y4xp[(y4xpp + 2) & 7]
			  + Bd[3] * y4xp[(y4xpp + 3) & 7];
		  y4xpp = (y4xpp - 1 + 8) & 7;
		  y4[idx] = (Ad[0] * y4p[y4pp] + Ad[1] * y4p[(y4pp + 1) & 7] +
			  Ad[2] * y4p[(y4pp + 2) & 7] + Ad[3] * y4p[(y4pp + 3) & 7]) + y4x;
		  y4pp = (y4pp - 1 + 8) & 7;
		  y4p[y4pp] = y4[idx];
	  }
	  return(y2[0]);
  }

  float nonLinear(float x){ 		// Overdrive curve
    if (x<-3)
      return -1;
    else if (x>3)
      return 1;
    else
      return x * ( 27 + x*x ) / ( 27 + 9*x*x );
  }
};

#endif // __OverOverPatch_hpp__
