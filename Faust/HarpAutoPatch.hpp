/* ------------------------------------------------------------
author: "Yann Orlarey"
name: "Kisana"
Code generated with Faust 2.0.a34 (http://faust.grame.fr)
------------------------------------------------------------ */

#ifndef  __HarpAuto_H__
#define  __HarpAuto_H__
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

#ifndef __HarpAutoPatch_h__
#define __HarpAutoPatch_h__

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

#include <math.h>


class HarpAutoSIG0 {
	
  private:
	
	
  public:
	
	int getNumInputsHarpAutoSIG0() {
		return 0;
		
	}
	int getNumOutputsHarpAutoSIG0() {
		return 1;
		
	}
	int getInputRateHarpAutoSIG0(int channel) {
		int rate;
		switch (channel) {
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	int getOutputRateHarpAutoSIG0(int channel) {
		int rate;
		switch (channel) {
			case 0: {
				rate = 0;
				break;
			}
			default: {
				rate = -1;
				break;
			}
			
		}
		return rate;
		
	}
	
	void instanceInitHarpAutoSIG0(int samplingFreq) {
		
	}
	
	void fillHarpAutoSIG0(int count, float* output) {
		for (int i = 0; (i < count); i = (i + 1)) {
			output[i] = 0.f;
			
		}
		
	}
};

HarpAutoSIG0* newHarpAutoSIG0() {return (HarpAutoSIG0*) new HarpAutoSIG0(); }
void deleteHarpAutoSIG0(HarpAutoSIG0* dsp) {delete dsp; }


#ifndef FAUSTCLASS 
#define FAUSTCLASS HarpAuto
#endif

class HarpAuto : public dsp {
	
  private:
	
	float fVec5[512];
	float fVec7[512];
	float fVec9[512];
	float fVec3[256];
	float ftbl0[16];
	float fRec0[3];
	float fRec6[3];
	float fRec8[3];
	float fRec10[3];
	int iRec1[2];
	int iVec0[2];
	int iRec3[2];
	int iVec1[2];
	int iRec4[2];
	int iRec5[2];
	int iVec2[2];
	float fRec2[2];
	int iVec4[2];
	float fRec7[2];
	int iVec6[2];
	float fRec9[2];
	int iVec8[2];
	float fRec11[2];
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fHslider2;
	int fSamplingFreq;
	int iConst0;
	FAUSTFLOAT fHslider3;
	int IOTA;
	
  public:
	
	void static metadata(Meta* m) { 
		m->declare("author", "Yann Orlarey");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/version", "1.0");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/license", "LGPL with exception");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/version", "1.0");
		m->declare("name", "Kisana");
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
		fHslider0 = FAUSTFLOAT(0.);
		fHslider1 = FAUSTFLOAT(0.);
		for (int i0 = 0; (i0 < 2); i0 = (i0 + 1)) {
			iRec1[i0] = 0;
			
		}
		HarpAutoSIG0* sig0 = newHarpAutoSIG0();
		sig0->instanceInitHarpAutoSIG0(samplingFreq);
		sig0->fillHarpAutoSIG0(16, ftbl0);
		fHslider2 = FAUSTFLOAT(0.);
		for (int i1 = 0; (i1 < 2); i1 = (i1 + 1)) {
			iVec0[i1] = 0;
			
		}
		iConst0 = (60 * min(192000, max(1, fSamplingFreq)));
		fHslider3 = FAUSTFLOAT(360.);
		for (int i2 = 0; (i2 < 2); i2 = (i2 + 1)) {
			iRec3[i2] = 0;
			
		}
		for (int i3 = 0; (i3 < 2); i3 = (i3 + 1)) {
			iVec1[i3] = 0;
			
		}
		for (int i4 = 0; (i4 < 2); i4 = (i4 + 1)) {
			iRec4[i4] = 0;
			
		}
		for (int i5 = 0; (i5 < 2); i5 = (i5 + 1)) {
			iRec5[i5] = 0;
			
		}
		for (int i6 = 0; (i6 < 2); i6 = (i6 + 1)) {
			iVec2[i6] = 0;
			
		}
		for (int i7 = 0; (i7 < 2); i7 = (i7 + 1)) {
			fRec2[i7] = 0.f;
			
		}
		IOTA = 0;
		for (int i8 = 0; (i8 < 256); i8 = (i8 + 1)) {
			fVec3[i8] = 0.f;
			
		}
		for (int i9 = 0; (i9 < 3); i9 = (i9 + 1)) {
			fRec0[i9] = 0.f;
			
		}
		for (int i10 = 0; (i10 < 2); i10 = (i10 + 1)) {
			iVec4[i10] = 0;
			
		}
		for (int i11 = 0; (i11 < 2); i11 = (i11 + 1)) {
			fRec7[i11] = 0.f;
			
		}
		for (int i12 = 0; (i12 < 512); i12 = (i12 + 1)) {
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
		for (int i16 = 0; (i16 < 512); i16 = (i16 + 1)) {
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
		for (int i20 = 0; (i20 < 512); i20 = (i20 + 1)) {
			fVec9[i20] = 0.f;
			
		}
		for (int i21 = 0; (i21 < 3); i21 = (i21 + 1)) {
			fRec10[i21] = 0.f;
			
		}
		deleteHarpAutoSIG0(sig0);
		
	}
	
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("0x00");
		interface->declare(&fHslider0, "OWL", "PARAMETER_D");
		interface->addHorizontalSlider("Master", &fHslider0, 0.f, 0.f, 1.f, 0.01f);
		interface->declare(&fHslider2, "OWL", "PARAMETER_A");
		interface->addHorizontalSlider("Note", &fHslider2, 0.f, 0.f, 4.f, 1.f);
		interface->declare(&fHslider3, "OWL", "PARAMETER_B");
		interface->addHorizontalSlider("Rate", &fHslider3, 360.f, 120.f, 640.f, 1.f);
		interface->declare(&fHslider1, "OWL", "PARAMETER_C");
		interface->addHorizontalSlider("Timbre", &fHslider1, 0.f, 0.f, 1.f, 0.01f);
		interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = (1.28897f * float(fHslider0));
		float fSlow1 = float(fHslider1);
		float fSlow2 = (1.f + fSlow1);
		float fSlow3 = (1.f - fSlow1);
		int iSlow4 = int(float(fHslider2));
		int iSlow5 = int((float(iConst0) / float(fHslider3)));
		int iSlow6 = int((float(iSlow4) <= 0.f));
		for (int i = 0; (i < count); i = (i + 1)) {
			float fTemp0 = float(input0[i]);
			iRec1[0] = (12345 + (1103515245 * iRec1[1]));
			iVec0[0] = iSlow4;
			iRec3[0] = ((1 + iRec3[1]) % iSlow5);
			int iTemp1 = int((iRec3[0] == 0));
			iVec1[0] = iTemp1;
			int iSel0 = 0;
			if (int(iVec1[1]) != 0) {
				iSel0 = 0;
				
			} else {
				iSel0 = (iRec4[1] + abs((iSlow4 - iVec0[1])));
				
			}
			iRec4[0] = iSel0;
			iRec5[0] = ((iTemp1 + iRec5[1]) % 15);
			ftbl0[(int((iTemp1 & ((iRec4[0] > 0) | iSlow6)))?iRec5[0]:15)] = float(iSlow4);
			int iTemp2 = int((fabsf((ftbl0[iRec5[0]] - 4.f)) < 0.5f));
			iVec2[0] = iTemp2;
			fRec2[0] = ((fRec2[1] + float(int((float((iTemp2 - iVec2[1])) > 0.f)))) - (0.00444439f * float(int((fRec2[1] > 0.f)))));
			fVec3[(IOTA & 255)] = ((0.495614f * ((fSlow2 * fRec0[1]) + (fSlow3 * fRec0[2]))) + (4.65661e-10f * float((iRec1[0] * int((fRec2[0] > 0.f))))));
			fRec0[0] = fVec3[((IOTA - 224) & 255)];
			int iTemp3 = int((fabsf((ftbl0[iRec5[0]] - 1.f)) < 0.5f));
			iVec4[0] = iTemp3;
			fRec7[0] = ((fRec7[1] + float(int((float((iTemp3 - iVec4[1])) > 0.f)))) - (0.00296628f * float(int((fRec7[1] > 0.f)))));
			fVec5[(IOTA & 511)] = ((0.493443f * ((fSlow2 * fRec6[1]) + (fSlow3 * fRec6[2]))) + (4.65661e-10f * float((iRec1[0] * int((fRec7[0] > 0.f))))));
			fRec6[0] = fVec5[((IOTA - 336) & 511)];
			int iTemp4 = int((fabsf((ftbl0[iRec5[0]] - 2.f)) < 0.5f));
			iVec6[0] = iTemp4;
			fRec9[0] = ((fRec9[1] + float(int((float((iTemp4 - iVec6[1])) > 0.f)))) - (0.00332953f * float(int((fRec9[1] > 0.f)))));
			fVec7[(IOTA & 511)] = ((0.494154f * ((fSlow2 * fRec8[1]) + (fSlow3 * fRec8[2]))) + (4.65661e-10f * float((iRec1[0] * int((fRec9[0] > 0.f))))));
			fRec8[0] = fVec7[((IOTA - 299) & 511)];
			int iTemp5 = int((fabsf((ftbl0[iRec5[0]] - 3.f)) < 0.5f));
			iVec8[0] = iTemp5;
			fRec11[0] = ((fRec11[1] + float(int((float((iTemp5 - iVec8[1])) > 0.f)))) - (0.00373727f * float(int((fRec11[1] > 0.f)))));
			fVec9[(IOTA & 511)] = ((0.494788f * ((fSlow2 * fRec10[1]) + (fSlow3 * fRec10[2]))) + (4.65661e-10f * float((iRec1[0] * int((fRec11[0] > 0.f))))));
			fRec10[0] = fVec9[((IOTA - 266) & 511)];
			output0[i] = FAUSTFLOAT(((fSlow0 * (fRec0[0] + ((fRec6[0] + (1.08842f * fRec8[0])) + (1.08842f * fRec10[0])))) + fTemp0));
			iRec1[1] = iRec1[0];
			iVec0[1] = iVec0[0];
			iRec3[1] = iRec3[0];
			iVec1[1] = iVec1[0];
			iRec4[1] = iRec4[0];
			iRec5[1] = iRec5[0];
			iVec2[1] = iVec2[0];
			fRec2[1] = fRec2[0];
			IOTA = (IOTA + 1);
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
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
			
		}
		
	}

	
};


/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/



/**************************************************************************************

	HarpAutoPatch : an OWL patch that calls Faust generated DSP code
	
***************************************************************************************/

class HarpAutoPatch : public Patch
{
    HarpAuto   fDSP;
    OwlUI	fUI;
    
public:

    HarpAutoPatch() : fUI(this)
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

#endif // __HarpAutoPatch_h__


////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
