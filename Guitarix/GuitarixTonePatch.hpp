/* ------------------------------------------------------------
author: "brummer"
copyright: "(c)brummer 2008"
license: "BSD"
version: "0.01"
Code generated with Faust 2.0.a34 (http://faust.grame.fr)
------------------------------------------------------------ */

#ifndef  __GuitarixTone_H__
#define  __GuitarixTone_H__
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

#ifndef __GuitarixTonePatch_h__
#define __GuitarixTonePatch_h__

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
#define FAUSTCLASS GuitarixTone
#endif

class GuitarixTone : public dsp {
	
  private:
	
	float fVec0[3];
	float fRec3[3];
	float fRec2[3];
	float fRec1[3];
	float fRec0[3];
	FAUSTFLOAT fVslider0;
	FAUSTFLOAT fVslider1;
	int fSamplingFreq;
	int iConst0;
	float fConst1;
	float fConst2;
	float fConst3;
	float fConst4;
	float fConst5;
	float fConst6;
	FAUSTFLOAT fVslider2;
	
  public:
	
	void static metadata(Meta* m) { 
		m->declare("author", "brummer");
		m->declare("copyright", "(c)brummer 2008");
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
		m->declare("id", "amp.tone");
		m->declare("license", "BSD");
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
		m->declare("version", "0.01");
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
		fVslider0 = FAUSTFLOAT(0.);
		fVslider1 = FAUSTFLOAT(0.);
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (15079.6f / float(iConst0));
		fConst2 = (1.41421f * sinf(fConst1));
		fConst3 = cosf(fConst1);
		fConst4 = (3769.91f / float(iConst0));
		fConst5 = (1.41421f * sinf(fConst4));
		fConst6 = cosf(fConst4);
		fVslider2 = FAUSTFLOAT(0.);
		for (int i0 = 0; (i0 < 3); i0 = (i0 + 1)) {
			fVec0[i0] = 0.f;
			
		}
		for (int i1 = 0; (i1 < 3); i1 = (i1 + 1)) {
			fRec3[i1] = 0.f;
			
		}
		for (int i2 = 0; (i2 < 3); i2 = (i2 + 1)) {
			fRec2[i2] = 0.f;
			
		}
		for (int i3 = 0; (i3 < 3); i3 = (i3 + 1)) {
			fRec1[i3] = 0.f;
			
		}
		for (int i4 = 0; (i4 < 3); i4 = (i4 + 1)) {
			fRec0[i4] = 0.f;
			
		}
		
	}
	
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("0x00");
		interface->declare(&fVslider2, "OWL", "PARAMETER_A");
		interface->declare(&fVslider2, "name", "bass");
		interface->declare(&fVslider2, "style", "knob");
		interface->addVerticalSlider("Bass", &fVslider2, 0.f, -20.f, 20.f, 0.1f);
		interface->declare(&fVslider1, "OWL", "PARAMETER_B");
		interface->declare(&fVslider1, "name", "middle");
		interface->declare(&fVslider1, "style", "knob");
		interface->addVerticalSlider("Middle", &fVslider1, 0.f, -20.f, 20.f, 0.1f);
		interface->declare(&fVslider0, "OWL", "PARAMETER_C");
		interface->declare(&fVslider0, "name", "treble");
		interface->declare(&fVslider0, "style", "knob");
		interface->addVerticalSlider("Treble", &fVslider0, 0.f, -20.f, 20.f, 0.1f);
		interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = float(fVslider1);
		float fSlow1 = (0.5f * fSlow0);
		float fSlow2 = powf(10.f, (0.025f * (float(fVslider0) - fSlow1)));
		float fSlow3 = (fConst2 * sqrtf(fSlow2));
		float fSlow4 = (fConst3 * (fSlow2 - 1.f));
		float fSlow5 = (1.f / ((1.f + (fSlow2 + fSlow3)) - fSlow4));
		float fSlow6 = (1.f + fSlow2);
		float fSlow7 = (fSlow6 - (fSlow4 + fSlow3));
		float fSlow8 = (fConst3 * fSlow6);
		float fSlow9 = (2.f * (0.f - ((1.f + fSlow8) - fSlow2)));
		float fSlow10 = (fSlow2 + fSlow4);
		float fSlow11 = (fSlow2 * (1.f + (fSlow10 + fSlow3)));
		float fSlow12 = powf(10.f, (0.0125f * fSlow0));
		float fSlow13 = sqrtf(fSlow12);
		float fSlow14 = (fConst5 * fSlow13);
		float fSlow15 = (fSlow12 - 1.f);
		float fSlow16 = (fConst6 * fSlow15);
		float fSlow17 = (1.f / ((1.f + (fSlow12 + fSlow14)) - fSlow16));
		float fSlow18 = (1.f + fSlow12);
		float fSlow19 = (fSlow18 - (fSlow16 + fSlow14));
		float fSlow20 = (fConst6 * fSlow18);
		float fSlow21 = (2.f * (0.f - ((1.f + fSlow20) - fSlow12)));
		float fSlow22 = (fSlow12 + fSlow16);
		float fSlow23 = (fSlow12 * (1.f + (fSlow22 + fSlow14)));
		float fSlow24 = (fConst2 * fSlow13);
		float fSlow25 = (fConst3 * fSlow15);
		float fSlow26 = (fSlow12 + fSlow25);
		float fSlow27 = (1.f / (1.f + (fSlow24 + fSlow26)));
		float fSlow28 = ((1.f + (fSlow12 + fSlow24)) - fSlow25);
		float fSlow29 = powf(10.f, (0.025f * (float(fVslider2) - fSlow1)));
		float fSlow30 = (fConst5 * sqrtf(fSlow29));
		float fSlow31 = (fConst6 * (fSlow29 - 1.f));
		float fSlow32 = (fSlow29 + fSlow31);
		float fSlow33 = (1.f / (1.f + (fSlow30 + fSlow32)));
		float fSlow34 = ((1.f + (fSlow29 + fSlow30)) - fSlow31);
		float fSlow35 = (1.f + fSlow29);
		float fSlow36 = (fConst6 * fSlow35);
		float fSlow37 = (2.f * (0.f - ((1.f + fSlow36) - fSlow29)));
		float fSlow38 = (fSlow35 - (fSlow31 + fSlow30));
		float fSlow39 = (0.f - (2.f * ((fSlow29 + fSlow36) - 1.f)));
		float fSlow40 = ((1.f + fSlow32) - fSlow30);
		float fSlow41 = (fConst3 * fSlow18);
		float fSlow42 = (2.f * (0.f - ((1.f + fSlow41) - fSlow12)));
		float fSlow43 = (fSlow18 - (fSlow25 + fSlow24));
		float fSlow44 = (0.f - (2.f * ((fSlow12 + fSlow41) - 1.f)));
		float fSlow45 = ((1.f + fSlow26) - fSlow24);
		float fSlow46 = ((0.f - (2.f * fSlow12)) * ((fSlow12 + fSlow20) - 1.f));
		float fSlow47 = (fSlow12 * ((1.f + fSlow22) - fSlow14));
		float fSlow48 = ((0.f - (2.f * fSlow2)) * ((fSlow2 + fSlow8) - 1.f));
		float fSlow49 = (fSlow2 * ((1.f + fSlow10) - fSlow3));
		for (int i = 0; (i < count); i = (i + 1)) {
			float fTemp0 = float(input0[i]);
			float fTemp1 = (fTemp0 + 1e-20f);
			fVec0[0] = fTemp1;
			fRec3[0] = (fSlow33 * ((fSlow29 * (((fSlow34 * fTemp1) + (fSlow37 * fVec0[1])) + (fSlow38 * fVec0[2]))) - ((fSlow39 * fRec3[1]) + (fSlow40 * fRec3[2]))));
			fRec2[0] = (fSlow27 * ((fSlow12 * (((fSlow28 * fRec3[0]) + (fSlow42 * fRec3[1])) + (fSlow43 * fRec3[2]))) - ((fSlow44 * fRec2[1]) + (fSlow45 * fRec2[2]))));
			fRec1[0] = (fSlow17 * ((0.f - ((fSlow19 * fRec1[2]) + (fSlow21 * fRec1[1]))) + (((fSlow23 * fRec2[0]) + (fSlow46 * fRec2[1])) + (fSlow47 * fRec2[2]))));
			fRec0[0] = (fSlow5 * ((0.f - ((fSlow7 * fRec0[2]) + (fSlow9 * fRec0[1]))) + (((fSlow11 * fRec1[0]) + (fSlow48 * fRec1[1])) + (fSlow49 * fRec1[2]))));
			output0[i] = FAUSTFLOAT(fRec0[0]);
			fVec0[2] = fVec0[1];
			fVec0[1] = fVec0[0];
			fRec3[2] = fRec3[1];
			fRec3[1] = fRec3[0];
			fRec2[2] = fRec2[1];
			fRec2[1] = fRec2[0];
			fRec1[2] = fRec1[1];
			fRec1[1] = fRec1[0];
			fRec0[2] = fRec0[1];
			fRec0[1] = fRec0[0];
			
		}
		
	}

	
};


/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/



/**************************************************************************************

	GuitarixTonePatch : an OWL patch that calls Faust generated DSP code
	
***************************************************************************************/

class GuitarixTonePatch : public Patch
{
    GuitarixTone   fDSP;
    OwlUI	fUI;
    
public:

    GuitarixTonePatch() : fUI(this)
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

#endif // __GuitarixTonePatch_h__


////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
