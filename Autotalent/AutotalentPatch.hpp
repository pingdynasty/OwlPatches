/*
 
Autotalent An auto-tuning LADSPA plugin.

Free software by Thomas A. Baran.
http://web.mit.edu/tbaran/www/autotalent.html
VERSION 0.2
March 20, 2010

port to IPlug by oli larkin
http://www.olilarkin.co.uk
*/

/*
 
AutotalentPatch.hpp

port to OWLSim from oli larkin IPlug version
Created by Tom on 17/01/2014.
 
 what parameters are useful if you can only have four controls as on the OWL for the purposes of the theremin bollards project?
 - pitch change
 - scale selection
 - correction strength?
 -
 
*/

#ifndef _AutotalentPatch_hpp
#define _AutotalentPatch_hpp

#define L2SC (float)3.32192809488736218171

#include <stdlib.h>
#include "StompBox.h"
#include "fftsetup.h"

#ifndef PI
#define PI 3.141592653589793238
#endif

// don't need the following defines for OWL -- delete when finished

#define AT_TUNE 0
#define AT_FIXED 1
#define AT_PULL 2
#define AT_A 3
#define AT_Bb 4
#define AT_B 5
#define AT_C 6
#define AT_Db 7
#define AT_D 8
#define AT_Eb 9
#define AT_E 10
#define AT_F 11
#define AT_Gb 12
#define AT_G 13
#define AT_Ab 14
#define AT_AMOUNT 15
#define AT_SMOOTH 16
#define AT_SHIFT 17
#define AT_SCWARP 18
#define AT_LFOAMP 19    //not using the LFO functions for the OWL implementation
#define AT_LFORATE 20
#define AT_LFOSHAPE 21
#define AT_LFOSYMM 22
#define AT_LFOQUANT 23
#define AT_FCORR 24
#define AT_FWARP 25
#define AT_MIX 26

#define SQUARE 0
#define SINE 1
#define TRI 2


////////////////////////////////////////

/* Start of .cpp file */

////////////////////////////////////////


/************* this section for reference only - delete when finshed porting

//autotalent::autotalent(IPlugInstanceInfo instanceInfo)
//:	IPLUG_CTOR(27, kNumPrograms, instanceInfo), mfs(44100)
//{
//    
//	// Define parameter ranges, display units, labels.
//	//arguments are: name, defaultVal, minVal, maxVal, step, label
//	//GetParam(kGain)->InitDouble("Gain", 0.0, -70.0, 12.0, 0.1, "dB");
//	
//	init(mfs);
//	
//	GetParam(AT_TUNE)->InitDouble("Concert A ref", 440.0, 430.0, 450.0, 0.1, "Hz");
//	GetParam(AT_FIXED)->InitDouble("Fixed pitch", 0.0, -36, 36.0, 0.01, "semitones");
//	GetParam(AT_PULL)->InitDouble("Pull to fixed pitch", 0.0, 0., 1.0, 0.01, "");
//	
//	GetParam(AT_A)->InitEnum("A", 1, 3);
//	GetParam(AT_Bb)->InitEnum("Bb", 1, 3);
//	GetParam(AT_B)->InitEnum("B", 1, 3);
//	GetParam(AT_C)->InitEnum("C", 1, 3);
//	GetParam(AT_Db)->InitEnum("Db", 1, 3);
//	GetParam(AT_D)->InitEnum("D", 1, 3);
//	GetParam(AT_Eb)->InitEnum("Eb", 1, 3);
//	GetParam(AT_E)->InitEnum("E", 1, 3);
//	GetParam(AT_F)->InitEnum("F", 1, 3);
//	GetParam(AT_Gb)->InitEnum("Gb", 1, 3);
//	GetParam(AT_G)->InitEnum("G", 1, 3);
//	GetParam(AT_Ab)->InitEnum("Ab", 1, 3);
//	
//	for(int i=3; i<15;i++)
//	{
//		GetParam(i)->SetDisplayText(0, "out");
//		GetParam(i)->SetDisplayText(1, "in");
//		GetParam(i)->SetDisplayText(2, "snap");
//	}
//	
//	GetParam(AT_AMOUNT)->InitDouble("Correction strength", 0.0, 0., 1.0, 0.01, "");
//	GetParam(AT_SMOOTH)->InitDouble("Correction smoothness", 0.0, 0., 1.0, 0.01, "");
//	GetParam(AT_SHIFT)->InitDouble("Pitch shift", 0.0, -12.0, 12.0, 0.01, "semitones");
//	GetParam(AT_SCWARP)->InitInt("Output scale rotate", 0, -3, 3,  "steps");
//	GetParam(AT_LFOAMP)->InitDouble("LFO depth", 0.0, 0., 1.0, 0.01, "");
//	GetParam(AT_LFORATE)->InitDouble("LFO rate", 0.05, 0.01, 10.0, 0.0001, "Hz");
//	GetParam(AT_LFOSHAPE)->InitEnum("LFO shape", SINE, 3);
//	GetParam(AT_LFOSHAPE)->SetDisplayText(SQUARE, "square");
//	GetParam(AT_LFOSHAPE)->SetDisplayText(SINE, "sine");
//	GetParam(AT_LFOSHAPE)->SetDisplayText(TRI, "triangle");
//	GetParam(AT_LFOSYMM)->InitDouble("LFO symmetry", 0.0, -1.0, 1.0, 0.01, "");
//	GetParam(AT_LFOQUANT)->InitBool("LFO quantise", 0, "");
//	GetParam(AT_FCORR)->InitBool("Formant correction", 1, "");
//	GetParam(AT_FWARP)->InitDouble("Formant warp", 0.0, -1.0, 1.0, 0.01, "");
//	GetParam(AT_MIX)->InitDouble("Mix", 1., 0., 1., 0.01, "");
//	
//	
//	//MakePreset("preset 1", -5.0, 5.0, 17, kReversed);
//	MakeDefaultPreset("-", kNumPrograms);
//}
//
//autotalent::~autotalent()
//{
//	fft_des(mfmembvars);
//	free(mcbi);
//	free(mcbf);
//	free(mcbo);
//	free(mcbwindow);
//	free(mhannwindow);
//	free(macwinv);
//	free(mfrag);
//	free(mffttime);
//	free(mfftfreqre);
//	free(mfftfreqim);
//	free(mfk);
//	free(mfb);
//	free(mfc);
//	free(mfrb);
//	free(mfrc);
//	free(mfsmooth);
//	free(mfsig);
//	
//	for (int ti=0; ti<mford; ti++)
//	{
//		free(mfbuff[ti]);
//	}
//	
//	free(mfbuff);
//	free(mftvec);
//}


//void OnParamChange(int paramIdx)
//{
//	
//	switch (paramIdx)
//	{
//			
//		case AT_TUNE:
//			m_pfTune = GetParam(AT_TUNE)->Value();
//			break;
//		case AT_FIXED:
//			m_pfFixed = GetParam(AT_FIXED)->Value();
//			break;
//		case AT_PULL:
//			m_pfPull = GetParam(AT_PULL)->Value();
//			break;
//		case AT_A:
//			m_pfA = GetParam(AT_A)->Value() - 1;
//			break;
//		case AT_Bb:
//			m_pfBb = GetParam(AT_Bb)->Value() - 1;
//			break;
//		case AT_B:
//			m_pfB = GetParam(AT_B)->Value() - 1;
//			break;
//		case AT_C:
//			m_pfC = GetParam(AT_C)->Value() - 1;
//			break;
//		case AT_Db:
//			m_pfDb = GetParam(AT_Db)->Value() - 1;
//			break;
//		case AT_D:
//			m_pfD = GetParam(AT_D)->Value() - 1;
//			break;
//		case AT_Eb:
//			m_pfEb = GetParam(AT_Eb)->Value() - 1;
//			break;
//		case AT_E:
//			m_pfE = GetParam(AT_E)->Value() - 1;
//			break;
//		case AT_F:
//			m_pfF = GetParam(AT_F)->Value() - 1;
//			break;
//		case AT_Gb:
//			m_pfGb = GetParam(AT_Gb)->Value() - 1;
//			break;
//		case AT_G:
//			m_pfG = GetParam(AT_G)->Value() - 1;
//			break;
//		case AT_Ab:
//			m_pfAb = GetParam(AT_Ab)->Value() - 1;
//			break;
//		case AT_AMOUNT:
//			m_pfAmount = GetParam(AT_AMOUNT)->Value();
//			break;
//		case AT_SMOOTH:
//			m_pfSmooth = GetParam(AT_SMOOTH)->Value();
//			break;
//		case AT_SHIFT:
//			m_pfShift = GetParam(AT_SHIFT)->Value();
//			break;
//		case AT_SCWARP:
//			m_pfScwarp = GetParam(AT_SCWARP)->Value();
//			break;
//		case AT_LFOAMP:
//			m_pfLfoamp = GetParam(AT_LFOAMP)->Value();
//			break;
//		case AT_LFORATE:
//			m_pfLforate = GetParam(AT_LFORATE)->Value();
//			break;
//		case AT_LFOSHAPE:
//			m_pfLfoshape = GetParam(AT_LFOSHAPE)->Value() - 1;
//			break;
//		case AT_LFOSYMM:
//			m_pfLfosymm = GetParam(AT_LFOSYMM)->Value();
//			break;
//		case AT_LFOQUANT:
//			m_pfLfoquant = GetParam(AT_LFOQUANT)->Value();
//			break;
//		case AT_FCORR:
//			m_pfFcorr = GetParam(AT_FCORR)->Value();
//			break;
//		case AT_FWARP:
//			m_pfFwarp = GetParam(AT_FWARP)->Value();
//			break;
//		case AT_MIX:
//			m_pfMix = GetParam(AT_MIX)->Value();
//			break;
//			
//		default:
//			break;
//	}
//}

-- above section for reference only - delete when finished porting ***********************/




///////////////////////////////////////////////////////////////////////////////////////////////

class Autotalent
{
public:
    
    Autotalent(unsigned long SampleRate);
    ~Autotalent(){      //does this destructor get called?
        fft_des(mfmembvars);
    	free(mcbi);
    	free(mcbf);
    	free(mcbo);
    	free(mcbwindow);
    	free(mhannwindow);
    	free(macwinv);
    	free(mfrag);
    	free(mffttime);
    	free(mfftfreqre);
    	free(mfftfreqim);
//    	free(mfk);
//    	free(mfb);
//      free(mfc);
//    	free(mfrb);
//    	free(mfrc);
//    	free(mfsmooth);
//    	free(mfsig);
//        
//    	for (int ti=0; ti<mford; ti++)
//    	{
//    		free(mfbuff[ti]);
//    	}
//    	
//    	free(mfbuff);
//     	free(mftvec);

    };
    
    void init (unsigned long SampleRate, unsigned long mBufSize);
    void Reset(unsigned long SampleRate, unsigned long mBufSize);
    void processReplacing(float *inputBuffer, float *outputBuffer, int SampleCount);

    fft_vars* mfmembvars; // member variables for fft routine
    
    void setMix(float mix){
        
        m_pfMix = mix;
    }
    
    void pitchShift(float pitchshift){
        
        m_pfShift = pitchshift;
    }
    
    unsigned long getBufSize(int SampleRate) {
        
        unsigned long mcbsize;
        
        if (SampleRate>=88200) {
            mcbsize = 4096;
        }
        else {
            mcbsize = 2048;
        }
        return mcbsize;
    }
    
private:
    
    unsigned long mfs; // Sample rate
    unsigned long mcbsize; // size of circular buffer
    unsigned long mcorrsize; // cbsize/2 + 1
    unsigned long mcbiwr;
    unsigned long mcbord;
    float* mcbi; // circular input buffer
    float* mcbf; // circular formant correction buffer
    float* mcbo; // circular output buffer
    
    float* mcbwindow; // hann of length N/2, zeros for the rest
    float* macwinv; // inverse of autocorrelation of window
    float* mhannwindow; // length-N hann
    int mnoverlap;
    
    float* mffttime;
    float* mfftfreqre;
    float* mfftfreqim;
    
    // VARIABLES FOR LOW-RATE SECTION
    float maref; // A tuning reference (Hz)
    float minpitch; // Input pitch (semitones)
    float mconf; // Confidence of pitch period estimate (between 0 and 1)
    float moutpitch; // Output pitch (semitones)
    float mvthresh; // Voiced speech threshold
    
    float mpmax; // Maximum allowable pitch period (seconds)
    float mpmin; // Minimum allowable pitch period (seconds)
    unsigned long mnmax; // Maximum period index for pitch prd est
    unsigned long mnmin; // Minimum period index for pitch prd est
    
    float mlrshift; // Shift prescribed by low-rate section
    int   mptarget; // Pitch target, between 0 and 11
    float msptarget; // Smoothed pitch target
    
//    float mlfophase;
    
    // VARIABLES FOR PITCH SHIFTER
    float mphprdd; // default (unvoiced) phase period
    double minphinc; // input phase increment
    double moutphinc; // input phase increment
    double mphincfact; // factor determining output phase increment
    double mphasein;
    double mphaseout;
    float* mfrag; // windowed fragment of speech
    unsigned long mfragsize; // size of fragment in samples
    
    // VARIABLES FOR FORMANT CORRECTOR
    int mford;
    float mfalph;
    float mflamb;
//    float* mfk;
//    float* mfb;
//    float* mfc;
//    float* mfrb;
//    float* mfrc;
//    float* mfsig;
//    float* mfsmooth;
//    float mfhp;
//    float mflp;
    float mflpa;
//    float** mfbuff;
//    float* mftvec;
    float mfmute;
//    float mfmutealph;
    
    float m_pfTune;
    float m_pfFixed;
    float m_pfPull;
    int m_pfA;
    int m_pfBb;
    int m_pfB;
    int m_pfC;
    int m_pfDb;
    int m_pfD;
    int m_pfEb;
    int m_pfE;
    int m_pfF;
    int m_pfGb;
    int m_pfG;
    int m_pfAb;
    float m_pfAmount;
    float m_pfSmooth;
    float m_pfShift;
    int m_pfScwarp;
    int m_pfFcorr;
    float m_pfFwarp;
    float m_pfMix;
    float m_pfPitch;
    float m_pfConf;
    float m_pfInputBuffer1;
    float m_pfOutputBuffer1;
    float m_pfLatency;
    
};

Autotalent::Autotalent(unsigned long SampleRate) {
    this->mfs = SampleRate;
}

void Autotalent::init(unsigned long SampleRate, unsigned long mBufSize)
{
    unsigned long ti;
    
    //Autotalent* membvars = malloc(sizeof(Autotalent));
    
    //AudioBuffer* mBuf = createMemoryBuffer(mBufSize, sizeof(float));
    
    maref = 440;
    
    if (SampleRate>=88200) {
        mcbsize = 4096;
    }
    else {
        mcbsize = 2048;
    }
    
    mcorrsize = mcbsize / 2 + 1;
    
    mpmax = 1/(float)70;  // max and min periods (ms)
    mpmin = 1/(float)700; // eventually may want to bring these out as sliders
    
    mnmax = (unsigned long)(SampleRate * mpmax);
    if (mnmax > mcorrsize) {
        mnmax = mcorrsize;
    }
    mnmin = (unsigned long)(SampleRate * mpmin);
    
    mcbi = (float*) calloc(mcbsize, sizeof(float));
    mcbf = (float*) calloc(mcbsize, sizeof(float));
    mcbo = (float*) calloc(mcbsize, sizeof(float));
    
    mcbiwr = 0;
    mcbord = 0;
    
//    mlfophase = 0;
    
    // Initialize formant corrector
//    mford = 7; // should be sufficient to capture formants
//    mfalph = pow(0.001f, (float) 80 / (SampleRate));
//    mflamb = -(0.8517*sqrt(atan(0.06583*SampleRate))-0.1916); // or about -0.88 @ 44.1kHz
//    mfk   = (float*) calloc(mford, sizeof(float));
//    mfb   = (float*) calloc(mford, sizeof(float));
//    mfc   = (float*) calloc(mford, sizeof(float));
//    mfrb  = (float*) calloc(mford, sizeof(float));
//    mfrc  = (float*) calloc(mford, sizeof(float));
//    mfsig = (float*) calloc(mford, sizeof(float));
//    mfsmooth = (float*) calloc(mford, sizeof(float));
//    mfhp = 0;
//    mflp = 0;
//    mflpa = pow(0.001f, (float) 10 / (SampleRate));
//    mfbuff = (float**) malloc((mford)*sizeof(float*));
//    for (ti=0; ti<mford; ti++)
//    {
//        mfbuff[ti] = (float*) calloc(mcbsize, sizeof(float));
//    }
//    mftvec = (float*) calloc(mford, sizeof(float));
//    mfmute = 1;
//    mfmutealph = pow(0.001f, (float)1 / (SampleRate));
    
    
    // Standard raised cosine window, max height at N/2
    mhannwindow = (float*) calloc(mcbsize, sizeof(float));
    for (ti=0; ti<mcbsize; ti++) {
        mhannwindow[ti] = -0.5*cos(2*PI*ti/mcbsize) + 0.5;
    }
    
    // Generate a window with a single raised cosine from N/4 to 3N/4
    mcbwindow = (float*) calloc(mcbsize, sizeof(float));
    for (ti=0; ti<(mcbsize / 2); ti++) {
        mcbwindow[ti+mcbsize/4] = -0.5*cos(4*PI*ti/(mcbsize - 1)) + 0.5;
    }
    
    mnoverlap = 4;
    
    mfmembvars = fft_con(mcbsize);
    
    mffttime =   (float*) calloc(mcbsize, sizeof(float));
    mfftfreqre = (float*) calloc(mcorrsize, sizeof(float));
    mfftfreqim = (float*) calloc(mcorrsize, sizeof(float));
    
    
    // ---- Calculate autocorrelation of window ----
    macwinv = (float*) calloc(mcbsize, sizeof(float));
    for (ti=0; ti<mcbsize; ti++) {
        mffttime[ti] = mcbwindow[ti];
    }
    fft_forward(mfmembvars, mcbwindow, mfftfreqre, mfftfreqim);
    for (ti=0; ti<mcorrsize; ti++) {
        mfftfreqre[ti] = (mfftfreqre[ti])*(mfftfreqre[ti]) + (mfftfreqim[ti])*(mfftfreqim[ti]);
        mfftfreqim[ti] = 0;
    }
    fft_inverse(mfmembvars, mfftfreqre, mfftfreqim, mffttime);
    for (ti=1; ti<mcbsize; ti++) {
        macwinv[ti] = mffttime[ti]/mffttime[0];
        if (macwinv[ti] > 0.000001) {
            macwinv[ti] = (float)1/macwinv[ti];
        }
        else {
            macwinv[ti] = 0;
        }
    }
    macwinv[0] = 1;
    // ---- END Calculate autocorrelation of window ----
    
    
    mlrshift = 0;
    mptarget = 0;
    msptarget = 0;
    mvthresh = 0.7;  //  The voiced confidence (unbiased peak) threshold level
    
    // Pitch shifter initialization
    mphprdd = 0.01; // Default period
    minphinc = (float)1/(mphprdd * SampleRate);
    mphincfact = 1;
    mphasein = 0;
    mphaseout = 0;
    mfrag = (float*) calloc(mcbsize, sizeof(float));
    mfragsize = 0;
    
    //	SetLatency(mcbsize-1);
}

void Autotalent::Reset(unsigned long SampleRate, unsigned long memBufSize)
{
    //mSampleRate = GetSampleRate();
    //mSamplePeriod = 1./mSampleRate;
    
    unsigned long sr = SampleRate;
    
    if( mfs != sr ) init(sr, memBufSize);
}

void Autotalent::processReplacing(float *inputBuffer, float *outputBuffer, int SampleCount)
{
    
    float fAmount = 1.0f;
    float fSmooth;
    int   iNotes[12];
    int   iPitch2Note[12];
    int   iNote2Pitch[12];
    int   numNotes;
    float fTune = 440.f;
    float fFixed = 0.0f;
    float fPull = 0.0f;
    float fShift = 12.0f;
    int   iScwarp = 1;
//    int   iFcorr; //formant correction variable commented out for the meantime
    float fFwarp;
    float fMix = 1.0f;
    
    unsigned long lSampleIndex;
    
    long int N;
    long int Nf;
    long int fs;
    float pmin;
    float pmax;
    unsigned long nmin;
    unsigned long nmax;
    
    long int ti;
    long int ti2;
    long int ti3;
    long int ti4;
    float tf;
    float tf2;
    
    // Variables for cubic spline interpolator
    float indd;
    int ind0;
    int ind1;
    int ind2;
    int ind3;
    float vald;
    float val0;
    float val1;
    float val2;
    float val3;
    
    int lowersnap;
    int uppersnap;
    
    float pperiod;
    float inpitch;
    float conf;
    float outpitch;
    float aref;
    float fa;
    float fb;
    float fc;
    float fk;
    float flamb;
    float frlamb;
    float falph;
    float foma;
    float f1resp;
    float f0resp;
    float flpa;
    int   ford;

//    fAmount = (float) m_pfAmount;
//    fSmooth = (float) m_pfSmooth * 0.8; // Scales max to a more reasonable value
//    fTune   = (float) m_pfTune;
//    iNotes[0] = (int) m_pfA;
//    iNotes[1] = (int) m_pfBb;
//    iNotes[2] = (int) m_pfB;
//    iNotes[3] = (int) m_pfC;
//    iNotes[4] = (int) m_pfDb;
//    iNotes[5] = (int) m_pfD;
//    iNotes[6] = (int) m_pfEb;
//    iNotes[7] = (int) m_pfE;
//    iNotes[8] = (int) m_pfF;
//    iNotes[9] = (int) m_pfGb;
//    iNotes[10] = (int) m_pfG;
//    iNotes[11] = (int) m_pfAb;
//    fFixed = (float) m_pfFixed;
//    fPull = (float) m_pfPull;
      fShift = (float) m_pfShift;
//    iScwarp = (int) m_pfScwarp;
//    iFcorr = (int) m_pfFcorr;
//    fFwarp = (float) m_pfFwarp;
      fMix = (float) m_pfMix;
 
    //test variables
        iNotes[0] = 1;
        iNotes[1] = 1;
        iNotes[2] = 1;
        iNotes[3] = 1;
        iNotes[4] = 1;
        iNotes[5] = 1;
        iNotes[6] = 1;
        iNotes[7] = 1;
        iNotes[8] = 1;
        iNotes[9] = 1;
        iNotes[10] = 1;
        iNotes[11] = 1;
    
    // Some logic for the semitone->scale and scale->semitone conversion
    // If no notes are selected as being in the scale, instead snap to all notes
    ti2 = 0;
    
    for (ti=0; ti<12; ti++) {
        if (iNotes[ti]>=0) {
            iPitch2Note[ti] = ti2;
            iNote2Pitch[ti2] = ti;
            ti2 = ti2 + 1;
        }
        else {
            iPitch2Note[ti] = -1;
        }
    }
    
    numNotes = ti2;
    while (ti2<12) {
        iNote2Pitch[ti2] = -1;
        ti2 = ti2 + 1;
    }
    if (numNotes==0) {
        for (ti=0; ti<12; ti++) {
            iNotes[ti] = 1;
            iPitch2Note[ti] = ti;
            iNote2Pitch[ti] = ti;
        }
        numNotes = 12;
    }
    iScwarp = (iScwarp + numNotes*5)%numNotes;
    
    ford = mford;
    falph = mfalph;
    foma = (float)1 - falph;
    flpa = mflpa;
    flamb = mflamb;
    tf = pow((float)2,fFwarp/2)*(1+flamb)/(1-flamb);
    frlamb = (tf - 1)/(tf + 1);
    
    maref = (float)fTune;
    
    N = mcbsize;
    Nf = mcorrsize;
    fs = mfs;
    
    pmax = mpmax;
    pmin = mpmin;
    nmax = mnmax;
    nmin = mnmin;
    
    aref     = maref;
    pperiod  = mpmax;
    inpitch  = minpitch;
    conf     = mconf;
    outpitch = moutpitch;
    
    
    /*******************
     *  MAIN DSP LOOP  *
     *******************/
    
    for (lSampleIndex = 0; lSampleIndex < SampleCount; lSampleIndex++)  {
        
        // load data into circular buffer
        tf = *(inputBuffer++);
        //inputBuffer++;
        ti4 = mcbiwr;
        mcbi[ti4] = tf;
        
//        if (iFcorr>=1) {
//            //  Somewhat experimental formant corrector
//            //  formants are removed using an adaptive pre-filter and
//            //  re-introduced after pitch manipulation using post-filter
//            //  tf is signal input
//            fa = tf - mfhp; // highpass pre-emphasis filter
//            mfhp = tf;
//            fb = fa;
//            for (ti=0; ti<ford; ti++) {
//                mfsig[ti] = fa*fa*foma + mfsig[ti]*falph;
//                fc = (fb-mfc[ti])*flamb + mfb[ti];
//                mfc[ti] = fc;
//                mfb[ti] = fb;
//                fk = fa*fc*foma + mfk[ti]*falph;
//                mfk[ti] = fk;
//                tf = fk/(mfsig[ti] + 0.000001);
//                tf = tf*foma + mfsmooth[ti]*falph;
//                mfsmooth[ti] = tf;
//                mfbuff[ti][ti4] = tf;
//                fb = fc - tf*fa;
//                fa = fa - tf*fc;
//            }
//            mcbf[ti4] = fa;
//            // Now hopefully the formants are reduced
//            // More formant correction code at the end of the DSP loop
//        }
//        else {
//            mcbf[ti4] = tf;
//        }
        
        mcbf[ti4] = tf; //temporary - replace above loop for formant correction
        
        // Input write pointer logic
        mcbiwr++;
        if (mcbiwr >= N) {
            mcbiwr = 0;
        }
        
        
        // ********************
        // * Low-rate section *
        // ********************
        
        // Every N/noverlap samples, run pitch estimation / manipulation code
        if ((mcbiwr)%(N/mnoverlap) == 0) {
            
            // ---- Obtain autocovariance ----
            
            // Window and fill FFT buffer
            ti2 = mcbiwr;
            for (ti=0; ti<N; ti++) {
                mffttime[ti] = (float)(mcbi[(ti2-ti+N)%N]*mcbwindow[ti]);
            }
            
            // Calculate FFT
            fft_forward(mfmembvars, mffttime, mfftfreqre, mfftfreqim);
            
            // Remove DC
            mfftfreqre[0] = 0;
            mfftfreqim[0] = 0;
            
            // Take magnitude squared
            for (ti=1; ti<Nf; ti++) {
                mfftfreqre[ti] = (mfftfreqre[ti])*(mfftfreqre[ti]) + (mfftfreqim[ti])*(mfftfreqim[ti]);
                mfftfreqim[ti] = 0;
            }
            
            // Calculate IFFT
            fft_inverse(mfmembvars, mfftfreqre, mfftfreqim, mffttime);
            
            // Normalize
            tf = (float)1/mffttime[0];
            for (ti=1; ti<N; ti++) {
                mffttime[ti] = mffttime[ti] * tf;
            }
            mffttime[0] = 1;
            
            //  ---- END Obtain autocovariance ----
            
            
            //  ---- Calculate pitch and confidence ----
            
            // Calculate pitch period
            //   Pitch period is determined by the location of the max (biased)
            //     peak within a given range
            //   Confidence is determined by the corresponding unbiased height
            tf2 = 0;
            pperiod = pmin;
            for (ti=nmin; ti<nmax; ti++) {
                ti2 = ti-1;
                ti3 = ti+1;
                if (ti2<0) {
                    ti2 = 0;
                }
                if (ti3>Nf) {
                    ti3 = Nf;
                }
                tf = mffttime[ti];
                
                if (tf>mffttime[ti2] && tf>=mffttime[ti3] && tf>tf2) {
                    tf2 = tf;
                    ti4 = ti;
                }
            }
            if (tf2>0) {
                conf = tf2*macwinv[ti4];
                if (ti4>0 && ti4<Nf) {
                    // Find the center of mass in the vicinity of the detected peak
                    tf = mffttime[ti4-1]*(ti4-1);
                    tf = tf + mffttime[ti4]*(ti4);
                    tf = tf + mffttime[ti4+1]*(ti4+1);
                    tf = tf/(mffttime[ti4-1] + mffttime[ti4] + mffttime[ti4+1]);
                    pperiod = tf/fs;
                }
                else {
                    pperiod = (float)ti4/fs;
                }
            }
            
            // Convert to semitones
            tf = (float) -12*log10((float)aref*pperiod)*L2SC;
            if (conf>=mvthresh) {
                inpitch = tf;
                minpitch = tf; // update pitch only if voiced
            }
            mconf = conf;
            
            //  ---- END Calculate pitch and confidence ----
            
            //  ---- Modify pitch in all kinds of ways! ----
            
            outpitch = inpitch;
            
            // Pull to fixed pitch
            outpitch = (1-fPull)*outpitch + fPull*fFixed;
            
            // -- Convert from semitones to scale notes --
            ti = (int)(outpitch/12 + 32) - 32; // octave
            tf = outpitch - ti*12; // semitone in octave
            ti2 = (int)tf;
            ti3 = ti2 + 1;
            // a little bit of pitch correction logic, since it's a convenient place for it
            if (iNotes[ti2%12]<0 || iNotes[ti3%12]<0) { // if between 2 notes that are more than a semitone apart
                lowersnap = 1;
                uppersnap = 1;
            }
            else {
                lowersnap = 0;
                uppersnap = 0;
                if (iNotes[ti2%12]==1) { // if specified by user
                    lowersnap = 1;
                }
                if (iNotes[ti3%12]==1) { // if specified by user
                    uppersnap = 1;
                }
            }
            // (back to the semitone->scale conversion)
            // finding next lower pitch in scale
            while (iNotes[(ti2+12)%12]<0) {
                ti2 = ti2 - 1;
            }
            // finding next higher pitch in scale
            while (iNotes[ti3%12]<0) {
                ti3 = ti3 + 1;
            }
            tf = (tf-ti2)/(ti3-ti2) + iPitch2Note[(ti2+12)%12];
            if (ti2<0) {
                tf = tf - numNotes;
            }
            outpitch = tf + numNotes*ti;
            // -- Done converting to scale notes --
            
            // The actual pitch correction
            ti = (int)(outpitch+128) - 128;
            tf = outpitch - ti - 0.5;
            ti2 = ti3-ti2;
            if (ti2>2) { // if more than 2 semitones apart, put a 2-semitone-like transition halfway between
                tf2 = (float)ti2/2;
            }
            else {
                tf2 = (float)1;
            }
            if (fSmooth<0.001) {
                tf2 = tf*tf2/0.001;
            }
            else {
                tf2 = tf*tf2/fSmooth;
            }
            if (tf2<-0.5) tf2 = -0.5;
            if (tf2>0.5) tf2 = 0.5;
            tf2 = 0.5*sin(PI*tf2) + 0.5; // jumping between notes using horizontally-scaled sine segment
            tf2 = tf2 + ti;
            if ( (tf<0.5 && lowersnap) || (tf>=0.5 && uppersnap) ) {
                outpitch = fAmount*tf2 + ((float)1-fAmount)*outpitch;
            }
            
            // Add in pitch shift
            outpitch = outpitch + fShift;

            // Convert back from scale notes to semitones
            outpitch = outpitch + iScwarp; // output scale rotate implemented here
            ti = (int)(outpitch/numNotes + 32) - 32;
            tf = outpitch - ti*numNotes;
            ti2 = (int)tf;
            ti3 = ti2 + 1;
            outpitch = iNote2Pitch[ti3%numNotes] - iNote2Pitch[ti2];
            if (ti3>=numNotes) {
                outpitch = outpitch + 12;
            }
            outpitch = outpitch*(tf - ti2) + iNote2Pitch[ti2];
            outpitch = outpitch + 12*ti;
            outpitch = outpitch - (iNote2Pitch[iScwarp] - iNote2Pitch[0]); //more scale rotation here
            
            
            if (outpitch<-36) outpitch = -48;
            if (outpitch>24) outpitch = 24;
            
            moutpitch = outpitch;
            
            //  ---- END Modify pitch in all kinds of ways! ----
            
            // Compute variables for pitch shifter that depend on pitch
            minphinc = aref*pow(2,inpitch/12)/fs;
            moutphinc = aref*pow(2,outpitch/12)/fs;
            mphincfact = moutphinc/minphinc;
        }
        // ************************
        // * END Low-Rate Section *
        // ************************
        
        
        
        // *****************
        // * Pitch Shifter *
        // *****************
        
        // Pitch shifter (kind of like a pitch-synchronous version of Fairbanks' technique)
        //   Note: pitch estimate is naturally N/2 samples old
        mphasein = mphasein + minphinc;
        mphaseout = mphaseout + moutphinc;
        
        //   When input phase resets, take a snippet from N/2 samples in the past
        if (mphasein >= 1) {
            mphasein = mphasein - 1;
            ti2 = mcbiwr - N/2;
            for (ti=-N/2; ti<N/2; ti++) {
                mfrag[(ti+N)%N] = mcbf[(ti + ti2 + N)%N];
            }
        }
        
        //   When output phase resets, put a snippet N/2 samples in the future
        if (mphaseout >= 1) {
            mfragsize = mfragsize*2;
            if (mfragsize > N) {
                mfragsize = N;
            }
            mphaseout = mphaseout - 1;
            ti2 = mcbord + N/2;
            ti3 = (long int)(((float)mfragsize) / mphincfact);
            if (ti3>=N/2) {
                ti3 = N/2 - 1;
            }
            for (ti=-ti3/2; ti<(ti3/2); ti++) {
                tf = mhannwindow[(long int)N/2 + ti*(long int)N/ti3];
                // 3rd degree polynomial interpolator - based on eqns from Hal Chamberlin's book
                indd = mphincfact*ti;
                ind1 = (int)indd;
                ind2 = ind1+1;
                ind3 = ind1+2;
                ind0 = ind1-1;
                val0 = mfrag[(ind0+N)%N];
                val1 = mfrag[(ind1+N)%N];
                val2 = mfrag[(ind2+N)%N];
                val3 = mfrag[(ind3+N)%N];
                vald = 0;
                vald = vald - (float)0.166666666667 * val0 * (indd - ind1) * (indd - ind2) * (indd - ind3);
                vald = vald + (float)0.5 * val1 * (indd - ind0) * (indd - ind2) * (indd - ind3);
                vald = vald - (float)0.5 * val2 * (indd - ind0) * (indd - ind1) * (indd - ind3);
                vald = vald + (float)0.166666666667 * val3 * (indd - ind0) * (indd - ind1) * (indd - ind2);
                mcbo[(ti + ti2 + N)%N] = mcbo[(ti + ti2 + N)%N] + vald*tf;
            }
            mfragsize = 0;
        }
        mfragsize++;
        
        //   Get output signal from buffer
        tf = mcbo[mcbord]; // read buffer
        
        mcbo[mcbord] = 0; // erase for next cycle
        mcbord++; // increment read pointer
        if (mcbord >= N) {
            mcbord = 0;
        }
        
        // *********************
        // * END Pitch Shifter *
        // *********************
        
        ti4 = (mcbiwr + 2)%N;
 
/* formant correction code below - replace when memory buffer problem resolved */
//        if (iFcorr>=1) {
//            // The second part of the formant corrector
//            // This is a post-filter that re-applies the formants, designed
//            //   to result in the exact original signal when no pitch
//            //   manipulation is performed.
//            // tf is signal input
//            // gotta run it 3 times because of a pesky delay free loop
//            //  first time: compute 0-response
//            tf2 = tf;
//            fa = 0;
//            fb = fa;
//            for (ti=0; ti<ford; ti++) {
//                fc = (fb-mfrc[ti])*frlamb + mfrb[ti];
//                tf = mfbuff[ti][ti4];
//                fb = fc - tf*fa;
//                mftvec[ti] = tf*fc;
//                fa = fa - mftvec[ti];
//            }
//            tf = -fa;
//            for (ti=ford-1; ti>=0; ti--) {
//                tf = tf + mftvec[ti];
//            }
//            f0resp = tf;
//            //  second time: compute 1-response
//            fa = 1;
//            fb = fa;
//            for (ti=0; ti<ford; ti++) {
//                fc = (fb-mfrc[ti])*frlamb + mfrb[ti];
//                tf = mfbuff[ti][ti4];
//                fb = fc - tf*fa;
//                mftvec[ti] = tf*fc;
//                fa = fa - mftvec[ti];
//            }
//            tf = -fa;
//            for (ti=ford-1; ti>=0; ti--) {
//                tf = tf + mftvec[ti];
//            }
//            f1resp = tf;
//            //  now solve equations for output, based on 0-response and 1-response
//            tf = (float)2*tf2;
//            tf2 = tf;
//            tf = ((float)1 - f1resp + f0resp);
//            if (tf!=0) {
//                tf2 = (tf2 + f0resp) / tf;
//            }
//            else {
//                tf2 = 0;
//            }
//            //  third time: update delay registers
//            fa = tf2;
//            fb = fa;
//            for (ti=0; ti<ford; ti++) {
//                fc = (fb-mfrc[ti])*frlamb + mfrb[ti];
//                mfrc[ti] = fc;
//                mfrb[ti] = fb;
//                tf = mfbuff[ti][ti4];
//                fb = fc - tf*fa;
//                fa = fa - tf*fc;
//            }
//            tf = tf2;
//            tf = tf + flpa*mflp;  // lowpass post-emphasis filter
//            mflp = tf;
//            // Bring up the gain slowly when formant correction goes from disabled
//            // to enabled, while things stabilize.
//            if (mfmute>0.5) {
//                tf = tf*(mfmute - 0.5)*2;
//            }
//            else {
//                tf = 0;
//            }
//            tf2 = mfmutealph;
//            mfmute = (1-tf2) + tf2*mfmute;
//            // now tf is signal output
//            // ...and we're done messing with formants
//        }
//        else {
//            mfmute = 0;
//        }
        
        mfmute = 0;     //temporary - delete when formant correction above is replaced
        
        // Write audio to output of plugin
        
          *(outputBuffer++) = fMix*tf + (1-fMix)*mcbi[ti4];

    }
}

/*********************************************************
 * Class TemplatePatch
 *
 * Main class for the OWL patch
 * Implements I/O methods for linking Autotalent to the OWL

 **********************************************************/

class AutotalentPatch : public Patch {
    
public:
	
	AutotalentPatch() : _Autotalent(getSampleRate()){
        registerParameter(PARAMETER_A, "A"); //  ?
        registerParameter(PARAMETER_B, "B"); //  ?
        registerParameter(PARAMETER_C, "C"); //  ?
        registerParameter(PARAMETER_D, "pitchshift"); //  ?
        registerParameter(PARAMETER_E, "mix");
        
//        memBuf = createMemoryBuffer(this->getBufSize(getSampleRate()), sizeof(float));
        
        this->_Autotalent.init(getSampleRate(), this->getBufSize(getSampleRate()));
        this->_Autotalent.Reset(getSampleRate(), this->getBufSize(getSampleRate()));
    }
    
//    ~AutotalentPatch(){    // complains that memory being freed is not allocated - fft_des(mfmembvars)
//        _Autotalent.~Autotalent();
//    };
    
    void processAudio(AudioBuffer &buffer) {
        
        int bufSize = buffer.getSize();
        
        float* buf = buffer.getSamples(0);
       
        this->_Autotalent.pitchShift(this->getPitch());
        this->_Autotalent.setMix(this->getMix());
        this->_Autotalent.processReplacing(buf, buf, bufSize);
    }
    
private:
    
    Autotalent _Autotalent;
//    AudioBuffer* memBuf = createMemoryBuffer(this->getBufSize(getSampleRate()), sizeof(float));
    
    unsigned long getBufSize(int SampleRate) {
        
        unsigned long mcbsize;
        
        if (SampleRate>=88200) {
            mcbsize = 4096;
        }
        else {
            mcbsize = 2048;
        }
        return mcbsize;
    }

    float getMix()
    {
        float mix = getParameterValue(PARAMETER_E);
        return mix;
    }
    
    float getPitch()
    {
        float pitch = getParameterValue(PARAMETER_D);
        return floor (pitch * 24 - 12);  //scale to +-12 semitones
    }

};


extern "C" {

#include "fftsetup.c"
#include "mayer_fft.c"

}

#endif  // __AutotalentPatch_hpp__
