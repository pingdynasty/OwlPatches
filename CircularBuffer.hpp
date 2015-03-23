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


#ifndef __CircularBuffer_h__
#define __CircularBuffer_h__

#include <string.h> /* for memset */

class CircularBuffer {
private:
  float* buffer;
  unsigned int size;
  unsigned int writeIndex;
public:
//   CircularBuffer(float* buf, int sz) : buffer(buf), size(sz), writeIndex(0) {
  CircularBuffer() : buffer(NULL), size(0), writeIndex(0) {
  }
  void initialise(float* buf, unsigned int sz){
    buffer = buf;
    size = sz;
    memset(buffer, 0, size*sizeof(float));
  }
  inline void write(float value){
    if(++writeIndex == size)
      writeIndex = 0;
    buffer[writeIndex] = value;
  }  
  inline float read(int index){
    return buffer[(writeIndex + (~index)) & (size-1)];
  }
  inline float readInterp(float index){
      // Perform Linear Interpolation
      int p = floor(index);
      float sw = index-p;
      float s1 = buffer[(writeIndex + (~p)) & (size-1)];
      float s2 = buffer[(writeIndex + (~(p+1))) & (size-1)];
    return (s1*(1-sw) + s2*sw);
  }
  inline float head(){
    return buffer[(writeIndex - 1) & (size-1)];
  }
  inline float tail(){
    return buffer[(writeIndex) & (size-1)];
  }
  inline unsigned int getSize(){
    return size;
  }
};

#endif // __CircularBuffer_h__
