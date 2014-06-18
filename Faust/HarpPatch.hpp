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
	int 	iVec12[2];
	float 	fRec15[2];
	float 	fVec13[64];
	float 	fRec14[3];
	int 	iVec14[2];
	float 	fRec17[2];
	float 	fVec15[64];
	float 	fRec16[3];
	int 	iVec16[2];
	float 	fRec19[2];
	float 	fVec17[64];
	float 	fRec18[3];
	int 	iVec18[2];
	float 	fRec21[2];
	float 	fVec19[32];
	float 	fRec20[3];
	int 	iVec20[2];
	float 	fRec23[2];
	float 	fVec21[32];
	float 	fRec22[3];
	int 	iVec22[2];
	float 	fRec25[2];
	float 	fVec23[32];
	float 	fRec24[3];
	int 	iVec24[2];
	float 	fRec27[2];
	float 	fVec25[32];
	float 	fRec26[3];
	int 	iVec26[2];
	float 	fRec29[2];
	float 	fVec27[16];
	float 	fRec28[3];
	int 	iVec28[2];
	float 	fRec31[2];
	float 	fVec29[14];
	float 	fRec30[3];
	int 	iVec30[2];
	float 	fRec33[2];
	float 	fVec31[12];
	float 	fRec32[3];
	int 	iVec32[2];
	float 	fRec35[2];
	float 	fVec33[10];
	float 	fRec34[3];
	int 	iVec34[2];
	float 	fRec37[2];
	float 	fVec35[9];
	float 	fRec36[3];
	int 	iVec36[2];
	float 	fRec39[2];
	float 	fVec37[8];
	float 	fRec38[3];
	int 	iVec38[2];
	float 	fRec41[2];
	float 	fVec39[7];
	float 	fRec40[3];
	int 	iVec40[2];
	float 	fRec43[2];
	float 	fVec41[6];
	float 	fRec42[3];
	int 	iVec42[2];
	float 	fRec45[2];
	float 	fVec43[5];
	float 	fRec44[3];
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
		for (int i=0; i<2; i++) iVec12[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<64; i++) fVec13[i] = 0;
		for (int i=0; i<3; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) iVec14[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<64; i++) fVec15[i] = 0;
		for (int i=0; i<3; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) iVec16[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<64; i++) fVec17[i] = 0;
		for (int i=0; i<3; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) iVec18[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<32; i++) fVec19[i] = 0;
		for (int i=0; i<3; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) iVec20[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<32; i++) fVec21[i] = 0;
		for (int i=0; i<3; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) iVec22[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<32; i++) fVec23[i] = 0;
		for (int i=0; i<3; i++) fRec24[i] = 0;
		for (int i=0; i<2; i++) iVec24[i] = 0;
		for (int i=0; i<2; i++) fRec27[i] = 0;
		for (int i=0; i<32; i++) fVec25[i] = 0;
		for (int i=0; i<3; i++) fRec26[i] = 0;
		for (int i=0; i<2; i++) iVec26[i] = 0;
		for (int i=0; i<2; i++) fRec29[i] = 0;
		for (int i=0; i<16; i++) fVec27[i] = 0;
		for (int i=0; i<3; i++) fRec28[i] = 0;
		for (int i=0; i<2; i++) iVec28[i] = 0;
		for (int i=0; i<2; i++) fRec31[i] = 0;
		for (int i=0; i<14; i++) fVec29[i] = 0;
		for (int i=0; i<3; i++) fRec30[i] = 0;
		for (int i=0; i<2; i++) iVec30[i] = 0;
		for (int i=0; i<2; i++) fRec33[i] = 0;
		for (int i=0; i<12; i++) fVec31[i] = 0;
		for (int i=0; i<3; i++) fRec32[i] = 0;
		for (int i=0; i<2; i++) iVec32[i] = 0;
		for (int i=0; i<2; i++) fRec35[i] = 0;
		for (int i=0; i<10; i++) fVec33[i] = 0;
		for (int i=0; i<3; i++) fRec34[i] = 0;
		for (int i=0; i<2; i++) iVec34[i] = 0;
		for (int i=0; i<2; i++) fRec37[i] = 0;
		for (int i=0; i<9; i++) fVec35[i] = 0;
		for (int i=0; i<3; i++) fRec36[i] = 0;
		for (int i=0; i<2; i++) iVec36[i] = 0;
		for (int i=0; i<2; i++) fRec39[i] = 0;
		for (int i=0; i<8; i++) fVec37[i] = 0;
		for (int i=0; i<3; i++) fRec38[i] = 0;
		for (int i=0; i<2; i++) iVec38[i] = 0;
		for (int i=0; i<2; i++) fRec41[i] = 0;
		for (int i=0; i<7; i++) fVec39[i] = 0;
		for (int i=0; i<3; i++) fRec40[i] = 0;
		for (int i=0; i<2; i++) iVec40[i] = 0;
		for (int i=0; i<2; i++) fRec43[i] = 0;
		for (int i=0; i<6; i++) fVec41[i] = 0;
		for (int i=0; i<3; i++) fRec42[i] = 0;
		for (int i=0; i<2; i++) iVec42[i] = 0;
		for (int i=0; i<2; i++) fRec45[i] = 0;
		for (int i=0; i<5; i++) fVec43[i] = 0;
		for (int i=0; i<3; i++) fRec44[i] = 0;
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
			iRec1[0] = (12345 + (1103515245 * iRec1[1]));
			fRec3[0] = ((0.9f * fRec3[1]) + fSlow2);
			float fTemp0 = min(fRec3[0], fRec3[1]);
			float fTemp1 = max(fRec3[0], fRec3[1]);
			int iTemp2 = ((fTemp0 < 0.022727272727272728f) & (0.022727272727272728f < fTemp1));
			iVec0[0] = iTemp2;
			fRec2[0] = ((fRec2[1] + ((iVec0[0] - iVec0[1]) > 0.0f)) - (0.009977324263038548f * (fRec2[1] > 0.0f)));
			fVec1[IOTA&127] = ((fSlow0 * (fRec0[1] + fRec0[2])) + (fSlow1 * (iRec1[0] * (fRec2[0] > 0.0f))));
			fRec0[0] = fVec1[(IOTA-99)&127];
			int iTemp3 = ((fTemp0 < 0.06818181818181818f) & (0.06818181818181818f < fTemp1));
			iVec2[0] = iTemp3;
			fRec5[0] = ((fRec5[1] + ((iVec2[0] - iVec2[1]) > 0.0f)) - (0.011460935968224386f * (fRec5[1] > 0.0f)));
			fVec3[IOTA&127] = ((fSlow0 * (fRec4[1] + fRec4[2])) + (fSlow1 * (iRec1[0] * (fRec5[0] > 0.0f))));
			fRec4[0] = fVec3[(IOTA-86)&127];
			int iTemp4 = ((fTemp0 < 0.11363636363636363f) & (0.11363636363636363f < fTemp1));
			iVec4[0] = iTemp4;
			fRec7[0] = ((fRec7[1] + ((iVec4[0] - iVec4[1]) > 0.0f)) - (0.013165158293425702f * (fRec7[1] > 0.0f)));
			fVec5[IOTA&127] = ((fSlow0 * (fRec6[1] + fRec6[2])) + (fSlow1 * (iRec1[0] * (fRec7[0] > 0.0f))));
			fRec6[0] = fVec5[(IOTA-74)&127];
			int iTemp5 = ((fTemp0 < 0.1590909090909091f) & (0.1590909090909091f < fTemp1));
			iVec6[0] = iTemp5;
			fRec9[0] = ((fRec9[1] + ((iVec6[0] - iVec6[1]) > 0.0f)) - (0.015122795674933676f * (fRec9[1] > 0.0f)));
			fVec7[IOTA&127] = ((fSlow0 * (fRec8[1] + fRec8[2])) + (fSlow1 * (iRec1[0] * (fRec9[0] > 0.0f))));
			fRec8[0] = fVec7[(IOTA-65)&127];
			int iTemp6 = ((fTemp0 < 0.20454545454545456f) & (0.20454545454545456f < fTemp1));
			iVec8[0] = iTemp6;
			fRec11[0] = ((fRec11[1] + ((iVec8[0] - iVec8[1]) > 0.0f)) - (0.01737153051475259f * (fRec11[1] > 0.0f)));
			fVec9[IOTA&63] = ((fSlow0 * (fRec10[2] + fRec10[1])) + (fSlow1 * (iRec1[0] * (fRec11[0] > 0.0f))));
			fRec10[0] = fVec9[(IOTA-56)&63];
			int iTemp7 = ((fTemp0 < 0.25f) & (0.25f < fTemp1));
			iVec10[0] = iTemp7;
			fRec13[0] = ((fRec13[1] + ((iVec10[0] - iVec10[1]) > 0.0f)) - (0.019954648526077097f * (fRec13[1] > 0.0f)));
			fVec11[IOTA&63] = ((fSlow0 * (fRec12[1] + fRec12[2])) + (fSlow1 * (iRec1[0] * (fRec13[0] > 0.0f))));
			fRec12[0] = fVec11[(IOTA-49)&63];
			int iTemp8 = ((fTemp0 < 0.29545454545454547f) & (0.29545454545454547f < fTemp1));
			iVec12[0] = iTemp8;
			fRec15[0] = ((fRec15[1] + ((iVec12[0] - iVec12[1]) > 0.0f)) - (0.02292187193644877f * (fRec15[1] > 0.0f)));
			fVec13[IOTA&63] = ((fSlow0 * (fRec14[1] + fRec14[2])) + (fSlow1 * (iRec1[0] * (fRec15[0] > 0.0f))));
			fRec14[0] = fVec13[(IOTA-42)&63];
			int iTemp9 = ((fTemp0 < 0.3409090909090909f) & (0.3409090909090909f < fTemp1));
			iVec14[0] = iTemp9;
			fRec17[0] = ((fRec17[1] + ((iVec14[0] - iVec14[1]) > 0.0f)) - (0.026330316586851404f * (fRec17[1] > 0.0f)));
			fVec15[IOTA&63] = ((fSlow0 * (fRec16[1] + fRec16[2])) + (fSlow1 * (iRec1[0] * (fRec17[0] > 0.0f))));
			fRec16[0] = fVec15[(IOTA-36)&63];
			int iTemp10 = ((fTemp0 < 0.38636363636363635f) & (0.38636363636363635f < fTemp1));
			iVec16[0] = iTemp10;
			fRec19[0] = ((fRec19[1] + ((iVec16[0] - iVec16[1]) > 0.0f)) - (0.03024559134986736f * (fRec19[1] > 0.0f)));
			fVec17[IOTA&63] = ((fSlow0 * (fRec18[1] + fRec18[2])) + (fSlow1 * (iRec1[0] * (fRec19[0] > 0.0f))));
			fRec18[0] = fVec17[(IOTA-32)&63];
			int iTemp11 = ((fTemp0 < 0.4318181818181818f) & (0.4318181818181818f < fTemp1));
			iVec18[0] = iTemp11;
			fRec21[0] = ((fRec21[1] + ((iVec18[0] - iVec18[1]) > 0.0f)) - (0.03474306102950518f * (fRec21[1] > 0.0f)));
			fVec19[IOTA&31] = ((fSlow0 * (fRec20[1] + fRec20[2])) + (fSlow1 * (iRec1[0] * (fRec21[0] > 0.0f))));
			fRec20[0] = fVec19[(IOTA-27)&31];
			int iTemp12 = ((fTemp0 < 0.4772727272727273f) & (0.4772727272727273f < fTemp1));
			iVec20[0] = iTemp12;
			fRec23[0] = ((fRec23[1] + ((iVec20[0] - iVec20[1]) > 0.0f)) - (0.039909297052154194f * (fRec23[1] > 0.0f)));
			fVec21[IOTA&31] = ((fSlow0 * (fRec22[1] + fRec22[2])) + (fSlow1 * (iRec1[0] * (fRec23[0] > 0.0f))));
			fRec22[0] = fVec21[(IOTA-24)&31];
			int iTemp13 = ((fTemp0 < 0.5227272727272727f) & (0.5227272727272727f < fTemp1));
			iVec22[0] = iTemp13;
			fRec25[0] = ((fRec25[1] + ((iVec22[0] - iVec22[1]) > 0.0f)) - (0.045843743872897545f * (fRec25[1] > 0.0f)));
			fVec23[IOTA&31] = ((fSlow0 * (fRec24[1] + fRec24[2])) + (fSlow1 * (iRec1[0] * (fRec25[0] > 0.0f))));
			fRec24[0] = fVec23[(IOTA-20)&31];
			int iTemp14 = ((fTemp0 < 0.5681818181818182f) & (0.5681818181818182f < fTemp1));
			iVec24[0] = iTemp14;
			fRec27[0] = ((fRec27[1] + ((iVec24[0] - iVec24[1]) > 0.0f)) - (0.05266063317370281f * (fRec27[1] > 0.0f)));
			fVec25[IOTA&31] = ((fSlow0 * (fRec26[1] + fRec26[2])) + (fSlow1 * (iRec1[0] * (fRec27[0] > 0.0f))));
			fRec26[0] = fVec25[(IOTA-17)&31];
			int iTemp15 = ((fTemp0 < 0.6136363636363636f) & (0.6136363636363636f < fTemp1));
			iVec26[0] = iTemp15;
			fRec29[0] = ((fRec29[1] + ((iVec26[0] - iVec26[1]) > 0.0f)) - (0.06049118269973472f * (fRec29[1] > 0.0f)));
			fVec27[0] = ((fSlow0 * (fRec28[1] + fRec28[2])) + (fSlow1 * (iRec1[0] * (fRec29[0] > 0.0f))));
			fRec28[0] = fVec27[15];
			int iTemp16 = ((fTemp0 < 0.6590909090909091f) & (0.6590909090909091f < fTemp1));
			iVec28[0] = iTemp16;
			fRec31[0] = ((fRec31[1] + ((iVec28[0] - iVec28[1]) > 0.0f)) - (0.06948612205901036f * (fRec31[1] > 0.0f)));
			fVec29[0] = ((fSlow0 * (fRec30[1] + fRec30[2])) + (fSlow1 * (iRec1[0] * (fRec31[0] > 0.0f))));
			fRec30[0] = fVec29[13];
			int iTemp17 = ((fTemp0 < 0.7045454545454546f) & (0.7045454545454546f < fTemp1));
			iVec30[0] = iTemp17;
			fRec33[0] = ((fRec33[1] + ((iVec30[0] - iVec30[1]) > 0.0f)) - (0.07981859410430839f * (fRec33[1] > 0.0f)));
			fVec31[0] = ((fSlow0 * (fRec32[1] + fRec32[2])) + (fSlow1 * (iRec1[0] * (fRec33[0] > 0.0f))));
			fRec32[0] = fVec31[11];
			int iTemp18 = ((fTemp0 < 0.75f) & (0.75f < fTemp1));
			iVec32[0] = iTemp18;
			fRec35[0] = ((fRec35[1] + ((iVec32[0] - iVec32[1]) > 0.0f)) - (0.09168748774579509f * (fRec35[1] > 0.0f)));
			fVec33[0] = ((fSlow0 * (fRec34[1] + fRec34[2])) + (fSlow1 * (iRec1[0] * (fRec35[0] > 0.0f))));
			fRec34[0] = fVec33[9];
			int iTemp19 = ((fTemp0 < 0.7954545454545454f) & (0.7954545454545454f < fTemp1));
			iVec34[0] = iTemp19;
			fRec37[0] = ((fRec37[1] + ((iVec34[0] - iVec34[1]) > 0.0f)) - (0.10532126634740561f * (fRec37[1] > 0.0f)));
			fVec35[0] = ((fSlow0 * (fRec36[1] + fRec36[2])) + (fSlow1 * (iRec1[0] * (fRec37[0] > 0.0f))));
			fRec36[0] = fVec35[8];
			int iTemp20 = ((fTemp0 < 0.8409090909090909f) & (0.8409090909090909f < fTemp1));
			iVec36[0] = iTemp20;
			fRec39[0] = ((fRec39[1] + ((iVec36[0] - iVec36[1]) > 0.0f)) - (0.12098236539946944f * (fRec39[1] > 0.0f)));
			fVec37[0] = ((fSlow0 * (fRec38[1] + fRec38[2])) + (fSlow1 * (iRec1[0] * (fRec39[0] > 0.0f))));
			fRec38[0] = fVec37[7];
			int iTemp21 = ((fTemp0 < 0.8863636363636364f) & (0.8863636363636364f < fTemp1));
			iVec38[0] = iTemp21;
			fRec41[0] = ((fRec41[1] + ((iVec38[0] - iVec38[1]) > 0.0f)) - (0.13897224411802073f * (fRec41[1] > 0.0f)));
			fVec39[0] = ((fSlow0 * (fRec40[1] + fRec40[2])) + (fSlow1 * (iRec1[0] * (fRec41[0] > 0.0f))));
			fRec40[0] = fVec39[6];
			int iTemp22 = ((fTemp0 < 0.9318181818181818f) & (0.9318181818181818f < fTemp1));
			iVec40[0] = iTemp22;
			fRec43[0] = ((fRec43[1] + ((iVec40[0] - iVec40[1]) > 0.0f)) - (0.15963718820861678f * (fRec43[1] > 0.0f)));
			fVec41[0] = ((fSlow0 * (fRec42[1] + fRec42[2])) + (fSlow1 * (iRec1[0] * (fRec43[0] > 0.0f))));
			fRec42[0] = fVec41[5];
			int iTemp23 = ((fTemp0 < 0.9772727272727273f) & (0.9772727272727273f < fTemp1));
			iVec42[0] = iTemp23;
			fRec45[0] = ((fRec45[1] + ((iVec42[0] - iVec42[1]) > 0.0f)) - (0.18337497549159018f * (fRec45[1] > 0.0f)));
			fVec43[0] = ((fSlow0 * (fRec44[1] + fRec44[2])) + (fSlow1 * (iRec1[0] * (fRec45[0] > 0.0f))));
			fRec44[0] = fVec43[4];
			output0[i] = (FAUSTFLOAT)((((((((((((((((((((((0.9885710532241612f * fRec0[0]) + (0.9653072991634228f * fRec4[0])) + (0.9414688716912718f * fRec6[0])) + (0.917010954628728f * fRec8[0])) + (0.8918825850158447f * fRec10[0])) + (0.8660254037844386f * fRec12[0])) + (0.8393720596645177f * fRec14[0])) + (0.8118441408859888f * fRec16[0])) + (0.7833494518006403f * fRec18[0])) + (0.753778361444409f * fRec20[0])) + (0.7229988054812212f * fRec22[0])) + (0.6908492797077574f * fRec24[0])) + (0.6571287406727709f * fRec26[0])) + (0.621581560508061f * fRec28[0])) + (0.5838742081211422f * fRec30[0])) + (0.543557306504609f * fRec32[0])) + (0.5f * fRec34[0])) + (0.4522670168666455f * fRec36[0])) + (0.3988620176087328f * fRec38[0])) + (0.3370999312316211f * fRec40[0])) + (0.26111648393354686f * fRec42[0])) + (0.15075567228888173f * fRec44[0]));
			output1[i] = (FAUSTFLOAT)((((((((((((((((((((((0.15075567228888181f * fRec0[0]) + (0.26111648393354675f * fRec4[0])) + (0.337099931231621f * fRec6[0])) + (0.3988620176087328f * fRec8[0])) + (0.45226701686664544f * fRec10[0])) + (0.5f * fRec12[0])) + (0.543557306504609f * fRec14[0])) + (0.5838742081211422f * fRec16[0])) + (0.621581560508061f * fRec18[0])) + (0.6571287406727709f * fRec20[0])) + (0.6908492797077574f * fRec22[0])) + (0.7229988054812212f * fRec24[0])) + (0.7537783614444091f * fRec26[0])) + (0.7833494518006403f * fRec28[0])) + (0.8118441408859887f * fRec30[0])) + (0.8393720596645177f * fRec32[0])) + (0.8660254037844386f * fRec34[0])) + (0.8918825850158447f * fRec36[0])) + (0.917010954628728f * fRec38[0])) + (0.9414688716912718f * fRec40[0])) + (0.9653072991634227f * fRec42[0])) + (0.9885710532241612f * fRec44[0]));
			// post processing
			fRec44[2] = fRec44[1]; fRec44[1] = fRec44[0];
			for (int i=4; i>0; i--) fVec43[i] = fVec43[i-1];
			fRec45[1] = fRec45[0];
			iVec42[1] = iVec42[0];
			fRec42[2] = fRec42[1]; fRec42[1] = fRec42[0];
			for (int i=5; i>0; i--) fVec41[i] = fVec41[i-1];
			fRec43[1] = fRec43[0];
			iVec40[1] = iVec40[0];
			fRec40[2] = fRec40[1]; fRec40[1] = fRec40[0];
			for (int i=6; i>0; i--) fVec39[i] = fVec39[i-1];
			fRec41[1] = fRec41[0];
			iVec38[1] = iVec38[0];
			fRec38[2] = fRec38[1]; fRec38[1] = fRec38[0];
			for (int i=7; i>0; i--) fVec37[i] = fVec37[i-1];
			fRec39[1] = fRec39[0];
			iVec36[1] = iVec36[0];
			fRec36[2] = fRec36[1]; fRec36[1] = fRec36[0];
			for (int i=8; i>0; i--) fVec35[i] = fVec35[i-1];
			fRec37[1] = fRec37[0];
			iVec34[1] = iVec34[0];
			fRec34[2] = fRec34[1]; fRec34[1] = fRec34[0];
			for (int i=9; i>0; i--) fVec33[i] = fVec33[i-1];
			fRec35[1] = fRec35[0];
			iVec32[1] = iVec32[0];
			fRec32[2] = fRec32[1]; fRec32[1] = fRec32[0];
			for (int i=11; i>0; i--) fVec31[i] = fVec31[i-1];
			fRec33[1] = fRec33[0];
			iVec30[1] = iVec30[0];
			fRec30[2] = fRec30[1]; fRec30[1] = fRec30[0];
			for (int i=13; i>0; i--) fVec29[i] = fVec29[i-1];
			fRec31[1] = fRec31[0];
			iVec28[1] = iVec28[0];
			fRec28[2] = fRec28[1]; fRec28[1] = fRec28[0];
			for (int i=15; i>0; i--) fVec27[i] = fVec27[i-1];
			fRec29[1] = fRec29[0];
			iVec26[1] = iVec26[0];
			fRec26[2] = fRec26[1]; fRec26[1] = fRec26[0];
			fRec27[1] = fRec27[0];
			iVec24[1] = iVec24[0];
			fRec24[2] = fRec24[1]; fRec24[1] = fRec24[0];
			fRec25[1] = fRec25[0];
			iVec22[1] = iVec22[0];
			fRec22[2] = fRec22[1]; fRec22[1] = fRec22[0];
			fRec23[1] = fRec23[0];
			iVec20[1] = iVec20[0];
			fRec20[2] = fRec20[1]; fRec20[1] = fRec20[0];
			fRec21[1] = fRec21[0];
			iVec18[1] = iVec18[0];
			fRec18[2] = fRec18[1]; fRec18[1] = fRec18[0];
			fRec19[1] = fRec19[0];
			iVec16[1] = iVec16[0];
			fRec16[2] = fRec16[1]; fRec16[1] = fRec16[0];
			fRec17[1] = fRec17[0];
			iVec14[1] = iVec14[0];
			fRec14[2] = fRec14[1]; fRec14[1] = fRec14[0];
			fRec15[1] = fRec15[0];
			iVec12[1] = iVec12[0];
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
