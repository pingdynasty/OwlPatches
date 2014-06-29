//-----------------------------------------------------
// name: "Harpe"
// author: "Grame"
//
// Code generated with Faust 0.9.67 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with  */
#ifndef FAUSTPOWER
#define FAUSTPOWER
#include <cmath>
template <int N> inline float faustpower(float x)          { return powf(x,N); } 
template <int N> inline double faustpower(double x)        { return pow(x,N); }
template <int N> inline int faustpower(int x)              { return faustpower<N/2>(x) * faustpower<N-N/2>(x); } 
template <> 	 inline int faustpower<0>(int x)            { return 1; }
template <> 	 inline int faustpower<1>(int x)            { return x; }
#endif
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

#ifndef __HarpPatch_h__
#define __HarpPatch_h__

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
 protected:
	int fSamplingFreq;
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

    virtual void declare(FAUSTFLOAT*, const char*, const char*) {}
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

typedef long double quad;

#ifndef FAUSTCLASS 
#define FAUSTCLASS Harp
#endif

class Harp : public dsp {
  private:
	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fslider1;
	int 	iRec1[2];
	FAUSTFLOAT 	fslider2;
	float 	fRec3[2];
	int 	iVec0[2];
	float 	fRec2[2];
	int 	IOTA;
	float 	fVec1[128];
	float 	fRec0[3];
	int 	iVec2[2];
	float 	fRec5[2];
	float 	fVec3[128];
	float 	fRec4[3];
	int 	iVec4[2];
	float 	fRec7[2];
	float 	fVec5[128];
	float 	fRec6[3];
	int 	iVec6[2];
	float 	fRec9[2];
	float 	fVec7[128];
	float 	fRec8[3];
	int 	iVec8[2];
	float 	fRec11[2];
	float 	fVec9[64];
	float 	fRec10[3];
	int 	iVec10[2];
	float 	fRec13[2];
	float 	fVec11[64];
	float 	fRec12[3];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("name", "Harpe");
		m->declare("author", "Grame");
	}

	virtual int getNumInputs() 	{ return 0; }
	virtual int getNumOutputs() 	{ return 2; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fslider0 = 0.005f;
		fslider1 = 0.5f;
		for (int i=0; i<2; i++) iRec1[i] = 0;
		fslider2 = 0.0f;
		for (int i=0; i<2; i++) fRec3[i] = 0;
		for (int i=0; i<2; i++) iVec0[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		IOTA = 0;
		for (int i=0; i<128; i++) fVec1[i] = 0;
		for (int i=0; i<3; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) iVec2[i] = 0;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<128; i++) fVec3[i] = 0;
		for (int i=0; i<3; i++) fRec4[i] = 0;
		for (int i=0; i<2; i++) iVec4[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<128; i++) fVec5[i] = 0;
		for (int i=0; i<3; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) iVec6[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<128; i++) fVec7[i] = 0;
		for (int i=0; i<3; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) iVec8[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<64; i++) fVec9[i] = 0;
		for (int i=0; i<3; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) iVec10[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<64; i++) fVec11[i] = 0;
		for (int i=0; i<3; i++) fRec12[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("Harp");
		interface->declare(&fslider0, "OWL", "PARAMETER_B");
		interface->declare(&fslider0, "osc", "/1/fader3");
		interface->addHorizontalSlider("attenuation", &fslider0, 0.005f, 0.0f, 0.01f, 0.001f);
		interface->declare(&fslider2, "OWL", "PARAMETER_C");
		interface->declare(&fslider2, "osc", "/accxyz/1 -10 10");
		interface->addHorizontalSlider("hand", &fslider2, 0.0f, 0.0f, 1.0f, 0.01f);
		interface->declare(&fslider1, "OWL", "PARAMETER_A");
		interface->declare(&fslider1, "osc", "/accxyz/0 -10 10");
		interface->declare(&fslider1, "unit", "f");
		interface->addHorizontalSlider("level", &fslider1, 0.5f, 0.0f, 1.0f, 0.01f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = (0.5f * (1.0f - float(fslider0)));
		float 	fSlow1 = (4.656612875245797e-10f * faustpower<2>(float(fslider1)));
		float 	fSlow2 = (0.09999999999999998f * float(fslider2));
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			iRec1[0] = ((1103515245 * iRec1[1]) + 12345);
			fRec3[0] = ((0.9f * fRec3[1]) + fSlow2);
			float fTemp0 = min(fRec3[0], fRec3[1]);
			float fTemp1 = max(fRec3[0], fRec3[1]);
			int iTemp2 = ((fTemp0 < 0.08333333333333333f) & (0.08333333333333333f < fTemp1));
			iVec0[0] = iTemp2;
			fRec2[0] = ((fRec2[1] + ((iVec0[0] - iVec0[1]) > 0.0f)) - (0.009977324263038548f * (fRec2[1] > 0.0f)));
			fVec1[IOTA&127] = ((fSlow0 * (fRec0[1] + fRec0[2])) + (fSlow1 * (iRec1[0] * (fRec2[0] > 0.0f))));
			fRec0[0] = fVec1[(IOTA-99)&127];
			int iTemp3 = ((fTemp0 < 0.25f) & (0.25f < fTemp1));
			iVec2[0] = iTemp3;
			fRec5[0] = ((fRec5[1] + ((iVec2[0] - iVec2[1]) > 0.0f)) - (0.011460935968224386f * (fRec5[1] > 0.0f)));
			fVec3[IOTA&127] = ((fSlow0 * (fRec4[1] + fRec4[2])) + (fSlow1 * (iRec1[0] * (fRec5[0] > 0.0f))));
			fRec4[0] = fVec3[(IOTA-86)&127];
			int iTemp4 = ((fTemp0 < 0.4166666666666667f) & (0.4166666666666667f < fTemp1));
			iVec4[0] = iTemp4;
			fRec7[0] = ((fRec7[1] + ((iVec4[0] - iVec4[1]) > 0.0f)) - (0.013165158293425702f * (fRec7[1] > 0.0f)));
			fVec5[IOTA&127] = ((fSlow0 * (fRec6[1] + fRec6[2])) + (fSlow1 * (iRec1[0] * (fRec7[0] > 0.0f))));
			fRec6[0] = fVec5[(IOTA-74)&127];
			int iTemp5 = ((fTemp0 < 0.5833333333333334f) & (0.5833333333333334f < fTemp1));
			iVec6[0] = iTemp5;
			fRec9[0] = ((fRec9[1] + ((iVec6[0] - iVec6[1]) > 0.0f)) - (0.015122795674933676f * (fRec9[1] > 0.0f)));
			fVec7[IOTA&127] = ((fSlow0 * (fRec8[1] + fRec8[2])) + (fSlow1 * (iRec1[0] * (fRec9[0] > 0.0f))));
			fRec8[0] = fVec7[(IOTA-65)&127];
			int iTemp6 = ((fTemp0 < 0.75f) & (0.75f < fTemp1));
			iVec8[0] = iTemp6;
			fRec11[0] = ((fRec11[1] + ((iVec8[0] - iVec8[1]) > 0.0f)) - (0.01737153051475259f * (fRec11[1] > 0.0f)));
			fVec9[IOTA&63] = ((fSlow0 * (fRec10[2] + fRec10[1])) + (fSlow1 * (iRec1[0] * (fRec11[0] > 0.0f))));
			fRec10[0] = fVec9[(IOTA-56)&63];
			int iTemp7 = ((fTemp0 < 0.9166666666666666f) & (0.9166666666666666f < fTemp1));
			iVec10[0] = iTemp7;
			fRec13[0] = ((fRec13[1] + ((iVec10[0] - iVec10[1]) > 0.0f)) - (0.019954648526077097f * (fRec13[1] > 0.0f)));
			fVec11[IOTA&63] = ((fSlow0 * (fRec12[1] + fRec12[2])) + (fSlow1 * (iRec1[0] * (fRec13[0] > 0.0f))));
			fRec12[0] = fVec11[(IOTA-49)&63];
			output0[i] = (FAUSTFLOAT)((((((0.9574271077563381f * fRec0[0]) + (0.8660254037844386f * fRec4[0])) + (0.7637626158259733f * fRec6[0])) + (0.6454972243679028f * fRec8[0])) + (0.5f * fRec10[0])) + (0.2886751345948129f * fRec12[0]));
			output1[i] = (FAUSTFLOAT)((((((0.28867513459481287f * fRec0[0]) + (0.5f * fRec4[0])) + (0.6454972243679028f * fRec6[0])) + (0.7637626158259734f * fRec8[0])) + (0.8660254037844386f * fRec10[0])) + (0.9574271077563381f * fRec12[0]));
			// post processing
			fRec12[2] = fRec12[1]; fRec12[1] = fRec12[0];
			fRec13[1] = fRec13[0];
			iVec10[1] = iVec10[0];
			fRec10[2] = fRec10[1]; fRec10[1] = fRec10[0];
			fRec11[1] = fRec11[0];
			iVec8[1] = iVec8[0];
			fRec8[2] = fRec8[1]; fRec8[1] = fRec8[0];
			fRec9[1] = fRec9[0];
			iVec6[1] = iVec6[0];
			fRec6[2] = fRec6[1]; fRec6[1] = fRec6[0];
			fRec7[1] = fRec7[0];
			iVec4[1] = iVec4[0];
			fRec4[2] = fRec4[1]; fRec4[1] = fRec4[0];
			fRec5[1] = fRec5[0];
			iVec2[1] = iVec2[0];
			fRec0[2] = fRec0[1]; fRec0[1] = fRec0[0];
			IOTA = IOTA+1;
			fRec2[1] = fRec2[0];
			iVec0[1] = iVec0[0];
			fRec3[1] = fRec3[0];
			iRec1[1] = iRec1[0];
		}
	}
};



/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/



/**************************************************************************************

	HarpPatch : an OWL patch that calls Faust generated DSP code
	
***************************************************************************************/

class HarpPatch : public Patch
{
    Harp   fDSP;
    OwlUI	fUI;
    
public:

    HarpPatch() : fUI(patches.getCurrentPatchProcessor())
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

#endif // __HarpPatch_h__


////////////////////////////////////////////////////////////////////////////////////////////////////
