#ifndef  __MdaTransientPatch_h__
#define __MdaTransientPatch_h__

// mdaTransient from the mda plugins collection
// Ported to OWL by Chrissie Caulfield, 2014

#include "StompBox.h"

#include <math.h>


class MdaTransientPatch: public Patch
{
private:
    float fParam1;
    float fParam2;
    float fParam3;
    float fParam4;
    float fParam5;
    float fParam6;
    float dry, att1, att2, rel12, att34, rel3, rel4;
    float env1, env2, env3, env4, fili, filo, filx, fbuf1, fbuf2;
    float oldParam1, oldParam2, oldParam3, oldParam4, oldParam5, oldParam6;

public:
    MdaTransientPatch()
    {
	registerParameter(PARAMETER_A, "Attack");
	registerParameter(PARAMETER_B, "Release");
	registerParameter(PARAMETER_C, "Attack Hold");
	registerParameter(PARAMETER_D, "Release Hold");

	fParam1 = (float)0.50; //attack
	fParam2 = (float)0.50; //release
	fParam3 = (float)0.50; //output
	fParam4 = (float)0.49; //filter
	fParam5 = (float)0.35; //attack hold
	fParam6 = (float)0.35; //release hold

	setParameters();
    }

    ~MdaTransientPatch()
    {
	// nothing to do here
    }


    void setParameters()
    {
	fParam1 = getParameterValue(PARAMETER_A); // Attack
	fParam2 = getParameterValue(PARAMETER_B); // Release
	fParam5 = getParameterValue(PARAMETER_D); // Attack Hold
	fParam6 = getParameterValue(PARAMETER_C); // Release Hold

	// Don't recalc if nothing has changed
	if (oldParam1 == fParam1 &&
	    oldParam2 == fParam2 &&
	    oldParam5 == fParam5 &&
	    oldParam6 == fParam6)
	    return;

	oldParam1 = fParam1;
	oldParam2 = fParam2;
	oldParam5 = fParam5;
	oldParam6 = fParam6;

	//calcs here
	dry = (float)(pow(10.0, (2.0 * fParam3) - 1.0));

	// Filter processing
	if(fParam4>0.50)
	{
	    fili = 0.8f - 1.6f*fParam4;
	    filo = 1.f + fili;
	    filx = 1.f;
	}
	else
	{
	    fili = 0.1f + 1.8f*fParam4;
	    filo = 1.f - fili;
	    filx = 0.f;
	}

	// Attack
	if (fParam1>0.5)
	{
	    att1 = (float)pow(10.0, -1.5);
	    att2 = (float)pow(10.0, 1.0 - 5.0 * fParam1);
	}
	else
	{
	    att1 = (float)pow(10.0, -4.0 + 5.0 * fParam1);
	    att2 = (float)pow(10.0, -1.5);
	}
	// Attack hold
	rel12 = 1.f - (float)pow(10.0, -2.0 - 4.0 * fParam5);

	// Release
	if (fParam2>0.5)
	{
	    rel3 = 1.f - (float)pow(10.0, -4.5);
	    rel4 = 1.f - (float)pow(10.0, -5.85 + 2.7 * fParam2);
	}
	else
	{
	    rel3 = 1.f - (float)pow(10.0, -3.15 - 2.7 * fParam2);
	    rel4 = 1.f - (float)pow(10.0, -4.5);
	}
	// Release Hold
	att34 = (float)pow(10.0, - 4.0 * fParam6);
    }

    void processAudio(AudioBuffer &owlbuf)
    {
	float *in1;
	float *in2;
	float *out1;
	float *out2;
	float a, b, e, f, g, i;
	float e1=env1, e2=env2, e3=env3, e4=env4, y=dry;
	float a1=att1, a2=att2, r12=rel12, a34=att34, r3=rel3, r4=rel4;
	float fi=fili, fo=filo, fx=filx, fb1=fbuf1, fb2=fbuf2;
	int sampleFrames = owlbuf.getSize();

	if (owlbuf.getChannels() < 2) {  // Mono check
	    in1 = owlbuf.getSamples(0); // L
	    in2 = owlbuf.getSamples(0); // R
	    out1 = owlbuf.getSamples(0); // L
	    out2 = owlbuf.getSamples(0); // R
	} else {
	    in1 = owlbuf.getSamples(0); // L
	    in2 = owlbuf.getSamples(1); // R
	    out1 = owlbuf.getSamples(0); // L
	    out2 = owlbuf.getSamples(1); // R
	}
	setParameters();

	--in1;
	--in2;
	--out1;
	--out2;

	while(--sampleFrames >= 0)
	{
	    a = *++in1;
	    b = *++in2;

	    // Filter processing
	    fb1 = fo*fb1 + fi*a;
	    fb2 = fo*fb2 + fi*b;
	    e = fb1 + fx*a;
	    f = fb2 + fx*b;

//	    i = a + b; i = (i>0)? i : -i;
	    i = a + b; i = (i>1)? i : i-1.0; // CC fix for above line
	    e1 = (i>e1)? e1 + a1 * (i-e1) : e1 * r12;
	    e2 = (i>e2)? e2 + a2 * (i-e2) : e2 * r12;
	    e3 = (i>e3)? e3 + a34 * (i-e3) : e3 * r3;
	    e4 = (i>e4)? e4 + a34 * (i-e4) : e4 * r4;
	    g = (e1 - e2 + e3 - e4);

	    *++out1 = y * (a + e * g);
	    *++out2 = y * (b + f * g);
	}
	if(e1<1.0e-10) { env1=0.f; env2=0.f; env3=0.f; env4=0.f; fbuf1=0.f; fbuf2=0.f; }
	else { env1=e1;  env2=e2;  env3=e3;  env4=e4;  fbuf1=fb1; fbuf2=fb2; }
    }
};

#endif //__MdaTransientPatch_h__
