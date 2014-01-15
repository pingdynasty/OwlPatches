#ifndef  __MdaBandistoPatch_h__
#define __MdaBandistoPatch_h__

#include "StompBox.h"

#include <float.h>
#include <math.h>

class MdaBandistoPatch : public Patch
{
public:
	MdaBandistoPatch() {
		registerParameter(PARAMETER_A, "Dist Low");
		registerParameter(PARAMETER_B, "Dist Mid");
		registerParameter(PARAMETER_C, "Dist High");
		registerParameter(PARAMETER_D, "Gain");
		fParam1 = (float)1.00; //Listen: L/M/H/out
		fParam2 = (float)0.40; //xover1 (about 224 Hz)
		fParam3 = (float)0.38; //xover2 (about 438 Hz)
		fParam4 = (float)0.80; //L drive    (1)
		fParam5 = (float)0.50; //M drive
		fParam6 = (float)0.10; //H drive
		fParam7 = (float)0.50; //L trim     (2)
		fParam8 = (float)0.50; //M trim
		fParam9 = (float)0.50; //H trim
		fParam10 = (float)0.4; //transistor/valve (default: transistor)

		//calcs here!
		driv1 = (float)pow(10.0,(6.0 * fParam4 * fParam4) - 1.0);
		driv2 = (float)pow(10.0,(6.0 * fParam5 * fParam5) - 1.0);
		driv3 = (float)pow(10.0,(6.0 * fParam6 * fParam6) - 1.0);

		valve = int(1.99 * fParam10);
		if(valve)
		{
			trim1 = (float)(0.5);
			trim2 = (float)(0.5);
			trim3 = (float)(0.5);
		}
		else
		{
			trim1 = 0.3f*(float)pow(10.0,(4.0 * pow(fParam4,3.f)));//(0.5 + 500.0 * pow(fParam4,6.0));
			trim2 = 0.3f*(float)pow(10.0,(4.0 * pow(fParam5,3.f)));
			trim3 = 0.3f*(float)pow(10.0,(4.0 * pow(fParam6,3.f)));
		}
		trim1 = (float)(trim1 * pow(10.0, 2.0 * fParam7 - 1.0));
		trim2 = (float)(trim2 * pow(10.0, 2.0 * fParam8 - 1.0));
		trim3 = (float)(trim3 * pow(10.0, 2.0 * fParam9 - 1.0));

		switch(int(fParam1*5.0))
		{
		case 0: trim2=0.0; trim3=0.0; slev=0.0; break;
		case 1: trim1=0.0; trim3=0.0; slev=0.0; break;
		case 2: trim1=0.0; trim2=0.0; slev=0.0; break;
		default: slev=0.5; break;
		}
		fi1 = (float)pow(10.0,fParam2 - 1.70); fo1=(float)(1.0 - fi1);
		fi2 = (float)pow(10.0,fParam3 - 1.05); fo2=(float)(1.0 - fi2);
        
        mainGain = 1.0;
	}
	~MdaBandistoPatch() {}

private:
	float fParam1, fParam2, fParam3, fParam4;
	float fParam5, fParam6, fParam7, fParam8;
	float fParam9, fParam10;
	float gain1, driv1, trim1;
	float gain2, driv2, trim2;
	float gain3, driv3, trim3;
	float fi1, fb1, fo1, fi2, fb2, fo2, fb3, slev;
	int valve;
	float oldParamA, oldParamB, oldParamC, oldParamD;
    float mainGain;


	void getParameters()
	{
		fParam4 = getParameterValue(PARAMETER_A);
		fParam5 = getParameterValue(PARAMETER_B);
		fParam6 = getParameterValue(PARAMETER_C);
		mainGain = getParameterValue(PARAMETER_D);

		// Don't recalc if nothing has changed
		if (oldParamA == fParam4 &&
		    oldParamB == fParam5 &&
		    oldParamC == fParam6 &&
		    oldParamD == mainGain)
			return;

		// Save for next block
		oldParamA = fParam4;
		oldParamB = fParam5;
		oldParamC = fParam6;
		oldParamD = mainGain;

		// Others we leave at default

		//calcs here
		driv1 = (float)pow(10.0,(6.0 * fParam4 * fParam4) - 1.0);
		driv2 = (float)pow(10.0,(6.0 * fParam5 * fParam5) - 1.0);
		driv3 = (float)pow(10.0,(6.0 * fParam6 * fParam6) - 1.0);

		valve = int(1.99 * fParam10);
		if(valve)
		{
			trim1 = (float)(0.5);
			trim2 = (float)(0.5);
			trim3 = (float)(0.5);
		}
		else
		{
			trim1 = 0.3f*(float)pow(10.0,(4.0 * pow(fParam4,3.f)));//(0.5 + 500.0 * pow(fParam4,6.0));
			trim2 = 0.3f*(float)pow(10.0,(4.0 * pow(fParam5,3.f)));
			trim3 = 0.3f*(float)pow(10.0,(4.0 * pow(fParam6,3.f)));
		}
		trim1 = (float)(trim1 * pow(10.0, 2.0 * fParam7 - 1.0));
		trim2 = (float)(trim2 * pow(10.0, 2.0 * fParam8 - 1.0));
		trim3 = (float)(trim3 * pow(10.0, 2.0 * fParam9 - 1.0));

		switch(int(fParam1*5.0))
		{
		    case 0: trim2=0.0; trim3=0.0; slev=0.0; break;
		    case 1: trim1=0.0; trim3=0.0; slev=0.0; break;
		    case 2: trim1=0.0; trim2=0.0; slev=0.0; break;
		    default: slev=0.5; break;
		}
		fi1 = (float)pow(10.0,fParam2 - 1.70); fo1=(float)(1.0 - fi1);
		fi2 = (float)pow(10.0,fParam3 - 1.05); fo2=(float)(1.0 - fi2);
	}
public:
	void processAudio(AudioBuffer &buffer) {
		float *in1;
		float *in2;
		float *out1;
		float *out2;
		float a, b, c, d, g, l=fb3, m, h, s, sl=slev;
		float f1i=fi1, f1o=fo1, f2i=fi2, f2o=fo2, b1=fb1, b2=fb2;
		float g1, d1=driv1, t1=trim1;
		float g2, d2=driv2, t2=trim2;
		float g3, d3=driv3, t3=trim3;
		int v=valve;
		int sampleFrames = buffer.getSize();

		if (buffer.getChannels() < 2) {       // Mono check
            in1 = buffer.getSamples(0); // L
            in2 = buffer.getSamples(0); // R
            out1 = buffer.getSamples(0); // L
            out2 = buffer.getSamples(0); // R
        } else {
            in1 = buffer.getSamples(0); // L
            in2 = buffer.getSamples(1); // R
            out1 = buffer.getSamples(0); // L
            out2 = buffer.getSamples(1); // R
        }
        
		getParameters();

		--in1;
		--in2;
		--out1;
		--out2;
		while(--sampleFrames >= 0)
		{
			a = *++in1;
			b = *++in2; //process from here...

			s = (a - b) * sl; //keep stereo component for later
			a += (float)(b + 0.00002); //dope filter at low level
			b2 = (f2i * a) + (f2o * b2); //crossovers
			b1 = (f1i * b2) + (f1o * b1);
			l = (f1i * b1) + (f1o * l);
			m=b2-l; h=a-b2;

			g = (l>0)? l : -l;
			g = (float)(1.0 / (1.0 + d1 * g) ); //distort
			g1=g;

			g = (m>0)? m : -m;
			g = (float)(1.0 / (1.0 + d2 * g) );
			g2=g;

			g = (h>0)? h : -h;
			g = (float)(1.0 / (1.0 + d3 * g) );
			g3=g;

			if(v) { if(l>0)g1=1.0; if(m>0)g2=1.0; if(h>0)g3=1.0; }

			a = (l*g1*t1) + (m*g2*t2) + (h*g3*t3);
			c = a + s; // output
			d = a - s;

			*++out1 = c * (mainGain * 2.0);
			*++out2 = d * (mainGain * 2.0);
		}
		fb1=b1; fb2=b2, fb3=l;
	}
};



#if 0 // CC Keeping this here for reference
void mdaBandisto::getParameterName(VstInt32 index, char *label)
{
	switch(index)
  {
    case 0: strcpy(label, "Listen:"); break;
    case 1: strcpy(label, "L <> M"); break;
    case 2: strcpy(label, "M <> H"); break;
    case 3: strcpy(label, "L Dist"); break;
    case 4: strcpy(label, "M Dist"); break;
    case 5: strcpy(label, "H Dist"); break;
    case 6: strcpy(label, "L Out"); break;
    case 7: strcpy(label, "M Out"); break;
    case 8: strcpy(label, "H Out"); break;
    case 9: strcpy(label, "Mode:"); break;
  }
}

void mdaBandisto::getParameterDisplay(VstInt32 index, char *text)
{
	switch(index)
  {
    case 0: switch(int(fParam1*5.0))
    { case 0: strcpy(text, "Low"); break;
      case 1: strcpy(text, "Mid"); break;
      case 2: strcpy(text, "High"); break;
     default: strcpy(text, "Output"); break; } break;
    case 1: sprintf(text, "%.0f", getSampleRate() * fi1 * (0.098 + 0.09*fi1 + 0.5*pow(fi1,8.2f))); break;
    case 2: sprintf(text, "%.0f", getSampleRate() * fi2 * (0.015 + 0.15*fi2 + 0.9*pow(fi2,8.2f))); break;
    case 3: sprintf(text, "%.0f", 60.0 * fParam4); break;
    case 4: sprintf(text, "%.0f", 60.0 * fParam5); break;
    case 5: sprintf(text, "%.0f", 60.0 * fParam6); break;
    case 6: sprintf(text, "%.0f", 40.0 * fParam7 - 20.0); break;
    case 7: sprintf(text, "%.0f", 40.0 * fParam8 - 20.0); break;
    case 8: sprintf(text, "%.0f", 40.0 * fParam9 - 20.0); break;
    case 9: if(fParam10>0.5) { strcpy(text, "Unipolar"); }
                        else { strcpy(text, "Bipolar"); } break;
  }
}
#endif // Reference section



#endif //__MdaBandistoPatch_h__
