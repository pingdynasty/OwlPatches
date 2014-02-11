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

		context = ctx_tann_new(2, 2, getBlockSize(), getSampleRate());
	}
	
	~TannhauserPatch() {
		ctx_tann_free(context);
	}
	
	void processAudio(AudioBuffer &buffer) {
		float paramA = getParameterValue(PARAMETER_A);
		float paramB = getParameterValue(PARAMETER_B);
		float paramC = getParameterValue(PARAMETER_C);
		float paramD = getParameterValue(PARAMETER_D);
		
		PdMessage *msgA = PD_MESSAGE_ON_STACK(1);
		PdMessage *msgB = PD_MESSAGE_ON_STACK(1);
		PdMessage *msgC = PD_MESSAGE_ON_STACK(1);
		PdMessage *msgD = PD_MESSAGE_ON_STACK(1);
		
		msg_initWithFloat(msgA, 0.0, paramA);
		msg_initWithFloat(msgB, 0.0, paramB);
		msg_initWithFloat(msgC, 0.0, paramC);
		msg_initWithFloat(msgD, 0.0, paramD);

		TannBase *base = ctx_tann_getBase(context);
		ctx_scheduleMessageForReceiver(base, "Channel-A", msgA);
		ctx_scheduleMessageForReceiver(base, "Channel-B", msgB);
		ctx_scheduleMessageForReceiver(base, "Channel-C", msgC);
		ctx_scheduleMessageForReceiver(base, "Channel-D", msgD);
		
        int nbSples=getBlockSize()*buffer.getChannels();
        float inputCopy[nbSples];
        memcpy(inputCopy,buffer.getSamples(0),nbSples*sizeof(float));
        
		ctx_tann_process(context,
						 inputCopy,
						 buffer.getSamples(0),
						 getBlockSize());
	}
	
private:
	Tann_tann *context;
};

#endif // __TannhauserPatch_hpp__
