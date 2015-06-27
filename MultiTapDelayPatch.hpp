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

/* created by Vincent Decorges 22.02.2015 */

////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef __MultiTapDelayPatch_hpp__
#define __MultiTapDelayPatch_hpp__

#include "SampleBasedPatch.hpp"
#include "StompBox.h"

#define MAX_DELAY_MS 2000
#define NBR_HEADS 4

class MultiTapDelayPatch : public SampleBasedPatch {
private:
  float* buffer;

  
  float currentDelay;
  float currentSelection;
  
  float delayInSamples;
  
  int readIndex[NBR_HEADS];
  int writeIndex;
  int bufferSize;
  
  bool activeHeads[NBR_HEADS];
  int nbrActiveHead;
  
  inline float dLinTerp(float x1, float x2, float y1, float y2, float x)
  {
    float denom = x2 - x1;
    if(denom == 0)
      return y1; // should not ever happen

    // calculate decimal position of x
    float dx = (x - x1)/(x2 - x1);

    // use weighted sum method of interpolating
    float result = dx*y2 + (1-dx)*y1;

    return result;

  }  
public:
  MultiTapDelayPatch() {
    registerParameter(PARAMETER_A, "Delay");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "Dry/Wet");
    registerParameter(PARAMETER_D, "Tap selector");
	
	
    AudioBuffer* audioBuffer = createMemoryBuffer(1, (int) (MAX_DELAY_MS / 1000) * getSampleRate());
    buffer = audioBuffer->getSamples(0);
    memset(buffer, 0, audioBuffer->getSize() *sizeof(float));

    bufferSize = audioBuffer->getSize();
	
    writeIndex = 0;
    currentDelay = 0;
    currentSelection = 0;
    delayInSamples = 0;

    computeVariables();
  }
  
  
  void computeVariables() {
  
    currentDelay = getParameterValue(PARAMETER_A);
    delayInSamples = (MAX_DELAY_MS * currentDelay) * (getSampleRate() / 1000.0);
	
    for (int i = 0; i < NBR_HEADS; i++) {
      float divNote = pow(2.0, (NBR_HEADS-(i+1)));
      readIndex[i] = writeIndex - (int) (delayInSamples / divNote);
		
      if (readIndex[i] < 0) {
	readIndex[i] += bufferSize;
      }
    }
	
    currentSelection = getParameterValue(PARAMETER_D);

    nbrActiveHead = 0;
    for (int i=0; i < NBR_HEADS; i++) {
      activeHeads[i] = isHeadActive(i);
		
      if (activeHeads[i]) nbrActiveHead++;
    }
  
  }
  
  bool isHeadActive(int head) {

    // POS 1
    if (currentSelection >= 0 && currentSelection < 0.083) {
      return head == 3;
    }
    // POS 2
    else if (currentSelection >= 0.083 && currentSelection < 0.166) {
      return head == 0 || head == 1;
    }
    // POS 3
    else if (currentSelection >= 0.166 && currentSelection < 0.25) {
      return head == 1 || head == 2;
    }
    // POS 4
    else if (currentSelection >= 0.25 && currentSelection < 0.333) {
      return head == 2 || head == 3;
    }
    // POS 5
    else if (currentSelection >= 0.333 && currentSelection < 0.416) {
      return head == 0 || head == 2;
    }
    // POS 6
    else if (currentSelection >= 0.416 && currentSelection < 0.5) {
      return head == 1 || head == 3;
    }
    // POS 7
    else if (currentSelection >= 0.5 && currentSelection < 0.583) {
      return head == 0 || head == 3;
    }
    // POS 8
    else if (currentSelection >= 0.583 && currentSelection < 0.666) {
      return head == 0 || head == 1 || head == 2;
    }
    // POS 9
    else if (currentSelection >= 0.666 && currentSelection < 0.75) {
      return head == 1 || head == 2 || head == 3;
    }
    // POS 10
    else if (currentSelection >= 0.75 && currentSelection < 0.833) {
      return head == 0 || head == 1 || head == 3;
    }
    // POS 11
    else if (currentSelection >= 0.833 && currentSelection < 0.916) {
      return head == 0 || head == 2 || head == 3;
    }
    // POS 11
    else {
      return true;
    }

  } 

  void prepare() {
    if (currentDelay != getParameterValue(PARAMETER_A) ||
	currentSelection != getParameterValue(PARAMETER_D)) {
      computeVariables();
    }
  }
  
  float processSample(float sample)
  {
  	
    float xn = sample;
	
    // head output
    float ynHeads[NBR_HEADS];
	
    // delay output
    float yn=0.0;
	
    for (int i=0; i < NBR_HEADS; i++) {
      ynHeads[i] = buffer[readIndex[i]];
		
      if (readIndex[i] == writeIndex && delayInSamples < 1.00) {
	ynHeads[i] = xn;
      }
		
      if (activeHeads[i]) {
		
	int readIndex_1 = readIndex[i] - 1;
			
	if (readIndex_1 < 0) {
	  readIndex_1 = bufferSize - 1;
	}
			
	float yn_1 = buffer[readIndex_1];
	float fracDelay = delayInSamples - (int) delayInSamples;
			
	ynHeads[i] = dLinTerp(0, 1, ynHeads[i], yn_1, fracDelay);
		
      }
      else {
	ynHeads[i] = 0;
      }
		
      readIndex[i]++;
      if (readIndex[i] >= bufferSize) {
	readIndex[i] = 0;
      }
		
      if (activeHeads[i]) {
	yn = yn + (ynHeads[i] * (1.0 / nbrActiveHead));
			
	// hard limiting
	if (yn > 1.0) yn = 1.0;
	else if (yn < -1.0) yn = -1.0;
      }
		
    }

    // no delay!
    if (delayInSamples == 0) yn = xn;

    buffer[writeIndex] = xn + getParameterValue(PARAMETER_B) * yn;

    // increment for next sample
    writeIndex++;
    if (writeIndex >= bufferSize) {
      writeIndex = 0;
    }
	
    return getParameterValue(PARAMETER_C) * yn + (1.0 - getParameterValue(PARAMETER_C)) * xn;
  }
};

#endif   // __MultiTapDelayPatch_hpp__
