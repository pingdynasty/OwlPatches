//-----------------------------------------------------
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

#ifndef __HighShelfPatch_h__
#define __HighShelfPatch_h__

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

typedef long double quad;

#ifndef FAUSTCLASS 
#define FAUSTCLASS HighShelf
#endif

class HighShelf : public dsp {
  private:
	int 	iVec0[2];
	int 	iConst0;
	float 	fConst1;
	float 	fConst2;
	float 	fConst3;
	float 	fConst4;
	float 	fConst5;
	float 	fConst6;
	float 	fConst7;
	float 	fConst8;
	float 	fConst9;
	float 	fRec1[2];
	float 	fRec0[3];
	float 	fConst10;
	float 	fConst11;
	float 	fConst12;
  public:
	static void metadata(Meta* m) 	{ 
		m->declare("id", "HighShelf");
		m->declare("maxmsp.lib/name", "MaxMSP compatibility Library");
		m->declare("maxmsp.lib/author", "GRAME");
		m->declare("maxmsp.lib/copyright", "GRAME");
		m->declare("maxmsp.lib/version", "1.1");
		m->declare("maxmsp.lib/license", "LGPL");
		m->declare("music.lib/author", "GRAME");
		m->declare("music.lib/name", "Music Library");
		m->declare("music.lib/copyright", "GRAME");
		m->declare("music.lib/version", "1.0");
		m->declare("music.lib/license", "LGPL with exception");
		m->declare("math.lib/name", "Math Library");
		m->declare("math.lib/author", "GRAME");
		m->declare("math.lib/copyright", "GRAME");
		m->declare("math.lib/version", "1.0");
		m->declare("math.lib/license", "LGPL with exception");
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
		for (int i=0; i<2; i++) iVec0[i] = 0;
		iConst0 = min(192000, max(1, fSamplingFreq));
		fConst1 = (6.283185307179586f * (max((float)0, ((0.5f * iConst0) - 1e+02f)) / float(iConst0)));
		fConst2 = cosf(fConst1);
		fConst3 = (1.316227766016838f * fConst2);
		fConst4 = (2 * (0 - (0.683772233983162f + fConst3)));
		fConst5 = (0.005623413251903491f * sinf(fConst1));
		fConst6 = (0.683772233983162f * fConst2);
		fConst7 = ((1.316227766016838f + fConst6) - fConst5);
		fConst8 = (fConst6 + fConst5);
		fConst9 = (1.0f / (1.316227766016838f + fConst8));
		for (int i=0; i<2; i++) fRec1[i] = 0;
		for (int i=0; i<3; i++) fRec0[i] = 0;
		fConst10 = (0.31622776601683794f * (1.316227766016838f - fConst8));
		fConst11 = (0.31622776601683794f * ((1.316227766016838f + fConst5) - fConst6));
		fConst12 = (0 - (0.6324555320336759f * (fConst3 - 0.683772233983162f)));
	}
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	virtual void buildUserInterface(UI* interface) {
		interface->openVerticalBox("HighShelf");
		interface->closeBox();
	}
	virtual void compute (int count, FAUSTFLOAT** input, FAUSTFLOAT** output) {
		FAUSTFLOAT* input0 = input[0];
		FAUSTFLOAT* output0 = output[0];
		for (int i=0; i<count; i++) {
			float fTemp0 = (float)input0[i];
			iVec0[0] = 1;
			fRec1[0] = ((1e-20f * (1 - iVec0[1])) - fRec1[1]);
			fRec0[0] = ((fTemp0 + fRec1[0]) - (fConst9 * ((fConst7 * fRec0[2]) + (fConst4 * fRec0[1]))));
			output0[i] = (FAUSTFLOAT)(fConst9 * (((fConst12 * fRec0[1]) + (fConst11 * fRec0[0])) + (fConst10 * fRec0[2])));
			// post processing
			fRec0[2] = fRec0[1]; fRec0[1] = fRec0[0];
			fRec1[1] = fRec1[0];
			iVec0[1] = iVec0[0];
		}
	}
};



/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/



/**************************************************************************************

	HighShelfPatch : an OWL patch that calls Faust generated DSP code
	
***************************************************************************************/

class HighShelfPatch : public Patch
{
    HighShelf   fDSP;
    OwlUI	fUI;
    
public:

    HighShelfPatch() : fUI(this)
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

#endif // __HighShelfPatch_h__


////////////////////////////////////////////////////////////////////////////////////////////////////
