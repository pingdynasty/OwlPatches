/* ------------------------------------------------------------
author: "Grame"
name: "Harpe"
Code generated with Faust 2.0.a34 (http://faust.grame.fr)
------------------------------------------------------------ */

#ifndef  __Harp_H__
#define  __Harp_H__
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
//  Signal processor definition
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
	Patch* 	fPatch;		// needed to register and read owl parameters
	PatchParameterId	fParameter;		// OWL parameter code : PARAMETER_A,...
	FAUSTFLOAT* 		fZone;			// Faust widget zone
	const char*			fLabel;			// Faust widget label 
	float				fMin;			// Faust widget minimal value
	float				fSpan;			// Faust widget value span (max-min)
	
  public:
	OwlWidget() :
		fPatch(0), fParameter(PARAMETER_A), fZone(0), fLabel(""), fMin(0), fSpan(1) {}
	OwlWidget(const OwlWidget& w) :
		fPatch(w.fPatch), fParameter(w.fParameter), fZone(w.fZone), fLabel(w.fLabel), fMin(w.fMin), fSpan(w.fSpan) {}
	OwlWidget(Patch* pp, PatchParameterId param, FAUSTFLOAT* z, const char* l, float lo, float hi) :
		fPatch(pp), fParameter(param), fZone(z), fLabel(l), fMin(lo), fSpan(hi-lo) {}
	void bind() 	{ fPatch->registerParameter(fParameter, fLabel); }
	void update()	{ *fZone = fMin + fSpan*fPatch->getParameterValue(fParameter); }
	
};


/**************************************************************************************

	OwlUI : Faust User Interface builder. Passed to buildUserInterface OwlUI ensures
	the mapping between owl parameters and faust widgets. It relies on specific
	metadata "...[OWL:PARAMETER_X]..." in widget's label for that. For example any 
	faust widget with metadata [OWL:PARAMETER_B] will be controlled by PARAMETER_B 
	(the second knob).
	
***************************************************************************************/

// The maximun number of mappings between owl parameters and faust widgets 
#define MAXOWLWIDGETS 8

class OwlUI : public UI
{
	Patch* 	fPatch;
	PatchParameterId	fParameter;					// current parameter ID, value PARAMETER_F means not set
	int					fIndex;						// number of OwlWidgets collected so far
	OwlWidget			fTable[MAXOWLWIDGETS];		// kind of static list of OwlWidgets
	
	// check if the widget is an Owl parameter and, if so, add the corresponding OwlWidget
	void addOwlWidget(const char* label, FAUSTFLOAT* zone, FAUSTFLOAT lo, FAUSTFLOAT hi) {
		if ((fParameter >= PARAMETER_A) && (fParameter <= PARAMETER_E) && (fIndex < MAXOWLWIDGETS)) {
			fTable[fIndex] = OwlWidget(fPatch, fParameter, zone, label, lo, hi);
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

	OwlUI(Patch* pp) : fPatch(pp), fParameter(PARAMETER_F), fIndex(0) {}
	
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


float faustpower2_f(float value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS Harp
#endif

class Harp : public dsp {
	
  private:
	
	float fVec1[128];
	float fVec3[128];
	float fVec5[128];
	float fVec7[128];
	float fVec9[64];
	float fVec11[64];
	float fRec0[3];
	float fRec4[3];
	float fRec6[3];
	float fRec8[3];
	float fRec10[3];
	float fRec12[3];
	int iRec1[2];
	float fRec3[2];
	int iVec0[2];
	float fRec2[2];
	int iVec2[2];
	float fRec5[2];
	int iVec4[2];
	float fRec7[2];
	int iVec6[2];
	float fRec9[2];
	int iVec8[2];
	float fRec11[2];
	int iVec10[2];
	float fRec13[2];
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fHslider2;
	int IOTA;
	int fSamplingFreq;
	
  public:
	
	void static metadata(Meta* m) { 
		m->declare("author", "Grame");
		m->declare("name", "Harpe");
	}

	virtual int getNumInputs() {
		return 0;
		
	}
	virtual int getNumOutputs() {
		return 2;
		
	}
	virtual int getInputRate(int channel) {
		int rate;
		switch (channel) {
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
			case 1: {
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
		fHslider0 = FAUSTFLOAT(0.005);
		fHslider1 = FAUSTFLOAT(0.5);
		for (int i0 = 0; (i0 < 2); i0 = (i0 + 1)) {
			iRec1[i0] = 0;
			
		}
		fHslider2 = FAUSTFLOAT(0.);
		for (int i1 = 0; (i1 < 2); i1 = (i1 + 1)) {
			fRec3[i1] = 0.f;
			
		}
		for (int i2 = 0; (i2 < 2); i2 = (i2 + 1)) {
			iVec0[i2] = 0;
			
		}
		for (int i3 = 0; (i3 < 2); i3 = (i3 + 1)) {
			fRec2[i3] = 0.f;
			
		}
		IOTA = 0;
		for (int i4 = 0; (i4 < 128); i4 = (i4 + 1)) {
			fVec1[i4] = 0.f;
			
		}
		for (int i5 = 0; (i5 < 3); i5 = (i5 + 1)) {
			fRec0[i5] = 0.f;
			
		}
		for (int i6 = 0; (i6 < 2); i6 = (i6 + 1)) {
			iVec2[i6] = 0;
			
		}
		for (int i7 = 0; (i7 < 2); i7 = (i7 + 1)) {
			fRec5[i7] = 0.f;
			
		}
		for (int i8 = 0; (i8 < 128); i8 = (i8 + 1)) {
			fVec3[i8] = 0.f;
			
		}
		for (int i9 = 0; (i9 < 3); i9 = (i9 + 1)) {
			fRec4[i9] = 0.f;
			
		}
		for (int i10 = 0; (i10 < 2); i10 = (i10 + 1)) {
			iVec4[i10] = 0;
			
		}
		for (int i11 = 0; (i11 < 2); i11 = (i11 + 1)) {
			fRec7[i11] = 0.f;
			
		}
		for (int i12 = 0; (i12 < 128); i12 = (i12 + 1)) {
			fVec5[i12] = 0.f;
			
		}
		for (int i13 = 0; (i13 < 3); i13 = (i13 + 1)) {
			fRec6[i13] = 0.f;
			
		}
		for (int i14 = 0; (i14 < 2); i14 = (i14 + 1)) {
			iVec6[i14] = 0;
			
		}
		for (int i15 = 0; (i15 < 2); i15 = (i15 + 1)) {
			fRec9[i15] = 0.f;
			
		}
		for (int i16 = 0; (i16 < 128); i16 = (i16 + 1)) {
			fVec7[i16] = 0.f;
			
		}
		for (int i17 = 0; (i17 < 3); i17 = (i17 + 1)) {
			fRec8[i17] = 0.f;
			
		}
		for (int i18 = 0; (i18 < 2); i18 = (i18 + 1)) {
			iVec8[i18] = 0;
			
		}
		for (int i19 = 0; (i19 < 2); i19 = (i19 + 1)) {
			fRec11[i19] = 0.f;
			
		}
		for (int i20 = 0; (i20 < 64); i20 = (i20 + 1)) {
			fVec9[i20] = 0.f;
			
		}
		for (int i21 = 0; (i21 < 3); i21 = (i21 + 1)) {
			fRec10[i21] = 0.f;
			
		}
		for (int i22 = 0; (i22 < 2); i22 = (i22 + 1)) {
			iVec10[i22] = 0;
			
		}
		for (int i23 = 0; (i23 < 2); i23 = (i23 + 1)) {
			fRec13[i23] = 0.f;
			
		}
		for (int i24 = 0; (i24 < 64); i24 = (i24 + 1)) {
			fVec11[i24] = 0.f;
			
		}
		for (int i25 = 0; (i25 < 3); i25 = (i25 + 1)) {
			fRec12[i25] = 0.f;
			
		}
		
	}
	
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("0x00");
		interface->declare(&fHslider0, "OWL", "PARAMETER_B");
		interface->declare(&fHslider0, "osc", "/1/fader3");
		interface->addHorizontalSlider("Attenuation", &fHslider0, 0.005f, 0.f, 0.01f, 0.001f);
		interface->declare(&fHslider2, "OWL", "PARAMETER_C");
		interface->declare(&fHslider2, "osc", "/accxyz/1 -10 10");
		interface->addHorizontalSlider("Hand", &fHslider2, 0.f, 0.f, 1.f, 0.01f);
		interface->declare(&fHslider1, "OWL", "PARAMETER_A");
		interface->declare(&fHslider1, "osc", "/accxyz/0 -10 10");
		interface->declare(&fHslider1, "unit", "f");
		interface->addHorizontalSlider("Level", &fHslider1, 0.5f, 0.f, 1.f, 0.01f);
		interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = (0.5f * (1.f - float(fHslider0)));
		float fSlow1 = (4.65661e-10f * faustpower2_f(float(fHslider1)));
		float fSlow2 = (0.1f * float(fHslider2));
		for (int i = 0; (i < count); i = (i + 1)) {
			iRec1[0] = (12345 + (1103515245 * iRec1[1]));
			fRec3[0] = ((0.9f * fRec3[1]) + fSlow2);
			float fTemp0 = min(fRec3[0], fRec3[1]);
			float fTemp1 = max(fRec3[0], fRec3[1]);
			int iTemp2 = (int((fTemp0 < 0.0833333f)) & int((0.0833333f < fTemp1)));
			iVec0[0] = iTemp2;
			fRec2[0] = ((fRec2[1] + float(int((float((iTemp2 - iVec0[1])) > 0.f)))) - (0.00997732f * float(int((fRec2[1] > 0.f)))));
			fVec1[(IOTA & 127)] = ((fSlow0 * (fRec0[1] + fRec0[2])) + (fSlow1 * float((iRec1[0] * int((fRec2[0] > 0.f))))));
			fRec0[0] = fVec1[((IOTA - 99) & 127)];
			int iTemp3 = (int((fTemp0 < 0.25f)) & int((0.25f < fTemp1)));
			iVec2[0] = iTemp3;
			fRec5[0] = ((fRec5[1] + float(int((float((iTemp3 - iVec2[1])) > 0.f)))) - (0.0114609f * float(int((fRec5[1] > 0.f)))));
			fVec3[(IOTA & 127)] = ((fSlow0 * (fRec4[1] + fRec4[2])) + (fSlow1 * float((iRec1[0] * int((fRec5[0] > 0.f))))));
			fRec4[0] = fVec3[((IOTA - 86) & 127)];
			int iTemp4 = (int((fTemp0 < 0.416667f)) & int((0.416667f < fTemp1)));
			iVec4[0] = iTemp4;
			fRec7[0] = ((fRec7[1] + float(int((float((iTemp4 - iVec4[1])) > 0.f)))) - (0.0131652f * float(int((fRec7[1] > 0.f)))));
			fVec5[(IOTA & 127)] = ((fSlow0 * (fRec6[1] + fRec6[2])) + (fSlow1 * float((iRec1[0] * int((fRec7[0] > 0.f))))));
			fRec6[0] = fVec5[((IOTA - 74) & 127)];
			int iTemp5 = (int((fTemp0 < 0.583333f)) & int((0.583333f < fTemp1)));
			iVec6[0] = iTemp5;
			fRec9[0] = ((fRec9[1] + float(int((float((iTemp5 - iVec6[1])) > 0.f)))) - (0.0151228f * float(int((fRec9[1] > 0.f)))));
			fVec7[(IOTA & 127)] = ((fSlow0 * (fRec8[1] + fRec8[2])) + (fSlow1 * float((iRec1[0] * int((fRec9[0] > 0.f))))));
			fRec8[0] = fVec7[((IOTA - 65) & 127)];
			int iTemp6 = (int((fTemp0 < 0.75f)) & int((0.75f < fTemp1)));
			iVec8[0] = iTemp6;
			fRec11[0] = ((fRec11[1] + float(int((float((iTemp6 - iVec8[1])) > 0.f)))) - (0.0173715f * float(int((fRec11[1] > 0.f)))));
			fVec9[(IOTA & 63)] = ((fSlow0 * (fRec10[1] + fRec10[2])) + (fSlow1 * float((iRec1[0] * int((fRec11[0] > 0.f))))));
			fRec10[0] = fVec9[((IOTA - 56) & 63)];
			int iTemp7 = (int((fTemp0 < 0.916667f)) & int((0.916667f < fTemp1)));
			iVec10[0] = iTemp7;
			fRec13[0] = ((fRec13[1] + float(int((float((iTemp7 - iVec10[1])) > 0.f)))) - (0.0199546f * float(int((fRec13[1] > 0.f)))));
			fVec11[(IOTA & 63)] = ((fSlow0 * (fRec12[1] + fRec12[2])) + (fSlow1 * float((iRec1[0] * int((fRec13[0] > 0.f))))));
			fRec12[0] = fVec11[((IOTA - 49) & 63)];
			output0[i] = FAUSTFLOAT(((((((0.957427f * fRec0[0]) + (0.866025f * fRec4[0])) + (0.763763f * fRec6[0])) + (0.645497f * fRec8[0])) + (0.5f * fRec10[0])) + (0.288675f * fRec12[0])));
			output1[i] = FAUSTFLOAT(((((((0.288675f * fRec0[0]) + (0.5f * fRec4[0])) + (0.645497f * fRec6[0])) + (0.763763f * fRec8[0])) + (0.866025f * fRec10[0])) + (0.957427f * fRec12[0])));
			iRec1[1] = iRec1[0];
			fRec3[1] = fRec3[0];
			iVec0[1] = iVec0[0];
			fRec2[1] = fRec2[0];
			IOTA = (IOTA + 1);
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			iVec2[1] = iVec2[0];
			fRec5[1] = fRec5[0];
			fRec4[2] = fRec4[1];
			fRec4[1] = fRec4[0];
			iVec4[1] = iVec4[0];
			fRec7[1] = fRec7[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			iVec6[1] = iVec6[0];
			fRec9[1] = fRec9[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			iVec8[1] = iVec8[0];
			fRec11[1] = fRec11[0];
			fRec10[2] = fRec10[1];
			fRec10[1] = fRec10[0];
			iVec10[1] = iVec10[0];
			fRec13[1] = fRec13[0];
			fRec12[2] = fRec12[1];
			fRec12[1] = fRec12[0];
			
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

    HarpPatch() : fUI(this)
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

#endif
