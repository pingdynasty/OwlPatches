#ifndef  __MdaStereoPatch_h__
#define __MdaStereoPatch_h__

#include "StompBox.h"

#include <math.h>
#include <float.h>


class MdaStereoPatch : public Patch
{
private:
	float fParam1;
	float fParam2;
	float fParam3;
	float fParam4;
	float fParam5;
	float fParam6;

	float fli, fld, fri, frd, fdel, phi, dphi, mod;
    int size, bufpos;
	float oldParam1, oldParam2, oldParam3, oldParam4, oldParam5;
	AudioBuffer *audiobuffer;
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

			fParam1 = (float)0.78; //Haas/Comb width
			fParam2 = (float)0.43; //delay
			fParam3 = (float)0.50; //balance
			fParam4 = (float)0.00; //mod
			fParam5 = (float)0.50; //rate

			size = 4800;
			bufpos = 0;
			audiobuffer = createMemoryBuffer(1, size);
			buffer = audiobuffer->getSamples(0);

			//calcs here!
			phi=0;
			dphi=(float)(3.141 * pow(10.0,-2.0 + 3.0 * fParam5) / getSampleRate());
			mod=(float)(2100.0 * pow(fParam4, 2));

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

	void setParameter()
		{
			fParam1 = getParameterValue(PARAMETER_A); // Comb Width
			fParam2 = getParameterValue(PARAMETER_B); // Delay
			fParam3 = getParameterValue(PARAMETER_D); // Balance
			fParam5 = getParameterValue(PARAMETER_C); // Rate
			fParam4 = getParameterValue(PARAMETER_E); // Mod

			// Don't recalc if nothing has changed
			if (oldParam1 == fParam1 &&
			    oldParam2 == fParam2 &&
			    oldParam3 == fParam3 &&
			    oldParam4 == fParam4 &&
			    oldParam5 == fParam5)
				return;

			//calcs here
			dphi=(float)(3.141 * pow(10.0,-2.0 + 3.0 * fParam5) / getSampleRate());
			mod=(float)(2100.0 * pow(fParam4, 2));

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

// process

	void processAudio(AudioBuffer &owlbuf)
		{
			float *in1;
			float *in2;
			float *out1;
			float *out2;
			float a, b, c, d;
			float li, ld, ri, rd, del, ph=phi, dph=dphi, mo=mod;
			int tmp, bp = bufpos;
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
            
            setParameter();

			li = fli;
			ld = fld;
			ri = fri;
			rd = frd;
			del = fdel;

			--in1;
			--in2;
			--out1;
			--out2;
			if(mo>0.f) //modulated delay
			{
				while(--sampleFrames >= 0)
				{
					a = *++in1 + *++in2; //sum to mono

					*(buffer + bp) = a; //write
					tmp = (bp + (int)(del + fabs(mo * sin(ph)) ) ) % 4410;
					b = *(buffer + tmp);

					c = (a * li) - (b * ld); // output
					d = (a * ri) - (b * rd);

					bp = (bp - 1); if(bp < 0) bp = 4410; //buffer position

					ph = ph + dph;

					*++out1 = c;
					*++out2 = d;
				}
			}
			else
			{
				while(--sampleFrames >= 0)
				{
					a = *++in1 + *++in2; //sum to mono

					*(buffer + bp) = a; //write
					tmp = (bp + (int)(del) ) % 4410;
					b = *(buffer + tmp);

					c = (a * li) - (b * ld); // output
					d = (a * ri) - (b * rd);

					bp = (bp - 1); if(bp < 0) bp = 4410; //buffer position

					*++out1 = c;
					*++out2 = d;
				}
			}
			bufpos = bp;
			phi = (float)fmod(ph,6.2831853f);
		}
};
#endif //__MdaSterePatch_h__
