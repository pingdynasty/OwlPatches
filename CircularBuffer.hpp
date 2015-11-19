#ifndef __CircularBuffer_h__
#define __CircularBuffer_h__

class CircularBuffer {
private:
  FloatArray buffer;
  unsigned int writeIndex;
public:
  CircularBuffer() : writeIndex(0) {
  }
  CircularBuffer(float* buf, int size) : buffer(buf, size), writeIndex(0) {
  }

  /** 
   * write to the tail of the circular buffer 
   */
  inline void write(float value){
    if(++writeIndex == buffer.getSize())
      writeIndex = 0;
    buffer[writeIndex] = value;
  }

  /**
   * read the value @param index steps back from the head of the circular buffer
   */
  inline float read(int index){
    return buffer[(writeIndex + (~index)) & (buffer.getSize()-1)];
  }

  /**
   * get the value at the head of the circular buffer
   */
  inline float head(){
    return buffer[(writeIndex - 1) & (buffer.getSize()-1)];
  }

  /** 
   * get the most recently written value 
   */
  inline float tail(){
    return buffer[(writeIndex) & (buffer.getSize()-1)];
  }

  /**
   * get the capacity of the circular buffer
   */
  inline unsigned int getSize(){
    return buffer.getSize();
  }

  /**
   * return a value interpolated to a floating point index
   */
  inline float interpolate(float index){
    int idx = (int)index;
    float low = read(idx);
    float high = read(idx+1);
    float frac = index - idx;
    return low*frac + high*(1-frac);
  }

  FloatArray getSamples(){
    return buffer;
  }

  static CircularBuffer* create(int samples){
    CircularBuffer* buf = new CircularBuffer();
    buf->buffer = FloatArray::create(samples);
    return buf;
  }

  static void destroy(CircularBuffer* buf){
    FloatArray::destroy(buf->buffer);
  }
};

#endif // __CircularBuffer_h__
