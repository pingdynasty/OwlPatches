#ifndef __StereoPatch_hpp__
#define __StereoPatch_hpp__

#include "StompBox.h"

/**
 * Runs a mono patch in stereo
 */
template<class PatchClass>
class StereoPatch : public Patch {
private:
  PatchClass leftPatch;
  PatchClass rightPatch;
  AudioBuffer* buf;
public:
  StereoPatch(){
    buf = AudioBuffer::create(2, getBlockSize());
  }
  void processAudio(AudioBuffer &buffer){
    FloatArray r1 = buffer.getSamples(RIGHT_CHANNEL);
    FloatArray l2 = buf->getSamples(LEFT_CHANNEL);
    l2.copyFrom(r1);
    leftPatch.processAudio(buffer);
    rightPatch.processAudio(*buf);
    r1.copyFrom(l2);
  }
};

#endif   // __StereoPatch_hpp__
