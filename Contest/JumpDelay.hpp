// Copyright 2013 Steffen Fuerst

// This program is free software: you can redistribute it and/or modify
// under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef __JumpDelay_hpp__
#define __JumpDelay_hpp__

//#include <stdlib.h>
#include "StompBox.h"
namespace JDelay {

class Buffer {
private:
  float* buffer;
  unsigned int bufferSize;
  unsigned int writeIndex;
  unsigned int readIndex1;
  unsigned int readIndex2;
  float readMix; // 0 (only readIndex1) - 1 (only readIndex2)
  float readMixInc;

  inline unsigned int inc(unsigned int index, int amount) const {
    unsigned int r = index + amount;
    return (r % bufferSize);
  }
  
public:
  const static int MINSIZE = 400;
  const static int MAXSIZE = 88200;

  Buffer() {
    buffer = NULL;
    setBufferSize(MINSIZE);
  }

  inline void setBufferSize(unsigned int size) {
    if (bufferSize == size)
      return;

    bufferSize = size;
    if (buffer != NULL) free(buffer);
    buffer = (float *) calloc(bufferSize, sizeof(float));
    writeIndex = 0;
    readIndex1 = 1;
    readIndex2 = 1;
    readMix = 0;
  }

  inline void setWindowSize(unsigned int size) {
    readMixInc = 1.0f / size;
    if (readMixInc < 0.01) readMixInc = 0.01f;
  }

  inline void write(float value) {
    buffer[writeIndex] = value;
  }

  inline float readRecordHead() const {
    return buffer[writeIndex];
  }

  inline float readPlayHead() const {
    return buffer[readIndex1] * (1.0f - readMix) + buffer[readIndex2] * readMix;
  }

  inline void nextSample() {
    writeIndex = inc(writeIndex, 1);
    readIndex1 = inc(readIndex1, 1);
    readIndex2 = inc(readIndex2, 1);
    if (readMix < .999999) readMix += readMixInc;
  }

  inline unsigned int getWritePos() const {
    return writeIndex;
  }

  inline void jump(int size) {
    readIndex1 = readIndex2;
    readIndex2 = inc(readIndex2, size);
    readMix = 0;
  }
};

class Parameters {
  private:
    inline unsigned int round(double x) const {
      return (unsigned int) (x + 0.5);	
    }
  public:
    Parameters(Patch* p) {
      pPatch = p;
      update();
    }
    inline void update() {
      double param_d = pPatch->getParameterValue(PARAMETER_D);
      forward = (param_d < 0.5) ? false : true;
      int e = (int) (((param_d < 0.5) ? 0.5 - param_d : param_d - 0.5) * 13.0);
      frag = 2 << e;

      double param_a = pPatch->getParameterValue(PARAMETER_A);
      bufferSize = round(param_a * 
                         (float)((Buffer::MAXSIZE - Buffer::MINSIZE)) + Buffer::MINSIZE);
      windowSize = bufferSize / frag;
      jumpSize = forward ? windowSize : -windowSize;

      double param_c = pPatch->getParameterValue(PARAMETER_C);
      prob = round(75 * param_c);

      double param_b = pPatch->getParameterValue(PARAMETER_B);
      feedback = (float) param_b;
    } 

    Patch* pPatch;
    unsigned int bufferSize;
    int windowSize;
    int jumpSize;
    unsigned int frag;
    bool forward;
    unsigned int prob;
    float feedback;
  };
} // end of namespace


class JumpDelay : public Patch {
private:
  inline void checkJump() const {
    unsigned int pos = pBuffer->getWritePos();
    if (pos % (pParams->windowSize + 1) == 0) {
		  unsigned int jumpSize = 0;
      while ((rand() % 100) < (int) pParams->prob) {
        jumpSize += pParams->jumpSize;
      }
      if (jumpSize > 0) pBuffer->jump(jumpSize);
    }
  }

public:
  JumpDelay() {
    pParams = new JDelay::Parameters(this);
    pBuffer = new JDelay::Buffer();
    registerParameter(PARAMETER_A, "Size");
    registerParameter(PARAMETER_B, "Feedback");
    registerParameter(PARAMETER_C, "Probability");
    registerParameter(PARAMETER_D, "Direction");

  }
  JDelay::Parameters* pParams;
  JDelay::Buffer* pBuffer;

  void processAudio(AudioBuffer &buffer) 
  {
    int size = buffer.getSize();

	for(int ch = 0; ch<buffer.getChannels(); ++ch)
	  { 
	    float* Buf = buffer.getSamples(ch);
	   
		
	    pParams->update();
	    pBuffer->setBufferSize(pParams->bufferSize);
	    pBuffer->setWindowSize(pParams->windowSize);

	    for(int i = 0; i < size; ++i) 
	    {
	      pBuffer->write(Buf[i] + pParams->feedback * pBuffer->readRecordHead());
	      Buf[i] = Buf[i] + pBuffer->readPlayHead();
				pBuffer->nextSample();
	      checkJump();
	    }
	  }
  }
};

#endif // __JumpDelay_hpp__
