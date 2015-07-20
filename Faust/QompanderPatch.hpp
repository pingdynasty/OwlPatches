/* ------------------------------------------------------------
author: "Bart Brouns"
copyright: "(c) Bart Brouns 2014"
license: "GNU 3.0"
name: "qompander"
version: "1.2"
Code generated with Faust 2.0.a34 (http://faust.grame.fr)
------------------------------------------------------------ */

#ifndef  __Qompander_H__
#define  __Qompander_H__
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

#ifndef __QompanderPatch_h__
#define __QompanderPatch_h__

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

float faustpower2_f(float value) {
	return (value * value);
	
}

#ifndef FAUSTCLASS 
#define FAUSTCLASS Qompander
#endif

class Qompander : public dsp {
	
  private:
	
	float fRec7[3];
	float fRec6[3];
	float fRec5[3];
	float fRec4[3];
	float fRec11[3];
	float fRec10[3];
	float fRec9[3];
	float fRec8[3];
	float fVec0[2];
	float fRec0[2];
	float fRec2[2];
	float fRec12[2];
	float fRec3[2];
	float fRec1[2];
	float fRec13[2];
	FAUSTFLOAT fHslider0;
	int fSamplingFreq;
	float fConst0;
	FAUSTFLOAT fHslider1;
	FAUSTFLOAT fHslider2;
	FAUSTFLOAT fHslider3;
	
  public:
	
	void static metadata(Meta* m) { 
		m->declare("additional", "filter coefficients by Olli Niemitalo");
		m->declare("author", "Bart Brouns");
		m->declare("copyright", "(c) Bart Brouns 2014");
		m->declare("credits", "ported from qompander in pd by Katja Vetter");
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
		m->declare("license", "GNU 3.0");
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
		m->declare("name", "qompander");
		m->declare("see", "http://www.katjaas.nl/compander/compander.html");
		m->declare("version", "1.2");
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
		for (int i0 = 0; (i0 < 2); i0 = (i0 + 1)) {
			fVec0[i0] = 0.f;
			
		}
		fHslider0 = FAUSTFLOAT(3.);
		for (int i1 = 0; (i1 < 2); i1 = (i1 + 1)) {
			fRec0[i1] = 0.f;
			
		}
		fConst0 = (1000.f / float(min(192000, max(1, fSamplingFreq))));
		fHslider1 = FAUSTFLOAT(1.);
		for (int i2 = 0; (i2 < 2); i2 = (i2 + 1)) {
			fRec2[i2] = 0.f;
			
		}
		for (int i3 = 0; (i3 < 3); i3 = (i3 + 1)) {
			fRec7[i3] = 0.f;
			
		}
		for (int i4 = 0; (i4 < 3); i4 = (i4 + 1)) {
			fRec6[i4] = 0.f;
			
		}
		for (int i5 = 0; (i5 < 3); i5 = (i5 + 1)) {
			fRec5[i5] = 0.f;
			
		}
		for (int i6 = 0; (i6 < 3); i6 = (i6 + 1)) {
			fRec4[i6] = 0.f;
			
		}
		for (int i7 = 0; (i7 < 3); i7 = (i7 + 1)) {
			fRec11[i7] = 0.f;
			
		}
		for (int i8 = 0; (i8 < 3); i8 = (i8 + 1)) {
			fRec10[i8] = 0.f;
			
		}
		for (int i9 = 0; (i9 < 3); i9 = (i9 + 1)) {
			fRec9[i9] = 0.f;
			
		}
		for (int i10 = 0; (i10 < 3); i10 = (i10 + 1)) {
			fRec8[i10] = 0.f;
			
		}
		fHslider2 = FAUSTFLOAT(20.);
		for (int i11 = 0; (i11 < 2); i11 = (i11 + 1)) {
			fRec12[i11] = 0.f;
			
		}
		for (int i12 = 0; (i12 < 2); i12 = (i12 + 1)) {
			fRec3[i12] = 0.f;
			
		}
		for (int i13 = 0; (i13 < 2); i13 = (i13 + 1)) {
			fRec1[i13] = 0.f;
			
		}
		fHslider3 = FAUSTFLOAT(-40.);
		for (int i14 = 0; (i14 < 2); i14 = (i14 + 1)) {
			fRec13[i14] = 0.f;
			
		}
		
	}
	
	virtual void init(int samplingFreq) {
		classInit(samplingFreq);
		instanceInit(samplingFreq);
	}
	
	virtual void buildUserInterface(UI* interface) {
		interface->declare(0, "0", "");
		interface->declare(0, "tooltip", "Reference: http://www.katjaas.nl/compander/compander.html");
		interface->openVerticalBox("qompander");
		interface->declare(&fHslider0, "0", "");
		interface->declare(&fHslider0, "OWL", "PARAMETER_A");
		interface->declare(&fHslider0, "style", "knob");
		interface->declare(&fHslider0, "unit", ": 1");
		interface->addHorizontalSlider("Factor", &fHslider0, 3.f, 0.8f, 8.f, 0.01f);
		interface->declare(&fHslider3, "1", "");
		interface->declare(&fHslider3, "OWL", "PARAMETER_B");
		interface->declare(&fHslider3, "style", "knob");
		interface->declare(&fHslider3, "unit", "dB");
		interface->addHorizontalSlider("Threshold", &fHslider3, -40.f, -96.f, -20.f, 0.01f);
		interface->declare(&fHslider1, "2", "");
		interface->declare(&fHslider1, "OWL", "PARAMETER_C");
		interface->declare(&fHslider1, "style", "knob");
		interface->declare(&fHslider1, "unit", "ms");
		interface->addHorizontalSlider("Attack", &fHslider1, 1.f, 1.f, 20.f, 0.01f);
		interface->declare(&fHslider2, "3", "");
		interface->declare(&fHslider2, "OWL", "PARAMETER_D");
		interface->declare(&fHslider2, "style", "knob");
		interface->declare(&fHslider2, "unit", "ms");
		interface->addHorizontalSlider("Release", &fHslider2, 20.f, 20.f, 1000.f, 0.01f);
		interface->closeBox();
		
	}
	
	virtual void compute(int count, FAUSTFLOAT** inputs, FAUSTFLOAT** outputs) {
		FAUSTFLOAT* input0 = inputs[0];
		FAUSTFLOAT* output0 = outputs[0];
		float fSlow0 = (0.001f * float(fHslider0));
		float fSlow1 = (0.001f * float(fHslider1));
		float fSlow2 = (0.001f * float(fHslider2));
		float fSlow3 = (0.001f * float(fHslider3));
		for (int i = 0; (i < count); i = (i + 1)) {
			float fTemp0 = float(input0[i]);
			fVec0[0] = fTemp0;
			fRec0[0] = ((0.999f * fRec0[1]) + fSlow0);
			fRec2[0] = ((0.999f * fRec2[1]) + fSlow1);
			float fTemp1 = expf((0.f - (fConst0 / fRec2[0])));
			fRec7[0] = ((0.161758f * fRec7[2]) + fTemp0);
			fRec6[0] = (((0.161758f * fRec7[0]) + (0.733029f * fRec6[2])) - fRec7[2]);
			fRec5[0] = (((0.733029f * fRec6[0]) + (0.94535f * fRec5[2])) - fRec6[2]);
			fRec4[0] = (((0.94535f * fRec5[0]) + (0.990598f * fRec4[2])) - fRec5[2]);
			float fTemp2 = ((0.990598f * fRec4[0]) - fRec4[2]);
			fRec11[0] = (fVec0[1] + (0.479401f * fRec11[2]));
			fRec10[0] = (((0.479401f * fRec11[0]) + (0.876218f * fRec10[2])) - fRec11[2]);
			fRec9[0] = (((0.876218f * fRec10[0]) + (0.976599f * fRec9[2])) - fRec10[2]);
			fRec8[0] = (((0.976599f * fRec9[0]) + (0.9975f * fRec8[2])) - fRec9[2]);
			float fTemp3 = ((0.9975f * fRec8[0]) - fRec8[2]);
			float fTemp4 = fabsf(min(100.f, max(1e-05f, sqrtf((faustpower2_f(fTemp2) + faustpower2_f(fTemp3))))));
			fRec12[0] = ((0.999f * fRec12[1]) + fSlow2);
			float fTemp5 = expf((0.f - (fConst0 / fRec12[0])));
			fRec3[0] = max(fTemp4, ((fRec3[1] * fTemp5) + (fTemp4 * (1.f - fTemp5))));
			fRec1[0] = ((fRec1[1] * fTemp1) + (fRec3[0] * (1.f - fTemp1)));
			fRec13[0] = ((0.999f * fRec13[1]) + fSlow3);
			float fTemp6 = powf(10.f, (0.05f * fRec13[0]));
			output0[i] = FAUSTFLOAT((0.707107f * ((powf(min(1.f, max(1e-07f, sinf((1.5708f * (fRec0[0] * min((1.f / fRec0[0]), fRec1[0])))))), (logf(fTemp6) / logf(sinf((1.5708f * (fRec0[0] * fTemp6)))))) * (fTemp2 + fTemp3)) / fRec1[0])));
			fVec0[1] = fVec0[0];
			fRec0[1] = fRec0[0];
			fRec2[1] = fRec2[0];
			fRec7[2] = fRec7[1];
			fRec7[1] = fRec7[0];
			fRec6[2] = fRec6[1];
			fRec6[1] = fRec6[0];
			fRec5[2] = fRec5[1];
			fRec5[1] = fRec5[0];
			fRec4[2] = fRec4[1];
			fRec4[1] = fRec4[0];
			fRec11[2] = fRec11[1];
			fRec11[1] = fRec11[0];
			fRec10[2] = fRec10[1];
			fRec10[1] = fRec10[0];
			fRec9[2] = fRec9[1];
			fRec9[1] = fRec9[0];
			fRec8[2] = fRec8[1];
			fRec8[1] = fRec8[0];
			fRec12[1] = fRec12[0];
			fRec3[1] = fRec3[0];
			fRec1[1] = fRec1[0];
			fRec13[1] = fRec13[0];
			
		}
		
	}

	
};


/***************************END USER SECTION ***************************/

/*******************BEGIN ARCHITECTURE SECTION (part 2/2)***************/



/**************************************************************************************

	QompanderPatch : an OWL patch that calls Faust generated DSP code
	
***************************************************************************************/

class QompanderPatch : public Patch
{
    Qompander   fDSP;
    OwlUI	fUI;
    
public:

    QompanderPatch() : fUI(this)
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

#endif // __QompanderPatch_h__


////////////////////////////////////////////////////////////////////////////////////////////////////

#endif
