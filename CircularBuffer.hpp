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

// template<class T, unsigned int size, T* buffer>
// class CircularBuffer {
// private:
// public:
//   CircularBuffer():writeIndex(0) {
//     memset(buffer, 0, size*sizeof(T));
//   }
//   inline void write(T value){
//     if(++writeIndex > size)
//       writeIndex = 0;
//     buffer[writeIndex] = value;
//   }
// //   inline T head(){
// //     return buffer[(writeIndex - 1) & (size-1)];
// //   }
// //   inline T tail(){
// //     return buffer[(writeIndex) & (size-1)];
// //   }
//   /* get the value at @index positions previous to the last write */
//   inline T read(int index){
// //     index = writeIndex - index;
// //     if(index < 0)
// //       index += size;
// //     return buffer[index];
//     return buffer[(writeIndex + (~index)) & (size-1)];
//   }
//   inline unsigned int getSize(){
//     return size;
//   }
// };

#endif // __CircularBuffer_h__
