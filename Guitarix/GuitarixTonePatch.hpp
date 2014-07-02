//-----------------------------------------------------
// version: "0.01"
// author: "brummer"
// license: "BSD"
// copyright: "(c)brummer 2008"
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

#ifndef __GuitarixTonePatch_h__
#define __GuitarixTonePatch_h__

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
#define FAUSTCLASS GuitarixTone
#endif

class GuitarixTone : public dsp {
  private:
	FAUSTFLOAT 	fslider0;
	int 	iConst0;
	float 	fConst1;
	float 	fConst2;
	float 	fConst3;
	FAUSTFLOAT 	fslider1;
	float 	fConst4;
	float 	fConst5;
	float 	fConst6;
	float 	fVec0[3];
	float 	fRec3[3];
	float 	fRec2[3];
	float 	fRec1[3];
	FAUSTFLOAT 	fslider2;
	float 	fRec0[3];
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("id", "amp.tone");
		m->declare("version", "0.01");
		m->declare("author", "brummer");
		m->declare("license", "BSD");
		m->declare("copyright", "(c)brummer 2008");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/version", "1.0");
		m->declare("music.lib/license", "LGPL with exception");
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
	}

	virtual int getNumInputs() 	{ return 1; }
	virtual int getNumOutputs() 	{ return 1; }
	static void classInit(int samplingFreq) {
	}
	virtual void instanceInit(int samplingFreq) {
		fSamplingFreq = samplingFreq;
		fslider0 = 0.0f;
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (15079.644737231007f / float(iConst0));
		fConst2 = (1.4142135623730951f * sinf(fConst1));
		fConst3 = cosf(fConst1);
		fslider1 = 0.0f;
		fConst4 = (3769.9111843077517f / float(iConst0));
		fConst5 = (1.4142135623730951f * sinf(fConst4));
		fConst6 = cosf(fConst4);
		for (int i=0; i<3; i++) fVec0[i] = 0;
		for (int i=0; i<3; i++) fRec3[i] = 0;
		for (int i=0; i<3; i++) fRec2[i] = 0;
		for (int i=0; i<3; i++) fRec1[i] = 0;
		fslider2 = 0.0f;
		for (int i=0; i<3; i++) fRec0[i] = 0;
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("GuitarixTone");
		interface->declare(&fslider1, "OWL", "PARAMETER_A");
		interface->declare(&fslider1, "name", "bass");
		interface->declare(&fslider1, "style", "knob");
		interface->addVerticalSlider("Bass", &fslider1, 0.0f, -2e+01f, 2e+01f, 0.1f);
		interface->declare(&fslider0, "OWL", "PARAMETER_B");
		interface->declare(&fslider0, "name", "middle");
		interface->declare(&fslider0, "style", "knob");
		interface->addVerticalSlider("Middle", &fslider0, 0.0f, -2e+01f, 2e+01f, 0.1f);
		interface->declare(&fslider2, "OWL", "PARAMETER_C");
		interface->declare(&fslider2, "name", "treble");
		interface->declare(&fslider2, "style", "knob");
		interface->addVerticalSlider("Treble", &fslider2, 0.0f, -2e+01f, 2e+01f, 0.1f);
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		float 	fSlow0 = float(fslider0);
		float 	fSlow1 = powf(10,(0.0125f * fSlow0));
		float 	fSlow2 = sqrtf(fSlow1);
		float 	fSlow3 = (fConst2 * fSlow2);
		float 	fSlow4 = (fSlow1 - 1);
		float 	fSlow5 = (fConst3 * fSlow4);
		float 	fSlow6 = (fSlow1 + fSlow5);
		float 	fSlow7 = ((1 + fSlow6) - fSlow3);
		float 	fSlow8 = (1 + fSlow1);
		float 	fSlow9 = (fConst3 * fSlow8);
		float 	fSlow10 = (0 - (2 * ((fSlow1 + fSlow9) - 1)));
		float 	fSlow11 = (0.5f * fSlow0);
		float 	fSlow12 = powf(10,(0.025f * (float(fslider1) - fSlow11)));
		float 	fSlow13 = (fConst5 * sqrtf(fSlow12));
		float 	fSlow14 = (fConst6 * (fSlow12 - 1));
		float 	fSlow15 = (fSlow12 + fSlow14);
		float 	fSlow16 = ((1 + fSlow15) - fSlow13);
		float 	fSlow17 = (1 + fSlow12);
		float 	fSlow18 = (fConst6 * fSlow17);
		float 	fSlow19 = (0 - (2 * ((fSlow12 + fSlow18) - 1)));
		float 	fSlow20 = (fSlow17 - (fSlow14 + fSlow13));
		float 	fSlow21 = (2 * (0 - ((1 + fSlow18) - fSlow12)));
		float 	fSlow22 = ((1 + (fSlow12 + fSlow13)) - fSlow14);
		float 	fSlow23 = (1.0f / (1 + (fSlow13 + fSlow15)));
		float 	fSlow24 = (fSlow8 - (fSlow5 + fSlow3));
		float 	fSlow25 = (2 * (0 - ((1 + fSlow9) - fSlow1)));
		float 	fSlow26 = ((1 + (fSlow1 + fSlow3)) - fSlow5);
		float 	fSlow27 = (1.0f / (1 + (fSlow3 + fSlow6)));
		float 	fSlow28 = (fConst5 * fSlow2);
		float 	fSlow29 = (fConst6 * fSlow4);
		float 	fSlow30 = (fSlow1 + fSlow29);
		float 	fSlow31 = (fSlow1 * ((1 + fSlow30) - fSlow28));
		float 	fSlow32 = (fConst6 * fSlow8);
		float 	fSlow33 = ((0 - (2 * fSlow1)) * ((fSlow1 + fSlow32) - 1));
		float 	fSlow34 = (fSlow1 * (1 + (fSlow30 + fSlow28)));
		float 	fSlow35 = (2 * (0 - ((1 + fSlow32) - fSlow1)));
		float 	fSlow36 = (fSlow8 - (fSlow29 + fSlow28));
		float 	fSlow37 = (1.0f / ((1 + (fSlow1 + fSlow28)) - fSlow29));
		float 	fSlow38 = powf(10,(0.025f * (float(fslider2) - fSlow11)));
		float 	fSlow39 = (fConst2 * sqrtf(fSlow38));
		float 	fSlow40 = (fConst3 * (fSlow38 - 1));
		float 	fSlow41 = (fSlow38 + fSlow40);
		float 	fSlow42 = (fSlow38 * ((1 + fSlow41) - fSlow39));
		float 	fSlow43 = (1 + fSlow38);
		float 	fSlow44 = (fConst3 * fSlow43);
		float 	fSlow45 = ((0 - (2 * fSlow38)) * ((fSlow38 + fSlow44) - 1));
		float 	fSlow46 = (fSlow38 * (1 + (fSlow41 + fSlow39)));
		float 	fSlow47 = (2 * (0 - ((1 + fSlow44) - fSlow38)));
		float 	fSlow48 = (fSlow43 - (fSlow40 + fSlow39));
		float 	fSlow49 = (1.0f / ((1 + (fSlow38 + fSlow39)) - fSlow40));
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* output0 = output[0];
		for (int i=0; i<count; i++) {
			float fTemp0 = (float)input0[i];
			float fTemp1 = (fTemp0 + 1e-20f);
			fVec0[0] = fTemp1;
			fRec3[0] = (fSlow23 * ((fSlow12 * (((fSlow22 * fVec0[0]) + (fSlow21 * fVec0[1])) + (fSlow20 * fVec0[2]))) - ((fSlow19 * fRec3[1]) + (fSlow16 * fRec3[2]))));
			fRec2[0] = (fSlow27 * ((fSlow1 * (((fSlow26 * fRec3[0]) + (fSlow25 * fRec3[1])) + (fSlow24 * fRec3[2]))) - ((fSlow10 * fRec2[1]) + (fSlow7 * fRec2[2]))));
			fRec1[0] = (fSlow37 * ((0 - ((fSlow36 * fRec1[2]) + (fSlow35 * fRec1[1]))) + (((fSlow34 * fRec2[0]) + (fSlow33 * fRec2[1])) + (fSlow31 * fRec2[2]))));
			fRec0[0] = (fSlow49 * ((0 - ((fSlow48 * fRec0[2]) + (fSlow47 * fRec0[1]))) + (((fSlow46 * fRec1[0]) + (fSlow45 * fRec1[1])) + (fSlow42 * fRec1[2]))));
			output0[i] = (FAUSTFLOAT)fRec0[0];
			// post processing
			fRec0[2] = fRec0[1]; fRec0[1] = fRec0[0];
			fRec1[2] = fRec1[1]; fRec1[1] = fRec1[0];
			fRec2[2] = fRec2[1]; fRec2[1] = fRec2[0];
			fRec3[2] = fRec3[1]; fRec3[1] = fRec3[0];
			fVec0[2] = fVec0[1]; fVec0[1] = fVec0[0];
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

    GuitarixTonePatch() : fUI(patches.getCurrentPatchProcessor())
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
