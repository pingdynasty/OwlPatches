//-----------------------------------------------------
// name: "Phaser"
//
// Code generated with Faust 0.9.67 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with  */
#include <math.h>
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

#ifndef __GuitarixPhaserPatch_h__
#define __GuitarixPhaserPatch_h__

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
#define FAUSTCLASS GuitarixPhaser
#endif

class GuitarixPhaser : public dsp {
  private:
	FAUSTFLOAT 	fslider0;
	int 	iVec0[2];
	FAUSTFLOAT 	fcheckbox0;
	FAUSTFLOAT 	fslider1;
	int 	iConst0;
	float 	fConst1;
	FAUSTFLOAT 	fslider2;
	FAUSTFLOAT 	fslider3;
	FAUSTFLOAT 	fslider4;
	FAUSTFLOAT 	fslider5;
	float 	fConst2;
	FAUSTFLOAT 	fslider6;
	float 	fRec5[2];
	float 	fRec6[2];
	FAUSTFLOAT 	fslider7;
	float 	fRec4[3];
	float 	fRec3[3];
	float 	fRec2[3];
	float 	fRec1[3];
	float 	fRec0[2];
	FAUSTFLOAT 	fcheckbox1;
	float 	fRec11[3];
	float 	fRec10[3];
	float 	fRec9[3];
	float 	fRec8[3];
	float 	fRec7[2];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("effect.lib/name", "Faust Audio Effect Library");
		m->declare("effect.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("effect.lib/copyright", "Julius O. Smith III");
		m->declare("effect.lib/version", "1.33");
		m->declare("effect.lib/license", "STK-4.3");
		m->declare("effect.lib/exciter_name", "Harmonic Exciter");
		m->declare("effect.lib/exciter_author", "Priyanka Shekar (pshekar@ccrma.stanford.edu)");
		m->declare("effect.lib/exciter_copyright", "Copyright (c) 2013 Priyanka Shekar");
		m->declare("effect.lib/exciter_version", "1.0");
		m->declare("effect.lib/exciter_license", "MIT License (MIT)");
		m->declare("filter.lib/name", "Faust Filter Library");
		m->declare("filter.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("filter.lib/copyright", "Julius O. Smith III");
		m->declare("filter.lib/version", "1.29");
		m->declare("filter.lib/license", "STK-4.3");
		m->declare("filter.lib/reference", "https://ccrma.stanford.edu/~jos/filters/");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/version", "1.0");
		m->declare("music.lib/license", "LGPL with exception");
		m->declare("id", "phaser");
		m->declare("name", "Phaser");
		m->declare("category", "Modulation");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("oscillator.lib/name", "Faust Oscillator Library");
		m->declare("oscillator.lib/author", "Julius O. Smith (jos at ccrma.stanford.edu)");
		m->declare("oscillator.lib/copyright", "Julius O. Smith III");
		m->declare("oscillator.lib/version", "1.11");
		m->declare("oscillator.lib/license", "STK-4.3");
	}

	virtual int getNumInputs() 	{ return 2; }
	virtual int getNumOutputs() 	{ return 2; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fslider0 = 0.0f;
		for (int i=0; i<2; i++) iVec0[i] = 0;
		fcheckbox0 = 0.0;
		fslider1 = 1.0f;
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (1.0f / float(iConst0));
		fslider2 = 1e+03f;
		fslider3 = 1.5f;
		fslider4 = 1e+02f;
		fslider5 = 8e+02f;
		fConst2 = (6.283185307179586f / float(iConst0));
		fslider6 = 0.5f;
		for (int i=0; i<2; i++) fRec5[i] = 0;
		for (int i=0; i<2; i++) fRec6[i] = 0;
		fslider7 = 0.0f;
		for (int i=0; i<3; i++) fRec4[i] = 0;
		for (int i=0; i<3; i++) fRec3[i] = 0;
		for (int i=0; i<3; i++) fRec2[i] = 0;
		for (int i=0; i<3; i++) fRec1[i] = 0;
		for (int i=0; i<2; i++) fRec0[i] = 0;
		fcheckbox1 = 0.0;
		for (int i=0; i<3; i++) fRec11[i] = 0;
		for (int i=0; i<3; i++) fRec10[i] = 0;
		for (int i=0; i<3; i++) fRec9[i] = 0;
		for (int i=0; i<3; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("GuitarixPhaser");
		interface->declare(&fslider1, "OWL", "PARAMETER_B");
		interface->addHorizontalSlider("Depth", &fslider1, 1.0f, 0.0f, 1.0f, 0.01f);
		interface->declare(&fslider7, "OWL", "PARAMETER_C");
		interface->addHorizontalSlider("Feedback Gain", &fslider7, 0.0f, 0.0f, 1.0f, 0.01f);
		interface->declare(&fslider5, "unit", "Hz");
		interface->addHorizontalSlider("MaxNotch1Freq", &fslider5, 8e+02f, 2e+01f, 1e+04f, 1.0f);
		interface->declare(&fslider4, "unit", "Hz");
		interface->addHorizontalSlider("MinNotch1Freq", &fslider4, 1e+02f, 2e+01f, 5e+03f, 1.0f);
		interface->declare(&fslider2, "OWL", "PARAMETER_D");
		interface->declare(&fslider2, "unit", "Hz");
		interface->addHorizontalSlider("Notch Width", &fslider2, 1e+03f, 1e+01f, 5e+03f, 1.0f);
		interface->addHorizontalSlider("NotchFreq", &fslider3, 1.5f, 1.1f, 4.0f, 0.01f);
		interface->declare(&fslider6, "OWL", "PARAMETER_A");
		interface->declare(&fslider6, "unit", "Hz");
		interface->addHorizontalSlider("Speed", &fslider6, 0.5f, 0.0f, 1e+01f, 0.01f);
		interface->declare(&fcheckbox0, "enum", "direct | vibrato");
		interface->addCheckButton("VibratoMode", &fcheckbox0);
		interface->declare(&fcheckbox1, "enum", "linear|invert");
		interface->addCheckButton("invert", &fcheckbox1);
		interface->declare(&fslider0, "unit", "dB");
		interface->addHorizontalSlider("level", &fslider0, 0.0f, -6e+01f, 1e+01f, 0.1f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = powf(10,(0.05f * float(fslider0)));
		float 	fSlow1 = (0.5f * ((int(float(fcheckbox0)))?2:float(fslider1)));
		float 	fSlow2 = (1 - fSlow1);
		float 	fSlow3 = expf((fConst1 * (0 - (3.141592653589793f * float(fslider2)))));
		float 	fSlow4 = (0 - (2 * fSlow3));
		float 	fSlow5 = faustpower<2>(fSlow3);
		float 	fSlow6 = float(fslider3);
		float 	fSlow7 = (fConst1 * fSlow6);
		float 	fSlow8 = float(fslider4);
		float 	fSlow9 = (6.283185307179586f * fSlow8);
		float 	fSlow10 = (0.5f * ((6.283185307179586f * max(fSlow8, float(fslider5))) - fSlow9));
		float 	fSlow11 = (fConst2 * float(fslider6));
		float 	fSlow12 = sinf(fSlow11);
		float 	fSlow13 = cosf(fSlow11);
		float 	fSlow14 = (0 - fSlow12);
		float 	fSlow15 = float(fslider7);
		float 	fSlow16 = (fConst1 * faustpower<2>(fSlow6));
		float 	fSlow17 = (fConst1 * faustpower<3>(fSlow6));
		float 	fSlow18 = (fConst1 * faustpower<4>(fSlow6));
		float 	fSlow19 = ((int(float(fcheckbox1)))?(0 - fSlow1):fSlow1);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			float fTemp0 = (float)input0[i];
			float fTemp1 = (float)input1[i];
			iVec0[0] = 1;
			fRec5[0] = ((fSlow12 * fRec6[1]) + (fSlow13 * fRec5[1]));
			fRec6[0] = ((1 + ((fSlow13 * fRec6[1]) + (fSlow14 * fRec5[1]))) - iVec0[1]);
			float fTemp2 = (fSlow9 + (fSlow10 * (1 - fRec5[0])));
			float fTemp3 = (fRec4[1] * cosf((fSlow7 * fTemp2)));
			fRec4[0] = (0 - (((fSlow4 * fTemp3) + (fSlow5 * fRec4[2])) - ((fSlow0 * fTemp0) + (fSlow15 * fRec0[1]))));
			float fTemp4 = (fRec3[1] * cosf((fSlow16 * fTemp2)));
			fRec3[0] = ((fSlow5 * (fRec4[0] - fRec3[2])) + (fRec4[2] + (fSlow4 * (fTemp3 - fTemp4))));
			float fTemp5 = (fRec2[1] * cosf((fSlow17 * fTemp2)));
			fRec2[0] = ((fSlow4 * (fTemp4 - fTemp5)) + (fRec3[2] + (fSlow5 * (fRec3[0] - fRec2[2]))));
			float fTemp6 = (fRec1[1] * cosf((fSlow18 * fTemp2)));
			fRec1[0] = ((fRec2[2] + (fSlow5 * (fRec2[0] - fRec1[2]))) + (fSlow4 * (fTemp5 - fTemp6)));
			fRec0[0] = (((fSlow4 * fTemp6) + fRec1[2]) + (fSlow5 * fRec1[0]));
			output0[i] = (FAUSTFLOAT)((fSlow0 * (fTemp0 * fSlow2)) + (fRec0[0] * fSlow19));
			float fTemp7 = (fSlow9 + (fSlow10 * (1 - fRec6[0])));
			float fTemp8 = (fRec11[1] * cosf((fSlow7 * fTemp7)));
			fRec11[0] = (0 - (((fSlow4 * fTemp8) + (fSlow5 * fRec11[2])) - ((fSlow0 * fTemp1) + (fSlow15 * fRec7[1]))));
			float fTemp9 = (cosf((fSlow16 * fTemp7)) * fRec10[1]);
			fRec10[0] = ((fSlow4 * (fTemp8 - fTemp9)) + (fRec11[2] + (fSlow5 * (fRec11[0] - fRec10[2]))));
			float fTemp10 = (fRec9[1] * cosf((fSlow17 * fTemp7)));
			fRec9[0] = ((fSlow4 * (fTemp9 - fTemp10)) + (fRec10[2] + (fSlow5 * (fRec10[0] - fRec9[2]))));
			float fTemp11 = (fRec8[1] * cosf((fSlow18 * fTemp7)));
			fRec8[0] = ((fRec9[2] + (fSlow5 * (fRec9[0] - fRec8[2]))) + (fSlow4 * (fTemp10 - fTemp11)));
			fRec7[0] = (((fSlow4 * fTemp11) + fRec8[2]) + (fSlow5 * fRec8[0]));
			output1[i] = (FAUSTFLOAT)((fSlow0 * (fTemp1 * fSlow2)) + (fRec7[0] * fSlow19));
			// post processing
			fRec7[1] = fRec7[0];
			fRec8[2] = fRec8[1]; fRec8[1] = fRec8[0];
			fRec9[2] = fRec9[1]; fRec9[1] = fRec9[0];
			fRec10[2] = fRec10[1]; fRec10[1] = fRec10[0];
			fRec11[2] = fRec11[1]; fRec11[1] = fRec11[0];
			fRec0[1] = fRec0[0];
			fRec1[2] = fRec1[1]; fRec1[1] = fRec1[0];
			fRec2[2] = fRec2[1]; fRec2[1] = fRec2[0];
			fRec3[2] = fRec3[1]; fRec3[1] = fRec3[0];
			fRec4[2] = fRec4[1]; fRec4[1] = fRec4[0];
			fRec6[1] = fRec6[0];
			fRec5[1] = fRec5[0];
			iVec0[1] = iVec0[0];
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

    GuitarixPhaserPatch() : fUI(patches.getCurrentPatchProcessor())
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
