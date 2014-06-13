//-----------------------------------------------------
// name: "Kisana"
// author: "Yann Orlarey"
//
// Code generated with Faust 0.9.67 (http://faust.grame.fr)
//-----------------------------------------------------
/* link with  */
#include <math.h>
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

#ifndef __FstHarpeAutoPatch_h__
#define __FstHarpeAutoPatch_h__

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
#define FAUSTCLASS FstHarpeAuto
#endif

class FstHarpeAuto : public dsp {
  private:
	class SIG0 {
	  private:
		int 	fSamplingFreq;
	  public:
		int getNumInputs() 	{ return 0; }
		int getNumOutputs() 	{ return 1; }
		void init(int samplingFreq) {
			fSamplingFreq = samplingFreq;
		}
		void fill (int count, float output[]) {
			for (int i=0; i<count; i++) {
				output[i] = 0.0f;
			}
		}
	};


	FAUSTFLOAT 	fslider0;
	FAUSTFLOAT 	fslider1;
	int 	iRec1[2];
	float 	ftbl0[16];
	int 	iConst0;
	int 	iRec3[2];
	int 	iVec0[2];
	FAUSTFLOAT 	fslider2;
	int 	iVec1[2];
	int 	iRec4[2];
	int 	iRec5[2];
	int 	iVec2[2];
	float 	fRec2[2];
	int 	IOTA;
	float 	fVec3[512];
	float 	fRec0[3];
	int 	iVec4[2];
	float 	fRec7[2];
	float 	fVec5[512];
	float 	fRec6[3];
	int 	iVec6[2];
	float 	fRec9[2];
	float 	fVec7[512];
	float 	fRec8[3];
	int 	iVec8[2];
	float 	fRec11[2];
	float 	fVec9[256];
	float 	fRec10[3];
	int 	iVec10[2];
	float 	fRec13[2];
	float 	fVec11[256];
	float 	fRec12[3];
	int 	iVec12[2];
	float 	fRec15[2];
	float 	fVec13[256];
	float 	fRec14[3];
	int 	iVec14[2];
	float 	fRec17[2];
	float 	fVec15[256];
	float 	fRec16[3];
	int 	iVec16[2];
	float 	fRec19[2];
	float 	fVec17[256];
	float 	fRec18[3];
	int 	iVec18[2];
	float 	fRec21[2];
	float 	fVec19[128];
	float 	fRec20[3];
	int 	iVec20[2];
	float 	fRec23[2];
	float 	fVec21[128];
	float 	fRec22[3];
	int 	iVec22[2];
	float 	fRec25[2];
	float 	fVec23[128];
	float 	fRec24[3];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("name", "Kisana");
		m->declare("author", "Yann Orlarey");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/version", "1.0");
		m->declare("music.lib/license", "LGPL with exception");
	}

	virtual int getNumInputs() 	{ return 2; }
	virtual int getNumOutputs() 	{ return 2; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fslider0 = 0.0f;
		fslider1 = 0.0f;
		for (int i=0; i<2; i++) iRec1[i] = 0;
		SIG0 sig0;
		sig0.init(samplingFreq);
		sig0.fill(16,ftbl0);
		iConst0 = int((0.16666666666666666f * min(192000, max(1, fSamplingFreq))));
		for (int i=0; i<2; i++) iRec3[i] = 0;
		for (int i=0; i<2; i++) iVec0[i] = 0;
		fslider2 = 0.0f;
		for (int i=0; i<2; i++) iVec1[i] = 0;
		for (int i=0; i<2; i++) iRec4[i] = 0;
		for (int i=0; i<2; i++) iRec5[i] = 0;
		for (int i=0; i<2; i++) iVec2[i] = 0;
		for (int i=0; i<2; i++) fRec2[i] = 0;
		IOTA = 0;
		for (int i=0; i<512; i++) fVec3[i] = 0;
		for (int i=0; i<3; i++) fRec0[i] = 0;
		for (int i=0; i<2; i++) iVec4[i] = 0;
		for (int i=0; i<2; i++) fRec7[i] = 0;
		for (int i=0; i<512; i++) fVec5[i] = 0;
		for (int i=0; i<3; i++) fRec6[i] = 0;
		for (int i=0; i<2; i++) iVec6[i] = 0;
		for (int i=0; i<2; i++) fRec9[i] = 0;
		for (int i=0; i<512; i++) fVec7[i] = 0;
		for (int i=0; i<3; i++) fRec8[i] = 0;
		for (int i=0; i<2; i++) iVec8[i] = 0;
		for (int i=0; i<2; i++) fRec11[i] = 0;
		for (int i=0; i<256; i++) fVec9[i] = 0;
		for (int i=0; i<3; i++) fRec10[i] = 0;
		for (int i=0; i<2; i++) iVec10[i] = 0;
		for (int i=0; i<2; i++) fRec13[i] = 0;
		for (int i=0; i<256; i++) fVec11[i] = 0;
		for (int i=0; i<3; i++) fRec12[i] = 0;
		for (int i=0; i<2; i++) iVec12[i] = 0;
		for (int i=0; i<2; i++) fRec15[i] = 0;
		for (int i=0; i<256; i++) fVec13[i] = 0;
		for (int i=0; i<3; i++) fRec14[i] = 0;
		for (int i=0; i<2; i++) iVec14[i] = 0;
		for (int i=0; i<2; i++) fRec17[i] = 0;
		for (int i=0; i<256; i++) fVec15[i] = 0;
		for (int i=0; i<3; i++) fRec16[i] = 0;
		for (int i=0; i<2; i++) iVec16[i] = 0;
		for (int i=0; i<2; i++) fRec19[i] = 0;
		for (int i=0; i<256; i++) fVec17[i] = 0;
		for (int i=0; i<3; i++) fRec18[i] = 0;
		for (int i=0; i<2; i++) iVec18[i] = 0;
		for (int i=0; i<2; i++) fRec21[i] = 0;
		for (int i=0; i<128; i++) fVec19[i] = 0;
		for (int i=0; i<3; i++) fRec20[i] = 0;
		for (int i=0; i<2; i++) iVec20[i] = 0;
		for (int i=0; i<2; i++) fRec23[i] = 0;
		for (int i=0; i<128; i++) fVec21[i] = 0;
		for (int i=0; i<3; i++) fRec22[i] = 0;
		for (int i=0; i<2; i++) iVec22[i] = 0;
		for (int i=0; i<2; i++) fRec25[i] = 0;
		for (int i=0; i<128; i++) fVec23[i] = 0;
		for (int i=0; i<3; i++) fRec24[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("FstHarpeAuto");
		interface->openVerticalBox("loop48");
		interface->declare(&fslider2, "1", "");
		interface->declare(&fslider2, "OWL", "PARAMETER_B");
		interface->addHorizontalSlider("note", &fslider2, 0.0f, 0.0f, 11.0f, 1.0f);
		interface->closeBox();
		interface->declare(&fslider0, "OWL", "PARAMETER_A");
		interface->addHorizontalSlider("master", &fslider0, 0.0f, 0.0f, 1.0f, 0.01f);
		interface->declare(&fslider1, "OWL", "PARAMETER_C");
		interface->addHorizontalSlider("timbre", &fslider1, 0.0f, 0.0f, 1.0f, 0.01f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = float(fslider0);
		float 	fSlow1 = float(fslider1);
		float 	fSlow2 = (1 + fSlow1);
		float 	fSlow3 = (1 - fSlow1);
		int 	iSlow4 = int(float(fslider2));
		int 	iSlow5 = (iSlow4 <= 0.0f);
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* input1 = input[1];
		FAUSTFLOAT* output0 = output[0];
		FAUSTFLOAT* output1 = output[1];
		for (int i=0; i<count; i++) {
			float fTemp0 = (float)input0[i];
			float fTemp1 = (float)input1[i];
			iRec1[0] = (12345 + (1103515245 * iRec1[1]));
			iRec3[0] = ((1 + iRec3[1]) % iConst0);
			int iTemp2 = int((iRec3[0] == 0));
			iVec0[0] = iTemp2;
			iVec1[0] = iSlow4;
			iRec4[0] = ((int(iVec0[1]))?0:(iRec4[1] + abs((iSlow4 - iVec1[1]))));
			iRec5[0] = ((iVec0[0] + iRec5[1]) % 15);
			ftbl0[((int((iVec0[0] & ((iRec4[0] > 0) | iSlow5))))?iRec5[0]:15)] = iSlow4;
			float fTemp3 = ftbl0[iRec5[0]];
			int iTemp4 = (fabsf((fTemp3 - 1)) < 0.5f);
			iVec2[0] = iTemp4;
			fRec2[0] = ((fRec2[1] + ((iVec2[0] - iVec2[1]) > 0.0f)) - (0.00296627625057368f * (fRec2[1] > 0.0f)));
			fVec3[IOTA&511] = ((0.4934425764844625f * ((fSlow2 * fRec0[1]) + (fSlow3 * fRec0[2]))) + (4.656612875245797e-10f * (iRec1[0] * (fRec2[0] > 0.0f))));
			fRec0[0] = fVec3[(IOTA-336)&511];
			int iTemp5 = (fabsf((fTemp3 - 2)) < 0.5f);
			iVec4[0] = iTemp5;
			fRec7[0] = ((fRec7[1] + ((iVec4[0] - iVec4[1]) > 0.0f)) - (0.0033295325160703805f * (fRec7[1] > 0.0f)));
			fVec5[IOTA&511] = ((0.4941537998866976f * ((fSlow2 * fRec6[1]) + (fSlow3 * fRec6[2]))) + (4.656612875245797e-10f * (iRec1[0] * (fRec7[0] > 0.0f))));
			fRec6[0] = fVec5[(IOTA-299)&511];
			int iTemp6 = (fabsf((fTemp3 - 3)) < 0.5f);
			iVec6[0] = iTemp6;
			fRec9[0] = ((fRec9[1] + ((iVec6[0] - iVec6[1]) > 0.0f)) - (0.00373727388790102f * (fRec9[1] > 0.0f)));
			fVec7[IOTA&511] = ((0.4947882913184981f * ((fSlow2 * fRec8[1]) + (fSlow3 * fRec8[2]))) + (4.656612875245797e-10f * (iRec1[0] * (fRec9[0] > 0.0f))));
			fRec8[0] = fVec7[(IOTA-266)&511];
			int iTemp7 = (fabsf((fTemp3 - 4)) < 0.5f);
			iVec8[0] = iTemp7;
			fRec11[0] = ((fRec11[1] + ((iVec8[0] - iVec8[1]) > 0.0f)) - (0.004444392698205774f * (fRec11[1] > 0.0f)));
			fVec9[IOTA&255] = ((0.49561384415280396f * ((fSlow2 * fRec10[1]) + (fSlow3 * fRec10[2]))) + (4.656612875245797e-10f * (iRec1[0] * (fRec11[0] > 0.0f))));
			fRec10[0] = fVec9[(IOTA-224)&255];
			int iTemp8 = (fabsf((fTemp3 - 5)) < 0.5f);
			iVec10[0] = iTemp8;
			fRec13[0] = ((fRec13[1] + ((iVec10[0] - iVec10[1]) > 0.0f)) - (0.004988662131519274f * (fRec13[1] > 0.0f)));
			fVec11[IOTA&255] = ((0.49609050335141536f * ((fSlow2 * fRec12[1]) + (fSlow3 * fRec12[2]))) + (4.656612875245797e-10f * (iRec1[0] * (fRec13[0] > 0.0f))));
			fRec12[0] = fVec11[(IOTA-199)&255];
			int iTemp9 = (fabsf((fTemp3 - 6)) < 0.5f);
			iVec12[0] = iTemp9;
			fRec15[0] = ((fRec15[1] + ((iVec12[0] - iVec12[1]) > 0.0f)) - (0.00593255250114736f * (fRec15[1] > 0.0f)));
			fVec13[IOTA&255] = ((0.4967104672162962f * ((fSlow2 * fRec14[1]) + (fSlow3 * fRec14[2]))) + (4.656612875245797e-10f * (iRec1[0] * (fRec15[0] > 0.0f))));
			fRec14[0] = fVec13[(IOTA-167)&255];
			float fTemp10 = (0.7071067811865476f * fRec14[0]);
			int iTemp11 = (fabsf((fTemp3 - 7)) < 0.5f);
			iVec14[0] = iTemp11;
			fRec17[0] = ((fRec17[1] + ((iVec14[0] - iVec14[1]) > 0.0f)) - (0.006659065032140761f * (fRec17[1] > 0.0f)));
			fVec15[IOTA&255] = ((0.49706830510841143f * ((fSlow2 * fRec16[1]) + (fSlow3 * fRec16[2]))) + (4.656612875245797e-10f * (iRec1[0] * (fRec17[0] > 0.0f))));
			fRec16[0] = fVec15[(IOTA-149)&255];
			int iTemp12 = (fabsf((fTemp3 - 8)) < 0.5f);
			iVec16[0] = iTemp12;
			fRec19[0] = ((fRec19[1] + ((iVec16[0] - iVec16[1]) > 0.0f)) - (0.00747454777580204f * (fRec19[1] > 0.0f)));
			fVec17[IOTA&255] = ((0.49738731956016835f * ((fSlow2 * fRec18[1]) + (fSlow3 * fRec18[2]))) + (4.656612875245797e-10f * (iRec1[0] * (fRec19[0] > 0.0f))));
			fRec18[0] = fVec17[(IOTA-132)&255];
			int iTemp13 = (fabsf((fTemp3 - 9)) < 0.5f);
			iVec18[0] = iTemp13;
			fRec21[0] = ((fRec21[1] + ((iVec18[0] - iVec18[1]) > 0.0f)) - (0.008888785396411547f * (fRec21[1] > 0.0f)));
			fVec19[IOTA&127] = ((0.4978020912736325f * ((fSlow2 * fRec20[1]) + (fSlow3 * fRec20[2]))) + (4.656612875245797e-10f * (iRec1[0] * (fRec21[0] > 0.0f))));
			fRec20[0] = fVec19[(IOTA-111)&127];
			int iTemp14 = (fabsf((fTemp3 - 10)) < 0.5f);
			iVec20[0] = iTemp14;
			fRec23[0] = ((fRec23[1] + ((iVec20[0] - iVec20[1]) > 0.0f)) - (0.009977324263038548f * (fRec23[1] > 0.0f)));
			fVec21[IOTA&127] = ((0.4980414156229456f * ((fSlow2 * fRec22[1]) + (fSlow3 * fRec22[2]))) + (4.656612875245797e-10f * (iRec1[0] * (fRec23[0] > 0.0f))));
			fRec22[0] = fVec21[(IOTA-99)&127];
			int iTemp15 = (fabsf((fTemp3 - 11)) < 0.5f);
			iVec22[0] = iTemp15;
			fRec25[0] = ((fRec25[1] + ((iVec22[0] - iVec22[1]) > 0.0f)) - (0.01186510500229472f * (fRec25[1] > 0.0f)));
			fVec23[IOTA&127] = ((0.498352519415873f * ((fSlow2 * fRec24[1]) + (fSlow3 * fRec24[2]))) + (4.656612875245797e-10f * (iRec1[0] * (fRec25[0] > 0.0f))));
			fRec24[0] = fVec23[(IOTA-83)&127];
			output0[i] = (FAUSTFLOAT)(fTemp0 + (fSlow0 * (((((((((((0.9770084209183945f * fRec0[0]) + (0.9293203772845852f * fRec6[0])) + (0.8790490729915326f * fRec8[0])) + (0.8257228238447705f * fRec10[0])) + (0.7687061147858073f * fRec12[0])) + fTemp10) + (0.6396021490668313f * fRec16[0])) + (0.5640760748177662f * fRec18[0])) + (0.4767312946227962f * fRec20[0])) + (0.3692744729379982f * fRec22[0])) + (0.21320071635561033f * fRec24[0]))));
			output1[i] = (FAUSTFLOAT)(fTemp1 + (fSlow0 * ((((((0.7687061147858074f * fRec16[0]) + (fTemp10 + (((((0.21320071635561044f * fRec0[0]) + (0.3692744729379982f * fRec6[0])) + (0.4767312946227962f * fRec8[0])) + (0.5640760748177662f * fRec10[0])) + (0.6396021490668313f * fRec12[0])))) + (0.8257228238447705f * fRec18[0])) + (0.8790490729915326f * fRec20[0])) + (0.9293203772845852f * fRec22[0])) + (0.9770084209183945f * fRec24[0]))));
			// post processing
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
			fRec0[2] = fRec0[1]; fRec0[1] = fRec0[0];
			IOTA = IOTA+1;
			fRec2[1] = fRec2[0];
			iVec2[1] = iVec2[0];
			iRec5[1] = iRec5[0];
			iRec4[1] = iRec4[0];
			iVec1[1] = iVec1[0];
			iVec0[1] = iVec0[0];
			iRec3[1] = iRec3[0];
			iRec1[1] = iRec1[0];
		}
	}
};



/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/



/**************************************************************************************

	FstHarpeAutoPatch : an OWL patch that calls Faust generated DSP code
	
***************************************************************************************/

class FstHarpeAutoPatch : public Patch
{
    FstHarpeAuto   fDSP;
    OwlUI	fUI;
    
public:

    FstHarpeAutoPatch() : fUI(patches.getCurrentPatchProcessor())
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

#endif // __FstHarpeAutoPatch_h__


////////////////////////////////////////////////////////////////////////////////////////////////////
