/* ------------------------------------------------------------
name: "Phaser"
Code generated with Faust 2.0.a34 (http://faust.grame.fr)
------------------------------------------------------------ */

#ifndef  __GuitarixPhaser_H__
#define  __GuitarixPhaser_H__
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

#ifndef __GuitarixPhaserPatch_h__
#define __GuitarixPhaserPatch_h__

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


#ifndef FAUSTCLASS 
#define FAUSTCLASS GuitarixPhaser
#endif

class GuitarixPhaser : public dsp {
	
  private:
	
	float fRec4[3];
	float fRec3[3];
	float fRec2[3];
	float fRec1[3];
	float fRec11[3];
	float fRec10[3];
	float fRec9[3];
	float fRec8[3];
	int iVec0[2];
	float fRec5[2];
	float fRec6[2];
	float fRec0[2];
	float fRec7[2];
	FAUSTFLOAT fHslider0;
	FAUSTFLOAT fCheckbox0;
	FAUSTFLOAT fHslider1;
	int fSamplingFreq;
	int iConst0;
	float fConst1;
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fHslider3;
	FAUSTFLOAT fHslider4;
	FAUSTFLOAT fHslider5;
	float fConst2;
	FAUSTFLOAT fHslider6;
	FAUSTFLOAT fHslider7;
	FAUSTFLOAT fCheckbox1;
	
  public:
	
	void static metadata(Meta* m) { 
		m->declare("category", "Modulation");
		m->declare("effect.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("effect.lib/copyright", "Julius O. Smith III");
		m->declare("effect.lib/exciter_author", "Priyanka Shekar (pshekar@ccrma.stanford.edu)");
		m->declare("effect.lib/exciter_copyright", "Copyright (c) 2013 Priyanka Shekar");
		m->declare("effect.lib/exciter_license", "MIT License (MIT)");
		m->declare("effect.lib/exciter_name", "Harmonic Exciter");
		m->declare("effect.lib/exciter_version", "1.0");
		m->declare("effect.lib/license", "STK-4.3");
		m->declare("effect.lib/name", "Faust Audio Effect Library");
		m->declare("effect.lib/version", "1.33");
		m->declare("filter.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("filter.lib/copyright", "Julius O. Smith III");
		m->declare("filter.lib/license", "STK-4.3");
		m->declare("filter.lib/name", "Faust Filter Library");
		m->declare("filter.lib/reference", "https://ccrma.stanford.edu/~jos/filters/");
		m->declare("filter.lib/version", "1.29");
		m->declare("id", "phaser");
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
		m->declare("name", "Phaser");
		m->declare("oscillator.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("oscillator.lib/copyright", "Julius O. Smith III");
		m->declare("oscillator.lib/license", "STK-4.3");
		m->declare("oscillator.lib/name", "Faust Oscillator Library");
		m->declare("oscillator.lib/version", "1.11");
	}

	virtual int getNumInputs() {
		return 2;
		
	}
	virtual int getNumOutputs() {
		return 2;
		
	}
	virtual int getInputRate(int channel) {
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
		fHslider0 = FAUSTFLOAT(0.);
		for (int i0 = 0; (i0 < 2); i0 = (i0 + 1)) {
			iVec0[i0] = 0;
			
		}
		fCheckbox0 = FAUSTFLOAT(0.);
		fHslider1 = FAUSTFLOAT(1.);
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (1.f / float(iConst0));
		fHslider2 = FAUSTFLOAT(1000.);
		fHslider3 = FAUSTFLOAT(1.5);
		fHslider4 = FAUSTFLOAT(100.);
		fHslider5 = FAUSTFLOAT(800.);
		fConst2 = (6.28319f / float(iConst0));
		fHslider6 = FAUSTFLOAT(0.5);
		for (int i1 = 0; (i1 < 2); i1 = (i1 + 1)) {
			fRec5[i1] = 0.f;
			
		}
		for (int i2 = 0; (i2 < 2); i2 = (i2 + 1)) {
			fRec6[i2] = 0.f;
			
		}
		fHslider7 = FAUSTFLOAT(0.);
		for (int i3 = 0; (i3 < 3); i3 = (i3 + 1)) {
			fRec4[i3] = 0.f;
			
		}
		for (int i4 = 0; (i4 < 3); i4 = (i4 + 1)) {
			fRec3[i4] = 0.f;
			
		}
		for (int i5 = 0; (i5 < 3); i5 = (i5 + 1)) {
			fRec2[i5] = 0.f;
			
		}
		for (int i6 = 0; (i6 < 3); i6 = (i6 + 1)) {
			fRec1[i6] = 0.f;
			
		}
		for (int i7 = 0; (i7 < 2); i7 = (i7 + 1)) {
			fRec0[i7] = 0.f;
			
		}
		fCheckbox1 = FAUSTFLOAT(0.);
		for (int i8 = 0; (i8 < 3); i8 = (i8 + 1)) {
			fRec11[i8] = 0.f;
			
		}
		for (int i9 = 0; (i9 < 3); i9 = (i9 + 1)) {
			fRec10[i9] = 0.f;
			
		}
		for (int i10 = 0; (i10 < 3); i10 = (i10 + 1)) {
			fRec9[i10] = 0.f;
			
		}
		for (int i11 = 0; (i11 < 3); i11 = (i11 + 1)) {
			fRec8[i11] = 0.f;
			
		}
		for (int i12 = 0; (i12 < 2); i12 = (i12 + 1)) {
			fRec7[i12] = 0.f;
			
		}
		
	}
	
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("0x00");
		interface->declare(&fHslider1, "OWL", "PARAMETER_B");
		interface->addHorizontalSlider("Depth", &fHslider1, 1.f, 0.f, 1.f, 0.01f);
		interface->declare(&fHslider7, "OWL", "PARAMETER_C");
		interface->addHorizontalSlider("Feedback Gain", &fHslider7, 0.f, 0.f, 1.f, 0.01f);
		interface->declare(&fHslider5, "unit", "Hz");
		interface->addHorizontalSlider("MaxNotch1Freq", &fHslider5, 800.f, 20.f, 10000.f, 1.f);
		interface->declare(&fHslider4, "unit", "Hz");
		interface->addHorizontalSlider("MinNotch1Freq", &fHslider4, 100.f, 20.f, 5000.f, 1.f);
		interface->declare(&fHslider2, "OWL", "PARAMETER_D");
		interface->declare(&fHslider2, "unit", "Hz");
		interface->addHorizontalSlider("Notch Width", &fHslider2, 1000.f, 10.f, 5000.f, 1.f);
		interface->addHorizontalSlider("NotchFreq", &fHslider3, 1.5f, 1.1f, 4.f, 0.01f);
		interface->declare(&fHslider6, "OWL", "PARAMETER_A");
		interface->declare(&fHslider6, "unit", "Hz");
		interface->addHorizontalSlider("Speed", &fHslider6, 0.5f, 0.f, 10.f, 0.01f);
		interface->declare(&fCheckbox0, "enum", "direct | vibrato");
		interface->addCheckButton("VibratoMode",&fCheckbox0);
		interface->declare(&fCheckbox1, "enum", "linear|invert");
		interface->addCheckButton("invert",&fCheckbox1);
		interface->declare(&fHslider0, "unit", "dB");
		interface->addHorizontalSlider("level", &fHslider0, 0.f, -60.f, 10.f, 0.1f);
		interface->closeBox();
		
	}
	
float faustpower2_f(float value) {
	return (value * value);
	
}
float faustpower3_f(float value) {
	return ((value * value) * value);
	
}
float faustpower4_f(float value) {
	return (((value * value) * value) * value);
	
}
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* input1 = inputs[1];
		FAUSTFLOAT* output0 = outputs[0];
		FAUSTFLOAT* output1 = outputs[1];
		float fSlow0 = powf(10.f, (0.05f * float(fHslider0)));
		float fSlow1 = (0.5f * (int(float(fCheckbox0))?2.f:float(fHslider1)));
		float fSlow2 = (1.f - fSlow1);
		float fSlow3 = expf((fConst1 * (0.f - (3.14159f * float(fHslider2)))));
		float fSlow4 = faustpower2_f(fSlow3);
		float fSlow5 = (0.f - (2.f * fSlow3));
		float fSlow6 = float(fHslider3);
		float fSlow7 = (fConst1 * fSlow6);
		float fSlow8 = float(fHslider4);
		float fSlow9 = (6.28319f * fSlow8);
		float fSlow10 = (0.5f * ((6.28319f * max(fSlow8, float(fHslider5))) - fSlow9));
		float fSlow11 = (fConst2 * float(fHslider6));
		float fSlow12 = sinf(fSlow11);
		float fSlow13 = cosf(fSlow11);
		float fSlow14 = (0.f - fSlow12);
		float fSlow15 = float(fHslider7);
		float fSlow16 = (fConst1 * faustpower2_f(fSlow6));
		float fSlow17 = (fConst1 * faustpower3_f(fSlow6));
		float fSlow18 = (fConst1 * faustpower4_f(fSlow6));
		float fSlow19 = (int(float(fCheckbox1))?(0.f - fSlow1):fSlow1);
		for (int i = 0; (i < count); i = (i + 1)) {
			float fTemp0 = float(input0[i]);
			float fTemp1 = float(input1[i]);
			iVec0[0] = 1;
			fRec5[0] = ((fSlow12 * fRec6[1]) + (fSlow13 * fRec5[1]));
			fRec6[0] = ((1.f + ((fSlow13 * fRec6[1]) + (fSlow14 * fRec5[1]))) - float(iVec0[1]));
			float fTemp2 = ((fSlow10 * (1.f - fRec5[0])) + fSlow9);
			float fTemp3 = (fRec4[1] * cosf((fSlow7 * fTemp2)));
			fRec4[0] = (0.f - (((fSlow5 * fTemp3) + (fSlow4 * fRec4[2])) - ((fSlow0 * fTemp0) + (fSlow15 * fRec0[1]))));
			float fTemp4 = (fRec3[1] * cosf((fSlow16 * fTemp2)));
			fRec3[0] = ((fSlow5 * (fTemp3 - fTemp4)) + (fRec4[2] + (fSlow4 * (fRec4[0] - fRec3[2]))));
			float fTemp5 = (fRec2[1] * cosf((fSlow17 * fTemp2)));
			fRec2[0] = ((fSlow5 * (fTemp4 - fTemp5)) + (fRec3[2] + (fSlow4 * (fRec3[0] - fRec2[2]))));
			float fTemp6 = (fRec1[1] * cosf((fSlow18 * fTemp2)));
			fRec1[0] = ((fSlow5 * (fTemp5 - fTemp6)) + (fRec2[2] + (fSlow4 * (fRec2[0] - fRec1[2]))));
			fRec0[0] = ((fSlow4 * fRec1[0]) + ((fSlow5 * fTemp6) + fRec1[2]));
			output0[i] = FAUSTFLOAT(((fSlow0 * (fSlow2 * fTemp0)) + (fRec0[0] * fSlow19)));
			float fTemp7 = ((fSlow10 * (1.f - fRec6[0])) + fSlow9);
			float fTemp8 = (fRec11[1] * cosf((fSlow7 * fTemp7)));
			fRec11[0] = (0.f - (((fSlow5 * fTemp8) + (fSlow4 * fRec11[2])) - ((fSlow0 * fTemp1) + (fSlow15 * fRec7[1]))));
			float fTemp9 = (fRec10[1] * cosf((fSlow16 * fTemp7)));
			fRec10[0] = ((fSlow5 * (fTemp8 - fTemp9)) + (fRec11[2] + (fSlow4 * (fRec11[0] - fRec10[2]))));
			float fTemp10 = (fRec9[1] * cosf((fSlow17 * fTemp7)));
			fRec9[0] = ((fSlow5 * (fTemp9 - fTemp10)) + (fRec10[2] + (fSlow4 * (fRec10[0] - fRec9[2]))));
			float fTemp11 = (fRec8[1] * cosf((fSlow18 * fTemp7)));
			fRec8[0] = ((fSlow5 * (fTemp10 - fTemp11)) + (fRec9[2] + (fSlow4 * (fRec9[0] - fRec8[2]))));
			fRec7[0] = ((fSlow4 * fRec8[0]) + ((fSlow5 * fTemp11) + fRec8[2]));
			output1[i] = FAUSTFLOAT(((fSlow0 * (fSlow2 * fTemp1)) + (fRec7[0] * fSlow19)));
			iVec0[1] = iVec0[0];
			fRec5[1] = fRec5[0];
			fRec6[1] = fRec6[0];
			fRec4[2] = fRec4[1];
			fRec4[1] = fRec4[0];
			fRec3[2] = fRec3[1];
			fRec3[1] = fRec3[0];
			fRec2[2] = fRec2[1];
			fRec2[1] = fRec2[0];
			fRec1[2] = fRec1[1];
			fRec1[1] = fRec1[0];
			fRec0[1] = fRec0[0];
			fRec11[2] = fRec11[1];
			fRec11[1] = fRec11[0];
			fRec10[2] = fRec10[1];
			fRec10[1] = fRec10[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec7[1] = fRec7[0];
			
		}
		
	}

	
};


/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/



/**************************************************************************************

	GuitarixPhaserPatch : an OWL patch that calls Faust generated DSP code
	
***************************************************************************************/

class GuitarixPhaserPatch : public Patch
{
    GuitarixPhaser   fDSP;
    OwlUI	fUI;
    
public:

    GuitarixPhaserPatch() : fUI(this)
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

#endif // __GuitarixPhaserPatch_h__


////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
