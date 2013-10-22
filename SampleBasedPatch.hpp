#ifndef __SampleBasedPatch_hpp__
#define __SampleBasedPatch_hpp__

class SampleBasedPatch : public Patch {
public:
  virtual void prepare() = 0;
  virtual float processSample(float sample) = 0;
  void processAudio(AudioBuffer &buffer){
    prepare();
    int size = buffer.getSize();
    for(int ch=0; ch<buffer.getChannels(); ++ch){
      float* samples = buffer.getSamples(ch);
      for(int i=0; i<size; ++i)
	samples[i] = processSample(samples[i]);
    }
  }
};

#endif /* __SampleBasedPatch_hpp__ */

