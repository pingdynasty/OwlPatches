//-----------------------------------------------------
//
// Code generated with Faust 2.0.a23 (http://faust.grame.fr)
//-----------------------------------------------------

#ifndef  __qompander_H__
#define  __qompander_H__
/************************************************************************

	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2014 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work
	that contains this FAUST architecture section and distribute
	that work under terms of your choice, so long as this FAUST
	architecture section is not modified.


 ************************************************************************
 ************************************************************************/

#ifndef __qompanderPatch_h__
#define __qompanderPatch_h__

#include "StompBox.h"
#include "owlcontrol.h"
#include "ApplicationSettings.h"
#include "CodecController.h"
#include "PatchProcessor.h"
#include "PatchController.h"
#include "device.h"
#include <cstddef>
#include <string.h>
#include <strings.h>


#ifndef __FaustCommonInfrastructure__
#define __FaustCommonInfrastructure__


/************************************************************************
	IMPORTANT NOTE : this file contains two clearly delimited sections :
	the ARCHITECTURE section (in two parts) and the USER section. Each section
	is governed by its own copyright and license. Please check individually
	each section for license and copyright information.
*************************************************************************/

/*******************BEGIN ARCHITECTURE SECTION (part 1/2)****************/

/************************************************************************
    FAUST Architecture File
	Copyright (C) 2003-2011 GRAME, Centre National de Creation Musicale
    ---------------------------------------------------------------------
    This Architecture section is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 3 of
	the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
	along with this program; If not, see <http://www.gnu.org/licenses/>.

	EXCEPTION : As a special exception, you may create a larger work
	that contains this FAUST architecture section and distribute
	that work under terms of your choice, so long as this FAUST
	architecture section is not modified.


 ************************************************************************
 ************************************************************************/
 
/******************************************************************************
*******************************************************************************

								FAUST DSP

*******************************************************************************
*******************************************************************************/

#ifndef __dsp__
#define __dsp__

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

class UI;

//----------------------------------------------------------------
//  signal processor definition
//----------------------------------------------------------------

class dsp {

 public:
	dsp() {}
	virtual ~dsp() {}

	virtual int getNumInputs() 										= 0;
	virtual int getNumOutputs() 									= 0;
	virtual void buildUserInterface(UI* ui_interface) 				= 0;
	virtual void init(int samplingRate) 							= 0;
 	virtual void compute(int len, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) 	= 0;
};

// On Intel set FZ (Flush to Zero) and DAZ (Denormals Are Zero)
// flags to avoid costly denormals
#ifdef __SSE__
    #include <xmmintrin.h>
    #ifdef __SSE2__
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8040)
    #else
        #define AVOIDDENORMALS _mm_setcsr(_mm_getcsr() | 0x8000)
    #endif
#else
    #define AVOIDDENORMALS
#endif

#endif
#ifndef FAUST_UI_H
#define FAUST_UI_H

#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif

/*******************************************************************************
 * UI : Faust User Interface
 * This abstract class contains only the method that the faust compiler can
 * generate to describe a DSP interface.
 ******************************************************************************/

class UI
{

 public:

	UI() {}

	virtual ~UI() {}

    // -- widget's layouts

    virtual void openTabBox(const char* label) = 0;
    virtual void openHorizontalBox(const char* label) = 0;
    virtual void openVerticalBox(const char* label) = 0;
    virtual void closeBox() = 0;

    // -- active widgets

    virtual void addButton(const char* label, FAUSTFLOAT* zone) = 0;
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) = 0;
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT min, FAUSTFLOAT max, FAUSTFLOAT step) = 0;

    // -- passive widgets

    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;
    virtual void addVerticalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT min, FAUSTFLOAT max) = 0;

	// -- metadata declarations

    virtual void declare(FAUSTFLOAT* zone, const char* key, const char* val) {}
};

#endif



struct Meta
{
    virtual void declare(const char* key, const char* value) = 0;
};



/**************************************************************************************

	OwlWidget : object used by OwlUI to ensures the connection between an owl parameter 
	and a faust widget
	
***************************************************************************************/

class OwlWidget
{
  protected:
	PatchProcessor* 	fProcessor;		// needed to register and read owl parameters
	PatchParameterId	fParameter;		// OWL parameter code : PARAMETER_A,...
	FAUSTFLOAT* 		fZone;			// Faust widget zone
	const char*			fLabel;			// Faust widget label 
	float				fMin;			// Faust widget minimal value
	float				fSpan;			// Faust widget value span (max-min)
	
  public:
	OwlWidget() :
		fProcessor(0), fParameter(PARAMETER_A), fZone(0), fLabel(""), fMin(0), fSpan(1) {}
	OwlWidget(const OwlWidget& w) :
		fProcessor(w.fProcessor), fParameter(w.fParameter), fZone(w.fZone), fLabel(w.fLabel), fMin(w.fMin), fSpan(w.fSpan) {}
	OwlWidget(PatchProcessor* pp, PatchParameterId param, FAUSTFLOAT* z, const char* l, float lo, float hi) :
		fProcessor(pp), fParameter(param), fZone(z), fLabel(l), fMin(lo), fSpan(hi-lo) {}
	void bind() 	{ fProcessor->registerParameter(fParameter, fLabel); }
	void update()	{ *fZone = fMin + fSpan*fProcessor->getParameterValue(fParameter); }
	
};


/**************************************************************************************

	OwlUI : Faust User Interface builder. Passed to buildUserInterface OwlUI ensures
	the mapping between owl parameters and faust widgets. It relies on specific
	metadata "...[OWL:PARAMETER_X]..." in widget's label for that. For example any 
	faust widget with metadata [OWL:PARAMETER_B] will be controlled by PARAMETER_B 
	(the second knob).
	
***************************************************************************************/

// The maximun number of mappings between owl parameters and faust widgets 
#define MAXOWLWIDGETS 64

class OwlUI : public UI
{
	PatchProcessor* 	fProcessor;
	PatchParameterId	fParameter;					// current parameter ID, value PARAMETER_F means not set
	int					fIndex;						// number of OwlWidgets collected so far
	OwlWidget			fTable[MAXOWLWIDGETS];		// kind of static list of OwlWidgets
	
	// check if the widget is an Owl parameter and, if so, add the corresponding OwlWidget
	void addOwlWidget(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) {
		if ((fParameter >= PARAMETER_A) && (fParameter <= PARAMETER_E) && (fIndex < MAXOWLWIDGETS)) {
			fTable[fIndex] = OwlWidget(fProcessor, fParameter, zone, label, lo, hi);
			fTable[fIndex].bind();
			fIndex++;
		}
		fParameter = PARAMETER_F; 		// clear current parameter ID
	}

	// we dont want to create a widget by-ut we clear the current parameter ID just in case
	void skip() {
		fParameter = PARAMETER_F; 		// clear current parameter ID
	}

 public:

	OwlUI(PatchProcessor* pp) : fProcessor(pp), fParameter(PARAMETER_F), fIndex(0) {}
	
	virtual ~OwlUI() {}
	
	// should be called before compute() to update widget's zones registered as Owl parameters
	void update() {
		for (int i=0; i<fIndex; i++)  fTable[i].update();
	}

	//---------------------- virtual methods called by buildUserInterface ----------------
	
    // -- widget's layouts

    virtual void openTabBox(const char* label) {}
    virtual void openHorizontalBox(const char* label) {}
    virtual void openVerticalBox(const char* label) {}
    virtual void closeBox() {}

    // -- active widgets

    virtual void addButton(const char* label, FAUSTFLOAT* zone) 																			{ skip(); }
    virtual void addCheckButton(const char* label, FAUSTFLOAT* zone) 																		{ skip(); }
    virtual void addVerticalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) 	{ addOwlWidget(label, zone, lo, hi); }
    virtual void addHorizontalSlider(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) 	{ addOwlWidget(label, zone, lo, hi); }
    virtual void addNumEntry(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT init, FAUSTFLOAT lo, FAUSTFLOAT hi, FAUSTFLOAT step) 			{ addOwlWidget(label, zone, lo, hi); }

    // -- passive widgets

    virtual void addHorizontalBargraph(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) 									{ skip(); }
    virtual void addVerticalBargraph  (const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) 									{ skip(); }

	// -- metadata declarations

    virtual void declare(FAUSTFLOAT* z, const char* k, const char* id) {
    	if (strcasecmp(k,"OWL") == 0) {
    		     if (strcasecmp(id,"PARAMETER_A") == 0)  fParameter = PARAMETER_A;
    		else if (strcasecmp(id,"PARAMETER_B") == 0)  fParameter = PARAMETER_B;
    		else if (strcasecmp(id,"PARAMETER_C") == 0)  fParameter = PARAMETER_C;
    		else if (strcasecmp(id,"PARAMETER_D") == 0)  fParameter = PARAMETER_D;
    		else if (strcasecmp(id,"PARAMETER_E") == 0)  fParameter = PARAMETER_E;
    	}
    }
};

#endif // __FaustCommonInfrastructure__

/**************************BEGIN USER SECTION **************************/


#ifndef FAUSTFLOAT
#define FAUSTFLOAT float
#endif  

#include <math.h>

float expf(float dummy0);
float faustpower2_f(float value) {
	return (value * value);
	
}
float sqrtf(float dummy0);
float fabsf(float dummy0);
float sinf(float dummy0);
float powf(float dummy0, float dummy1);
float logf(float dummy0);

#ifndef FAUSTCLASS 
#define FAUSTCLASS qompander
#endif

class qompander : public dsp {
	
  private:
	
	float fRec7[3];
	float fRec6[3];
	float fRec5[3];
	float fRec4[3];
	float fRec11[3];
	float fRec10[3];
	float fRec9[3];
	float fRec8[3];
	float fVec0[2];
	float fRec0[2];
	float fRec2[2];
	float fRec12[2];
	float fRec3[2];
	float fRec1[2];
	float fRec13[2];
	FAUSTFLOAT fHslider0;
	int fSamplingFreq;
	float fConst0;
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fHslider3;
	
  public:
	
	void static metadata(Meta* m) { 
		m->declare("compilation_options", "-single -scal");
	}

	virtual int getNumInputs() {
		return 1;
		
	}
	virtual int getNumOutputs() {
		return 1;
		
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	virtual int getOutputRate(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 1;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	
	static void classInit(int samplingFreq) {
		
	}
	
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		for (int i = 0; (i < 2); i = (i + 1)) {
			fVec0[i] = 0.f;
			
		}
		fHslider0 = FAUSTFLOAT(3.);
		for (int i = 0; (i < 2); i = (i + 1)) {
			fRec0[i] = 0.f;
			
		}
		fConst0 = (1000.f / float(min(192000, max(1, fSamplingFreq))));
		fHslider1 = FAUSTFLOAT(1.);
		for (int i = 0; (i < 2); i = (i + 1)) {
			fRec2[i] = 0.f;
			
		}
		for (int i = 0; (i < 3); i = (i + 1)) {
			fRec7[i] = 0.f;
			
		}
		for (int i = 0; (i < 3); i = (i + 1)) {
			fRec6[i] = 0.f;
			
		}
		for (int i = 0; (i < 3); i = (i + 1)) {
			fRec5[i] = 0.f;
			
		}
		for (int i = 0; (i < 3); i = (i + 1)) {
			fRec4[i] = 0.f;
			
		}
		for (int i = 0; (i < 3); i = (i + 1)) {
			fRec11[i] = 0.f;
			
		}
		for (int i = 0; (i < 3); i = (i + 1)) {
			fRec10[i] = 0.f;
			
		}
		for (int i = 0; (i < 3); i = (i + 1)) {
			fRec9[i] = 0.f;
			
		}
		for (int i = 0; (i < 3); i = (i + 1)) {
			fRec8[i] = 0.f;
			
		}
		fHslider2 = FAUSTFLOAT(20.);
		for (int i = 0; (i < 2); i = (i + 1)) {
			fRec12[i] = 0.f;
			
		}
		for (int i = 0; (i < 2); i = (i + 1)) {
			fRec3[i] = 0.f;
			
		}
		for (int i = 0; (i < 2); i = (i + 1)) {
			fRec1[i] = 0.f;
			
		}
		fHslider3 = FAUSTFLOAT(-40.);
		for (int i = 0; (i < 2); i = (i + 1)) {
			fRec13[i] = 0.f;
			
		}
		
	}
	
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	
	virtual void buildUserInterface(UI* interface) {
		interface->declare(0, "0", "");
		interface->declare(0, "tooltip", "Reference: http://www.katjaas.nl/compander/compander.html");
		interface->openVerticalBox("qompander");
		interface->declare(&fHslider0, "0", "");
		interface->declare(&fHslider0, "OWL", "PARAMETER_A");
		interface->declare(&fHslider0, "style", "knob");
		interface->declare(&fHslider0, "unit", ": 1");
		interface->addHorizontalSlider("factor", &fHslider0, 3.f, 0.8f, 8.f, 0.01f);
		interface->declare(&fHslider3, "1", "");
		interface->declare(&fHslider3, "OWL", "PARAMETER_B");
		interface->declare(&fHslider3, "style", "knob");
		interface->declare(&fHslider3, "unit", "dB");
		interface->addHorizontalSlider("threshold", &fHslider3, -40.f, -96.f, -20.f, 0.01f);
		interface->declare(&fHslider1, "2", "");
		interface->declare(&fHslider1, "OWL", "PARAMETER_C");
		interface->declare(&fHslider1, "style", "knob");
		interface->declare(&fHslider1, "unit", "ms");
		interface->addHorizontalSlider("attack", &fHslider1, 1.f, 1.f, 20.f, 0.01f);
		interface->declare(&fHslider2, "3", "");
		interface->declare(&fHslider2, "OWL", "PARAMETER_D");
		interface->declare(&fHslider2, "style", "knob");
		interface->declare(&fHslider2, "unit", "ms");
		interface->addHorizontalSlider("release", &fHslider2, 20.f, 20.f, 1000.f, 0.01f);
		interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = (0.001f * float(fHslider0));
		float fSlow1 = (0.001f * float(fHslider1));
		float fSlow2 = (0.001f * float(fHslider2));
		float fSlow3 = (0.001f * float(fHslider3));
		for (int i = 0; (i < count); i = (i + 1)) {
			float fTemp0 = float(input0[i]);
			fVec0[0] = fTemp0;
			fRec0[0] = ((0.999f * fRec0[1]) + fSlow0);
			fRec2[0] = ((0.999f * fRec2[1]) + fSlow1);
			float fTemp1 = expf((0.f - (fConst0 / fRec2[0])));
			fRec7[0] = ((0.161758f * fRec7[2]) + fTemp0);
			fRec6[0] = (((0.161758f * fRec7[0]) + (0.733029f * fRec6[2])) - fRec7[2]);
			fRec5[0] = (((0.733029f * fRec6[0]) + (0.94535f * fRec5[2])) - fRec6[2]);
			fRec4[0] = (((0.94535f * fRec5[0]) + (0.990598f * fRec4[2])) - fRec5[2]);
			float fTemp2 = ((0.990598f * fRec4[0]) - fRec4[2]);
			fRec11[0] = (fVec0[1] + (0.479401f * fRec11[2]));
			fRec10[0] = (((0.479401f * fRec11[0]) + (0.876218f * fRec10[2])) - fRec11[2]);
			fRec9[0] = (((0.876218f * fRec10[0]) + (0.976599f * fRec9[2])) - fRec10[2]);
			fRec8[0] = (((0.976599f * fRec9[0]) + (0.9975f * fRec8[2])) - fRec9[2]);
			float fTemp3 = ((0.9975f * fRec8[0]) - fRec8[2]);
			float fTemp4 = fabsf(min(100.f, max(1e-05f, sqrtf((faustpower2_f(fTemp2) + faustpower2_f(fTemp3))))));
			fRec12[0] = ((0.999f * fRec12[1]) + fSlow2);
			float fTemp5 = expf((0.f - (fConst0 / fRec12[0])));
			fRec3[0] = ((max(fTemp4, fRec3[1]) * fTemp5) + (fTemp4 * (1.f - fTemp5)));
			fRec1[0] = ((fRec1[1] * fTemp1) + (fRec3[0] * (1.f - fTemp1)));
			fRec13[0] = ((0.999f * fRec13[1]) + fSlow3);
			float fTemp6 = powf(10.f, (0.05f * fRec13[0]));
			output0[i] = FAUSTFLOAT((0.707107f * ((powf(min(1.f, max(1e-07f, sinf((1.5708f * (fRec0[0] * min((1.f / fRec0[0]), fRec1[0])))))), (logf(fTemp6) / logf(sinf((1.5708f * (fRec0[0] * fTemp6)))))) * (fTemp2 + fTemp3)) / fRec1[0])));
			fVec0[1] = fVec0[0];
			fRec0[1] = fRec0[0];
			fRec2[1] = fRec2[0];
			fRec7[2] = fRec7[1];
			fRec7[1] = fRec7[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec4[2] = fRec4[1];
			fRec4[1] = fRec4[0];
			fRec11[2] = fRec11[1];
			fRec11[1] = fRec11[0];
			fRec10[2] = fRec10[1];
			fRec10[1] = fRec10[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec12[1] = fRec12[0];
			fRec3[1] = fRec3[0];
			fRec1[1] = fRec1[0];
			fRec13[1] = fRec13[0];
			
		}
		
	}

	
};


/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/



/**************************************************************************************

	qompanderPatch : an OWL patch that calls Faust generated DSP code
	
***************************************************************************************/

class qompanderPatch : public Patch
{
    qompander   fDSP;
    OwlUI	fUI;
    
public:

    qompanderPatch() : fUI(patches.getCurrentPatchProcessor())
    {
        fDSP.init(int(getSampleRate()));		// Init Faust code with the OWL sampling rate
        fDSP.buildUserInterface(&fUI);			// Maps owl parameters and faust widgets 
    }
    
    void processAudio(AudioBuffer &buffer)
    {
        // Reasonably assume we will not have more than 32 channels
        float*  ins[32];
        float*  outs[32];
        int     n = buffer.getChannels();
        
        if ( (fDSP.getNumInputs() < 32) && (fDSP.getNumOutputs() < 32) ) {
            
            // create the table of input channels
            for(int ch=0; ch<fDSP.getNumInputs(); ++ch) {
                ins[ch] = buffer.getSamples(ch%n);
            }
            
            // create the table of output channels
            for(int ch=0; ch<fDSP.getNumOutputs(); ++ch) {
                outs[ch] = buffer.getSamples(ch%n);
            }
            
            // read OWL parameters and updates corresponding Faust Widgets zones
            fUI.update(); 
            
            // Process the audio samples
            fDSP.compute(buffer.getSize(), ins, outs);
        }
    }

};

#endif // __qompanderPatch_h__


////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
