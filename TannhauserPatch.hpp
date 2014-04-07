#ifndef __TannhauserPatch_hpp__
#define __TannhauserPatch_hpp__

#include "StompBox.h"
extern "C" {
#include "Tannhauser/Tannhauser_tann.h"
}

class TannhauserPatch : public Patch {
public:
	TannhauserPatch() {
		registerParameter(PARAMETER_A, "Channel-A");
		registerParameter(PARAMETER_B, "Channel-B");
		registerParameter(PARAMETER_C, "Channel-C");
		registerParameter(PARAMETER_D, "Channel-D");

		context = th_tann_new(2, 2, getBlockSize(), getSampleRate());
	}
	
	~TannhauserPatch() {
		th_tann_free(context);
	}
	
	void processAudio(AudioBuffer &buffer) {
		float paramA = getParameterValue(PARAMETER_A);
		float paramB = getParameterValue(PARAMETER_B);
		float paramC = getParameterValue(PARAMETER_C);
		float paramD = getParameterValue(PARAMETER_D);
		
		th_vscheduleMessageForReceiver(context, "Channel-A", 0.0, "f", paramA);
        th_vscheduleMessageForReceiver(context, "Channel-B", 0.0, "f", paramB);
        th_vscheduleMessageForReceiver(context, "Channel-C", 0.0, "f", paramC);
        th_vscheduleMessageForReceiver(context, "Channel-D", 0.0, "f", paramD);
        /*
         The 0.0 parameter is the timestamp at which to execute the message, but in this case it simply means to execute it immediately. "f" says that the message contains one element and its type is float. paramA is then the value. 
         */
		
        int nbSples=getBlockSize()*buffer.getChannels();
        float inputCopy[nbSples];
        memcpy(inputCopy,buffer.getSamples(0),nbSples*sizeof(float));
        
		th_tann_process(context, inputCopy, buffer.getSamples(0));
	}
	
private:
	Tann_tann *context;
};

#endif // __TannhauserPatch_hpp__
