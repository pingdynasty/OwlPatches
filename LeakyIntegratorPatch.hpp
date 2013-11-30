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


#ifndef __LeakyIntegratorPatch_hpp__
#define __LeakyIntegratorPatch_hpp__

#include "SampleBasedPatch.hpp"

class LeakyIntegratorPatch : public SampleBasedPatch {
private:
// leaky integrator
// y[n] = (1-lambda)*x[n] + lambda*y[n-1]
// dc removal
// y[n] = lambda*y[n-1] + x[n] - x[n-1]
  float lambda, gain;
  float y1;
public:
  LeakyIntegratorPatch() : y1(0.0) {
    registerParameter(PARAMETER_A, "Lambda");    
    registerParameter(PARAMETER_A, "Gain");    
  }
  void prepare(){
    lambda = getParameterValue(PARAMETER_A);
    gain = getParameterValue(PARAMETER_D);
  }
  float processSample(float sample){
    y1 = lambda*y1 + (1-lambda)*sample;
    return gain*y1;
  }
};

#endif /* __LeakyIntegratorPatch_hpp__ */



////////////////////////////////////////////////////////////////////////////////////////////////////

