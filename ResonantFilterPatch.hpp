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


#ifndef __ResonantFilterPatch_hpp__
#define __ResonantFilterPatch_hpp__

#include "SampleBasedPatch.hpp"

/**
Resonant filter

References : Posted by Paul Kellett

Notes : 
This filter consists of two first order low-pass filters in
series, with some of the difference between the two filter
outputs fed back to give a resonant peak.

You can use more filter stages for a steeper cutoff but the
stability criteria get more complicated if the extra stages
are within the feedback loop.

Code : 
//set feedback amount given f and q between 0 and 1
fb = q + q/(1.0 - f);

//for each sample...
buf0 = buf0 + f * (in - buf0 + fb * (buf0 - buf1));
buf1 = buf1 + f * (buf0 - buf1);
out = buf1;
*/

class ResonantFilterPatch : public SampleBasedPatch {
private:
  float buf0, buf1;
  float q, f, fb;
public:
  ResonantFilterPatch() :buf0(0), buf1(0) {
    registerParameter(PARAMETER_A, "Fc");
    registerParameter(PARAMETER_B, "Q");
    registerParameter(PARAMETER_C, "");
    registerParameter(PARAMETER_D, "");
  }
  void prepare(){
    f = getParameterValue(PARAMETER_A);
    q = getParameterValue(PARAMETER_B);
    fb = q + q/(1.0 - f);
  }
  float processSample(float sample){
    //for each sample...
    buf0 = buf0 + f * (sample - buf0 + fb * (buf0 - buf1));
    buf1 = buf1 + f * (buf0 - buf1);
    return buf1;
  }
};

#endif /* __ResonantFilterPatch_hpp__ */
