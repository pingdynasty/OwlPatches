//*****************************************************************************
//
//  Siren : An FM-like siren synthesizer for dubwise effects with the OWL
//          Yet, Siren is pure synthesis, it does not require any input signal
//
// Parameters :
//     f0 : fundamental frequency [in Hz]
//     fm : modulation frequency [in Hz]
//     fs : sampling frequency [in Hz]
//     mode : type of modulation [0 to 6: 0<->cos, 1<->square, 2:6<->modified squares]
//     tr60 : reberberation time [in seconds]
//
//  Created by Charles Verron - www.charlesverron.com - June 2013
//
//  License : GnuGPL
//
//  Acknowledgments : Siren's feedback delay network is based on an original implementation by Spencer Campbell.
//
//*****************************************************************************

#ifndef __SirenPatch_hpp__
#define __SirenPatch_hpp__

#include <math.h>       /* fabs, floor, etc... */
#include "StompBox.h"

const float MY_FLOAT_THRESHOLD = 0.00001; // under this value all numbers are set to 0 (gives a -100dB SNR)
static const int Nchannels = 32;                    // number of internal channels
const int primes[Nchannels] = {503,577,641,701,769,839,911,983,1049,1109,1193,1277,1321,1429,1487,1559,1619,1699,1783,1871,1949,2017,2089,2161,2267,2339,2393,2473,2579,2663,2713,2791}; // prime numbers for ReverbFDN delay lines
static const float Nchannels_SQRT = 5.65685425;            // sqrt of number of channels



/*********************************************************
 * Class Sawtooth
 *
 * Produces a sawtooth signal with values between 0. and 1.
 * Parameters :
 *      freqHz : sawtooth frequency [in Hz]
 *      fs : sampling frequency [in Hz]
 **********************************************************/
class Sawtooth
{   
public:    
    // constructor (with an optional sawtooth initial value, usefull to avoid initial clics)
    Sawtooth(float freqHz, float fs, float init_val = 0.); // WARNING : optional init_val must be between 0 and 1 !
    
    // processReplacing method: computes bufferSize sawtooth values to fill an audio buffer
    void processReplacing(float *outputBuffer, int bufferSize);
    
    // processReplacing method with a time varying frequency (tvf) given as parameter (freqHzBuffer)
    // Notes: freqHzBuffer must be a buffer of bufferSize samples
    // using freqHzBuffer, the internal Sawtooth frequency is just ignored
    void processReplacing_tvf(float *outputBuffer, int bufferSize, float *freqHzBuffer);
    
    void setFreqHz(float f)
    {
        this->_freqHz=fabs(f); // warning: negative frequency would make the sawtooth go crazy (=>fabs for security)
        this->_freq_normalized=this->_freqHz/this->_fs;
    }
    
private:
    // PARAMETERS
    float _freqHz;          // sawtooth frequency [in Hz]
    float _fs;              // sampling frequency [in Hz]
    
    // INTERNAL THINGS
    float _val;             // current value
    float _freq_normalized; // normalized frequency (0 to 1)
    
};

Sawtooth::Sawtooth(float freqHz, float fs, float init_val)
{
    this->_freqHz = fabs(freqHz); // fabs for security (to avoid negative frequencies)
    this->_freq_normalized = this->_freqHz / fs;
    this->_fs = fs;
    if (init_val >= 0. && init_val < 1.)
        this->_val = init_val;
    else
        this->_val = 0.;
}

void Sawtooth::processReplacing(float *outputBuffer, int bufferSize)
{
    for (int i_samp=0; i_samp<bufferSize; ++i_samp)
    {
        this->_val += this->_freq_normalized;
        if (this->_val >= 1.)
            this->_val -= 1.;
        outputBuffer[i_samp] = this->_val;
    }
}

void Sawtooth::processReplacing_tvf(float *outputBuffer, int bufferSize, float *freqHzBuffer)
{
    for (int i_samp=0; i_samp<bufferSize; ++i_samp)
    {
        this->_val += fabs(freqHzBuffer[i_samp]/this->_fs); // again, fabs for security (to avoid negative frequencies)
        if (this->_val >= 1.)
            this->_val -= 1.;
        outputBuffer[i_samp] = this->_val;
    }
}





/*********************************************************
 * Class DCcutFilter
 *
 * Biquad filter cutting DC component (and very low frequencies)
 * Implements the difference equation y[n] = x[n] - x[n-1] + 0.9999 y[n-1]
 * Parameters :
 *      none
 **********************************************************/
class DCcutFilter
{
public:
    DCcutFilter();
    
    // process replacing
    void processReplacing(float *inputBuffer, float *outputBuffer, int bufferSize);

private:
    // INTERNAL THINGS
    float _xNmoins1; // x[n-1]
    float _yNmoins1; // y[n-1]
    
};

DCcutFilter::DCcutFilter()
{
    this->_xNmoins1 = 0.;
    this->_yNmoins1 = 0.;
}

void DCcutFilter::processReplacing(float *inputBuffer, float *outputBuffer, int bufferSize)
{
    float x;
    for (int i_samp=0; i_samp<bufferSize; ++i_samp)
    {
        x = inputBuffer[i_samp];
        this->_yNmoins1 = x - this->_xNmoins1 + 0.9999 * this->_yNmoins1;
        if (fabs(this->_yNmoins1) < MY_FLOAT_THRESHOLD)
            this->_yNmoins1 = 0.;
        this->_xNmoins1 = x;
        outputBuffer[i_samp] = this->_yNmoins1;
    }
}





/*********************************************************
 * Class FMSynth
 *
 * Simple frequency modulation synthesizer 
 * Parameters : 
 *      f0 : fundamental frequency [in Hz]
 *      fm : modulation frequency [in Hz]
 *      fs : sampling frequency [in Hz]
 *      mode : type of modulation [0 to 6: 0<->cos, 1<->square, 2:6<->modified squares]
 **********************************************************/
class FMSynth
{
public:
    FMSynth(float f0, float fm, float fs); // constructor (mode is set to 0 by default)
    
    // processReplacing method: synthesizes bufferSize values to fill an audio buffer
    void processReplacing(float *outputBuffer, int bufferSize);
    
    // accessors and modifiers
    void setF0(float f0)
    {
        this->_f0=f0;
    }
    
    void setFm(float fm)
    {
        this->_fm=fm;
        this->_sawtooth_fm.setFreqHz(fm);
    }
    
    void setMode(int mode)
    {
        this->_mode=mode;
    }
    
private:
    // PARAMETERS
    float _f0;       // fundamental frequency [in Hz]
    float _fm;       // modulation frequency [in Hz]
    float _fs;       // sampling frequency [in Hz]
    int _mode;       // mode for modulation [0: cos, 1: square, etc...]
    
    // INTERNAL THINGS
    Sawtooth _sawtooth_f0;
    Sawtooth _sawtooth_fm;
    DCcutFilter _DCcutFilter1;
    DCcutFilter _DCcutFilter2;
    
};

FMSynth::FMSynth(float f0, float fm, float fs) : _sawtooth_f0(f0,fs,0.5) , _sawtooth_fm(fm,fs)
{
    this->_f0 = f0;
    this->_fm = fm;
    this->_fs = fs;
    this->_mode = 0; // mode 0 by default
}

void FMSynth::processReplacing(float *outputBuffer, int bufferSize)
{
    float* freqHzBuffer = new float[bufferSize]; // buffer to store modulation frequency
    
    // compute modulation frequency (LFO)
    this->_sawtooth_fm.processReplacing(freqHzBuffer,bufferSize);
    
    // REPLACE SAWTOOTH LFO BY A SINUSOIDAL LFO
    for (int i_samp=0; i_samp<bufferSize; i_samp++)
    {
        freqHzBuffer[i_samp] = sinf(2*M_PI*freqHzBuffer[i_samp]) * 0.5;
    }
    
    // filter the sawtooth to remove its DC. Solves the terrible case fm=0 (that produces a random frequency offset).
    this->_DCcutFilter1.processReplacing(freqHzBuffer, freqHzBuffer, bufferSize);
    
    // other modes for computing fm (square-based modulations)
    if (this->_mode > 0)
    {
        for (int i_samp=0; i_samp<bufferSize; ++i_samp)
        {
            freqHzBuffer[i_samp] = freqHzBuffer[i_samp]>-0.00001 ? 0.5*this->_mode : -0.5*this->_mode;
        }
    }
    
    // add the constant f0 to the modulation
    for (int i_samp=0; i_samp<bufferSize; ++i_samp)
    {
        freqHzBuffer[i_samp] = this->_f0 * (1.+freqHzBuffer[i_samp]); 
    } 
    
    // compute audio signal
    this->_sawtooth_f0.processReplacing_tvf(outputBuffer, bufferSize, freqHzBuffer);
    
    // rescale to [-0.25;0.25] instead of [0 1]
    for (int i_samp=0; i_samp<bufferSize; ++i_samp)
    {
        outputBuffer[i_samp] -= 0.5;
        outputBuffer[i_samp] *= 0.5; // divide by 2 (to prevent clipping)
    }
    
    // and finally cut DC from the audio signal (now for the case f0=0)
    this->_DCcutFilter2.processReplacing(outputBuffer, outputBuffer, bufferSize);
	delete freqHzBuffer;
}





/*********************************************************
 * Class DelayLine
 *
 * Simple DelayLine
 * Parameters :
 *      size : size of the delay line [in sample]
 **********************************************************/
class DelayLine
{    
public:
    DelayLine(); // the default constructor builds a DelayLine of size=1 sample
    DelayLine(int size);
    
    // process_tick : feed the delay line with one input sample
    float process_tick(float input);
    
private:
    // PARAMETERS
    int _size;  // DelayLine size (in samples)
    
    // INTERNAL THINGS
    int _indRead;
    int _indWrite;
    float * _circbuffer;
    
};

DelayLine::DelayLine()
{
    this->_indRead = 0;
    this->_size = 1; // set size to 1 (no delay)
    this->_indWrite = 0;
    this->_circbuffer = new float[1]; // delay line of size 1 (no delay)
    this->_circbuffer[0] = 0.; // set the unique circbuffer sample to 0.
}

DelayLine::DelayLine(int size)
{
    this->_indRead = 0;
    
    if (size < 1)
    {
        // if size > max, then set size to 1
        this->_size = 1;
    }
    else
    {
        this->_size = size;
    }
    this->_indWrite = this->_size-1;
    this->_circbuffer = new float[this->_size];
    memset(this->_circbuffer,0,this->_size*sizeof(float)); // set the circbuffer samples to 0.
}

// process_tick : feed the delay line with one input sample
float DelayLine::process_tick(float input)
{
    float output;
    
    this->_circbuffer[this->_indWrite] = fabs(input)>MY_FLOAT_THRESHOLD ? input:0.;
    
    this->_indWrite++;
    if (this->_indWrite >= this->_size)
        this->_indWrite -= this->_size;
    
    output = this->_circbuffer [this->_indRead];
    
    this->_indRead++;
    if (this->_indRead >= this->_size)
        this->_indRead -= this->_size;
    
    return output;
}





/*********************************************************
 * Class ReverbFDN
 *
 * Feedback delay network reverberation (see e.g., J.M. Jot 1991 AES paper for theoretical aspects)
 * Parameters :
 *      fs : sampling frequency [in Hz]
 *      tr60 : reberberation time [in seconds]
 *      drywet : balance between the dry and wet signals [0. to 1.]
 *
 * Acknowledgments : the feedback delay network is based on an original implementation by Spencer Campbell.
 **********************************************************/
class ReverbFDN
{
public:
    ReverbFDN(float fs, float tr60, float drywet);
    
    // set reverberation time tr60 (in seconds)
    void setTR60(float tr60)
    {
        if (tr60 <= 0)
            tr60 = 0.0000000001;
        
        if (this->_tr60 == tr60)
        {
            return; // nothing to do
        }
        else
        {
            // set feedback gains appropriately
            for (int i_chan=0; i_chan<Nchannels; i_chan++)
            {
                this->_feedbackGains[i_chan] = 1./Nchannels_SQRT * pow(10., -3. * primes[i_chan] / (tr60*this->_fs));
            }
        }
    }
    
    // set dry/wet balance (between 0 and 1, 0<->100% wet ; 1<->100% dry)
    void setDryWet(float drywet)
    {
        if (drywet>1.)
            drywet=1.;
        else if (drywet<0.)
            drywet=0;
        this->_drywet=drywet;
    }
    
    // process method for ReverbFDN
    void processReplacing(float *inputBuffer, float *outputBuffer, int bufferSize);
    
private:
    // PARAMETERS
    float _fs;      // sampling frequency
    float _tr60;    // reverberation time (TR60)
    float _drywet;  // dry/wet balance (0<->100% wet ; 1<->100% dry)
    
    // INTERNAL THINGS
    static const int Nchannels_log2 = 5;                       // log2 of number of channels (for hadamard)
    DelayLine _delayLines[Nchannels];    // Nchannels delay lines
    float _dl[Nchannels];                // outputs of delay lines
    float _feedbackGains[Nchannels];     // feedback gains of delay lines
    
};

ReverbFDN::ReverbFDN(float fs, float tr60, float drywet)
{
    this->_fs = fs;
    this->setDryWet(drywet);
    this->setTR60(tr60);
    
    // init of the Nchannels delay lines
    for (int i_chan=0; i_chan<Nchannels; i_chan++)
    {
        this->_delayLines[i_chan] = DelayLine(primes[i_chan]);
        this->_dl[i_chan] = 0.;
    }
}

void ReverbFDN::processReplacing(float *inputBuffer, float *outputBuffer, int bufferSize)
{
    long temp1_long;
    float input, temp2_float;
    
    for (int i_samp=0 ; i_samp<bufferSize ; ++i_samp)
    {
        // keep track of input. Solves case where the same unique buffer is given as input and output
        input = inputBuffer[i_samp];
        
        // first set outputBuffer[i_samp] to 0
        outputBuffer[i_samp] = 0.;
        
        // get outputs from all delay lines and sum to the mono output
        for (int i_delayLine = 0 ; i_delayLine<Nchannels ; i_delayLine++)
        {
            this->_dl[i_delayLine] = this->_delayLines[i_delayLine].process_tick(this->_feedbackGains[i_delayLine]*(this->_dl[i_delayLine]+input));
            outputBuffer[i_samp] += this->_dl[i_delayLine];
        }
        
        // divide by Nchannels_SQRT to rescale the sum and apply dry/wet balance
        outputBuffer[i_samp] *= (1-this->_drywet)/Nchannels_SQRT;
        outputBuffer[i_samp] += input * this->_drywet;
        
        // hadamard matrixing and feedback loop (replacing delayLines inputs)
        for (int i=0 ; i < Nchannels_log2 ; ++i)
        {
            for (int j=0 ; j < (1 << Nchannels_log2) ; j += 1 << (i+1))
            {
                temp1_long = (1<<i);
                for (int k=0 ; k < temp1_long ; ++k)
                {
                    temp2_float = this->_dl[j + k];
                    this->_dl[j + k] += this->_dl[j + k + temp1_long];
                    this->_dl[j + k + temp1_long] = temp2_float - this->_dl[j + k + temp1_long];
                }
            }
        }
    }
}





/*********************************************************
 * Class Siren
 *
 * Main class for the siren synthesizer
 * Contains an FMSynth and a ReverbFDN
 * Parameters :
 *      f0 : fundamental frequency [in Hz]
 *      fm : modulation frequency [in Hz]
 *      fs : sampling frequency [in Hz]
 *      mode : type of modulation [0 to 6: 0<->cos, 1<->square, 2:6<->modified squares]
 *      tr60 : reberberation time [in seconds]
 **********************************************************/
class Siren
{
public:
    explicit Siren(float fs); // by default, f0=0Hz, fm=0Hz, mode=0, tr60=0
    
    // process method for ReverbFDN
    void processReplacing(float *outputBuffer, int bufferSize);
    
    // set Siren f0
    void setF0(float f0)
    {
        this->_FMSynth.setF0(f0);
    }
    
    // set Siren fm
    void setFm(float fm)
    {
        this->_FMSynth.setFm(fm);
    }
    
    // set Siren mode
    void setFMSynthMode(int mode)
    {
        this->_FMSynth.setMode(mode);
    }
    
    // set Siren tr60
    void setTR60(float tr60)
    {
        this->_reverbFDN.setTR60(tr60);
    }

private:
    float _fs;
    FMSynth _FMSynth;
    ReverbFDN _reverbFDN;
    
};

Siren::Siren(float fs) : _FMSynth(0., 0., fs) , _reverbFDN(fs, 0., 0.5)
{
    this->_fs = fs;
    // nothing else to do since other members are already initialised with appropriate parameters (initialization lists)
}

void Siren::processReplacing(float *outputBuffer, int bufferSize)
{
    this->_FMSynth.processReplacing(outputBuffer, bufferSize);
    this->_reverbFDN.processReplacing(outputBuffer, outputBuffer, bufferSize);
}





/*********************************************************
 * Class TemplatePatch 
 * 
 * Main class for the OWL patch
 * Implements I/O methods for linking the Siren to the OWL 
 * Parameters :
 *      none
 **********************************************************/
class SirenPatch : public Patch
{    
public:
    SirenPatch() : _Siren(getSampleRate()) {
      registerParameter(PARAMETER_A, "f0");
      registerParameter(PARAMETER_B, "fm");
      registerParameter(PARAMETER_C, "mode");
      registerParameter(PARAMETER_D, "tr60");
    }
    
    void processAudio(AudioBuffer &buffer) 
    {
        int bufferSize = buffer.getSize();
        
        // set parameters
        this->_Siren.setF0(this->getF0());
        this->_Siren.setFm(this->getFm());
        this->_Siren.setFMSynthMode(this->getMode());
        this->_Siren.setTR60(this->getTR60());
        
        // process samples
     for(int ch = 0; ch<buffer.getChannels(); ++ch)
       { 
        float* buf = buffer.getSamples(ch);
        this->_Siren.processReplacing(buf, bufferSize);
       }	
        
    }
    
private:
    Siren _Siren;

    // 1st knob controls Siren's f0
    // param_A=[0;1] <-> f0=[0;500]
    float getF0()
    {
        float f0 = 500 * getParameterValue(PARAMETER_A);
        return f0;
    }
    
    // 2nd knob controls Siren's fm
    // param_B=[0;1] <-> fm=[0;20]
    float getFm()
    {
        float fm = 20 * getParameterValue(PARAMETER_B);
        return fm;
    }
    
    // 3rd knob controls Siren's mode
    // param_C=[0;1] <-> mode=[0..6]
    int getMode()
    {
        int mode = 0;
        float param_C = getParameterValue(PARAMETER_C);
        if (param_C <= 0.25)
            mode = 0;
        else if (param_C <= 0.5)
            mode = 1;
        else if (param_C <= 0.75)
            mode = 2;
        else if (param_C <= 0.85)
            mode = 3;
        else if (param_C <= 0.95)
            mode = 4;
        else if (param_C <= 0.995)
            mode = 5;
        else
            mode = 6;
        return mode;
    }
    
    // 4th knob controls Siren's tr60
    // param_D=[0;1] <-> tr60=[0;10]
    float getTR60()
    {
        float tr60 = getParameterValue(PARAMETER_D);
        tr60 = tr60*10;
        return tr60;
    }
    
};



#endif // __SirenPatch_hpp__
