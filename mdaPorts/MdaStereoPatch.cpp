#ifndef  __MdaStereoPatch_h__
#define __MdaStereoPatch_h__

// mdaStereo from the mda plugins collection
// Ported to OWL by Chrissie Caulfield, 2014


#include "StompBox.h"

class MdaStereoPatch : public Patch
{
private:
    float fli, fld, fri, frd, fdel, phi, dphi, mod;
    int bufpos;
    float *buffer;

public:
    MdaStereoPatch()
    {
	//inits here!
	registerParameter(PARAMETER_A, "Comb Width");
	registerParameter(PARAMETER_B, "Delay");
	registerParameter(PARAMETER_C, "Rate");
	registerParameter(PARAMETER_D, "Balance");
	registerParameter(PARAMETER_E, "Mod");

	bufpos = 0;
	AudioBuffer *audiobuffer = createMemoryBuffer(1, 4800);
	buffer = audiobuffer->getSamples(0);
    }

    void setParameter()
    {
        float fParam1;
	float fParam2;
	float fParam3;
	float fParam4;
	float fParam5;
	float fParam6;
	fParam1 = getParameterValue(PARAMETER_A); // Comb Width
	fParam2 = getParameterValue(PARAMETER_B); // Delay
	fParam3 = getParameterValue(PARAMETER_D); // Balance
	fParam5 = getParameterValue(PARAMETER_C); // Rate
	fParam4 = getParameterValue(PARAMETER_E); // Mod

	//calcs here
	dphi=(float)(3.141 * powf(10.0,-2.0 + 3.0 * fParam5) / getSampleRate());
	mod=(float)(2100.0 * powf(fParam4, 2));

	if(fParam1<0.5)
	{
	    fli = (float)(0.25 + (1.5 * fParam1));
	    fld = 0.0;
	    fri = (float)(2.0 * fParam1);
	    frd = (float)(1.0 - fri);
	}
	else
	{
	    fli = (float)(1.5 - fParam1);
	    fld = (float)(fParam1 - 0.5);
	    fri = fli;
	    frd = -fld;
	}
	fdel = (float)(20.0 + 2080.0 * pow(fParam2, 2));
	if(fParam3>0.5)
	{
	    fli *= (float)((1.0 - fParam3) * 2.0);
	    fld *= (float)((1.0 - fParam3) * 2.0);
	}
	else
	{
	    fri *= (2 * fParam3);
	    frd *= (2 * fParam3);
	}
	fri *= (float)(0.5 + fabs(fParam1 - 0.5));
	frd *= (float)(0.5 + fabs(fParam1 - 0.5));
	fli *= (float)(0.5 + fabs(fParam1 - 0.5));
	fld *= (float)(0.5 + fabs(fParam1 - 0.5));
    }

    ~MdaStereoPatch()
    {
	// Should free the buffer here ... if it ever gets called!
    }

    void processAudio(AudioBuffer &owlbuf)
    {
	setParameter();
	float *in1;
	float *in2;
	float *out1;
	float *out2;
	float a, b, c, d;
	int tmp;
	int sampleFrames = owlbuf.getSize();

	in1 = owlbuf.getSamples(0); // L
	in2 = owlbuf.getSamples(1); // R
	out1 = owlbuf.getSamples(0); // L
	out2 = owlbuf.getSamples(1); // R
	setParameter();

	if(mod>0.f) //modulated delay
	{
	    while(sampleFrames-- > 0)
	    {
		a = *in1++ + *in2++; //sum to mono

		*(buffer + bufpos) = a; //write
		tmp = (bufpos + (int)(fdel + fabs(mod * sin(phi)) ) ) % 4410;
		b = *(buffer + tmp);

		c = (a * fli) - (b * fld); // output
		d = (a * fri) - (b * frd);

		bufpos = (bufpos - 1); if(bufpos < 0) bufpos = 4410; //buffer position

		phi = phi + dphi;

		*out1++ = c;
		*out2++ = d;
	    }
	}
	else
	{
	    while(sampleFrames-- > 0)
	    {
		a = *in1++ + *in2++; //sum to mono

		*(buffer + bufpos) = a; //write
		tmp = (bufpos + (int)(fdel) ) % 4410;
		b = *(buffer + tmp);

		c = (a * fli) - (b * fld); // output
		d = (a * fri) - (b * frd);

		bufpos = (bufpos - 1); if(bufpos < 0) bufpos = 4410; //buffer position

		*out1++ = c;
		*out2++ = d;
	    }
	}
	phi = (float)fmod(phi,6.2831853f);
    }
};
#endif //__MdaStereoPatch_h__
