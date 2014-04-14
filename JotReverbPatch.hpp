#ifndef __JotReverbPatch_hpp__
#define __JotReverbPatch_hpp__

#include "StompBox.h"

/*****************************************************************************************************************************************

Basic Jot Reverb
C-code floating point implementation
C++ wrapping for the Owl pedal

******************************************************************************************************************************************
 
AUTHOR:
    (c) 1994-2012  Robert Bristow-Johnson
    rbj@audioimagination.com

 
LICENSE:
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.
 
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
 
    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 
 
DESCRIPTION:
    The core of the reverb algorithm implemented here is contained in
    a feedback "matrix" and a set of eight delay lines. This structure
    represents a generalized feedback network in which each delay line
    input receives a linear combination of each of the delay outputs and
    of the input signal to the reverberator.  It is based on the published
    work of Jot:
 
    Digital Delay Networks for Designing Artificial Reverberators
 
    Within the framework of Schroeder's parallel comb filter reverberator,
    a method is proposed for controlling the decay characteristics (avoiding
    unnatural resonances) and for compensating the frequency response. The
    method is extended to any recursive delay network having a unitary
    feedback matrix, and allows selection of the reverberator structure
    irrespective of reverberation time control.
 
    Author: Jot, Jean-Marc
    Affiliation: Antoine Chaigne, Enst, departement SIGNAL, Paris, France
    AES Convention: 90 (February 1991)   Preprint Number:3030
 
IMPLEMENTATION NOTES:
    The delay lines are implemented using a base pointer and offset method. With
    this method, the entire delay memory is used to implement one large delay buffer,
    or circular queue. The write pointer is incremented each sample period (modulo
    the total delay length) and all writes and read are done at a fixed "offset"
    from the base pointer. This is analogous to having a large tape loop with
    multiple read and write heads.
 
    The audio data processing in this algorithm is accomplished in 64-sample "CHUNKS".
    This makes the delay line access much more efficient. Essentially, various pointer
    and coefficient loads can be amortized over a multi-sample period. There is no reason
    why the processing could not be done in even larger chunks, as long as there is
    enough memory available (signal storage is proportional to CHUNK length.)
 
OWL PATCH:
    The Patch that wraps this reverb algorithm is at the bottom of this file.
    This version uses chunks of 32 to enable multiples of 32samples buffer sizes.
    It has been tested as is on the Owl and works fine.
    It is possible to revert CHUNK_SIZE TO 64 if need be.
 
    Parameters:
    A   room size
    B   pre delay
    C   cutoff
    D   dry / wet
 
    Wrapped for the Owl effects pedal by the Owl team - hoxtonowl@gmail.com - http://hoxtonowl.com
 
 TO DO:
    replace fixed CHUNK_SIZE by an external parameter.
 
******************************************************************************************************************************************/

#define PRIME_NUMBER_TABLE_SIZE	7600
#define CHUNK_SIZE				64                          // must be power of 2
#define CHUNK_SIZE_RATIO        ((float) 64.f/CHUNK_SIZE)   // ratio between original (64) and actual chunk size
#define BIG_DELAY_BUFFER_SIZE	65536                       // must be power of 2

#define MAX_REVERB_TIME			480000                      // 10 seconds at 48000 Hz
#define MIN_REVERB_TIME			441
#define MAX_ROOM_SIZE			7552
#define MIN_ROOM_SIZE			(4*CHUNK_SIZE)
#define MAX_CUTOFF				0.4975
#define MIN_CUTOFF				0.1134

#define SQRT8					2.82842712474619	// sqrt(8)
#define ONE_OVER_SQRT8			0.353553390593274	//  1/sqrt(8)
#define ALPHA					0.943722057435498	//  pow(3/2, -1/(8-1))
//       of the 8 delay lines, the longest is 3/2 times longer than the shortest.
//       the longest delay is coupled to the room size.
//       the delay lines then decrease exponentially in length.


typedef struct {
	float		output[CHUNK_SIZE];					// each processing block owns its own output CHUNK
	
	float*		buffer_base;						// set to &(buffer[0])
	int			index_mask;							// set to sizeof(buffer) - 1.  sizeof(buffer) must be power of 2
	int			input_index;						// points to where in buffer samples go in
	int			delay_samples;						// the delay amount in samples
} delayBlock;


typedef struct {
	float		output[CHUNK_SIZE];					// each processing block owns its own output CHUNK
    
	float		b0;									// forward gain coefficient
	float		a1;									// feedback coefficient minus 1
	float		y1;									// feedback state
} filterBlock;


typedef struct {
	float           left_output[CHUNK_SIZE];			// each processing block owns its own output CHUNK
	float           right_output[CHUNK_SIZE];
    
	float*			bigDelayBuffer;
	char			primeNumberTable[PRIME_NUMBER_TABLE_SIZE];
    
	float			dry_coef;
	float			wet_coef0;
	float			wet_coef1;
	float			left_reverb_state;
	float			right_reverb_state;
	
	float			node0[CHUNK_SIZE];
	float			node1[CHUNK_SIZE];
	float			node2[CHUNK_SIZE];
	float			node3[CHUNK_SIZE];
	float			node4[CHUNK_SIZE];
	float			node5[CHUNK_SIZE];
	float			node6[CHUNK_SIZE];
	float			node7[CHUNK_SIZE];
	
	delayBlock		left_predelay;
	delayBlock		right_predelay;
	
	delayBlock		delay0;
	delayBlock		delay1;
	delayBlock		delay2;
	delayBlock		delay3;
	delayBlock		delay4;
	delayBlock		delay5;
	delayBlock		delay6;
	delayBlock		delay7;
	
	filterBlock		LPF0;
	filterBlock		LPF1;
	filterBlock		LPF2;
	filterBlock		LPF3;
	filterBlock		LPF4;
	filterBlock		LPF5;
	filterBlock		LPF6;
	filterBlock		LPF7;
} reverbBlock;


void BuildPrimeTable(char* prime_number_table)
{
	int max_stride = (int)sqrt((float)PRIME_NUMBER_TABLE_SIZE);
	
	for(int i=0; i<PRIME_NUMBER_TABLE_SIZE; i++)
	{
		prime_number_table[i] = 1;									// initial value of all entries is 1
	}
	
	prime_number_table[0] = 0;										// now we zero out any entry that is not prime
	prime_number_table[1] = 0;
	int stride = 2;													// start with stride set to the smallest prime
	while (stride <= max_stride)
	{
		for(int i=2*stride; i<PRIME_NUMBER_TABLE_SIZE; i+=stride)	// start at the 2nd multiple of this prime, NOT the prime number itself!!!
		{
			prime_number_table[i] = 0;								// zero out table entries for all multiples of this prime number
		}
		stride++;
		while (prime_number_table[stride] == 0)						// go to next non-zero entry which is the next prime
		{
			stride++;
		}
	}
}


int FindNearestPrime(char* prime_number_table, int number)
{
	while (prime_number_table[number] == 0)
	{
		number--;
	}
	return number;
}


void reverbInitialize(reverbBlock* this_reverb)
{
	int	current_assigned_index = -CHUNK_SIZE;
	current_assigned_index	+= ceil(119*CHUNK_SIZE*CHUNK_SIZE_RATIO);           // max number of samples (plus one extra CHUNK) allocated for this delay in this buffer
	this_reverb->left_predelay.buffer_base		= this_reverb->bigDelayBuffer;
	this_reverb->left_predelay.index_mask		= BIG_DELAY_BUFFER_SIZE-1;
	this_reverb->left_predelay.input_index		= current_assigned_index;		// initial index must always be an integer multiple of CHUNK_SIZE
	this_reverb->left_predelay.delay_samples		= 882;						// let's start out with predelay about 20ms x 44.1 kHz
	
	current_assigned_index	+= ceil(119*CHUNK_SIZE*CHUNK_SIZE_RATIO);  
	this_reverb->right_predelay.buffer_base		= this_reverb->bigDelayBuffer;
	this_reverb->right_predelay.index_mask		= BIG_DELAY_BUFFER_SIZE-1;
	this_reverb->right_predelay.input_index		= current_assigned_index;
	this_reverb->right_predelay.delay_samples	= 882;
	
	current_assigned_index	+= ceil(119*CHUNK_SIZE*CHUNK_SIZE_RATIO);  
	this_reverb->delay0.buffer_base		= this_reverb->bigDelayBuffer;
	this_reverb->delay0.index_mask		= BIG_DELAY_BUFFER_SIZE-1;
	this_reverb->delay0.input_index		= current_assigned_index;
	this_reverb->delay0.delay_samples	= 1544;									// let's start out with room size about 35ms x 44.1 kHz
	
	current_assigned_index	+= ceil(112*CHUNK_SIZE*CHUNK_SIZE_RATIO);
	this_reverb->delay1.buffer_base		= this_reverb->bigDelayBuffer;
	this_reverb->delay1.index_mask		= BIG_DELAY_BUFFER_SIZE-1;
	this_reverb->delay1.input_index		= current_assigned_index;
	this_reverb->delay1.delay_samples	= 1457;
	
	current_assigned_index	+= ceil(106*CHUNK_SIZE*CHUNK_SIZE_RATIO);
	this_reverb->delay2.buffer_base		= this_reverb->bigDelayBuffer;
	this_reverb->delay2.index_mask		= BIG_DELAY_BUFFER_SIZE-1;
	this_reverb->delay2.input_index		= current_assigned_index;
	this_reverb->delay2.delay_samples	= 1375;
	
	current_assigned_index	+= ceil(100*CHUNK_SIZE*CHUNK_SIZE_RATIO);
	this_reverb->delay3.buffer_base		= this_reverb->bigDelayBuffer;
	this_reverb->delay3.index_mask		= BIG_DELAY_BUFFER_SIZE-1;
	this_reverb->delay3.input_index		= current_assigned_index;
	this_reverb->delay3.delay_samples	= 1297;
	
	current_assigned_index	+= ceil(94*CHUNK_SIZE*CHUNK_SIZE_RATIO);
	this_reverb->delay4.buffer_base		= this_reverb->bigDelayBuffer;
	this_reverb->delay4.index_mask		= BIG_DELAY_BUFFER_SIZE-1;
	this_reverb->delay4.input_index		= current_assigned_index;
	this_reverb->delay4.delay_samples	= 1224;
	
	current_assigned_index	+= ceil(89*CHUNK_SIZE*CHUNK_SIZE_RATIO);
	this_reverb->delay5.buffer_base		= this_reverb->bigDelayBuffer;
	this_reverb->delay5.index_mask		= BIG_DELAY_BUFFER_SIZE-1;
	this_reverb->delay5.input_index		= current_assigned_index;
	this_reverb->delay5.delay_samples	= 1155;
	
	current_assigned_index	+= ceil(84*CHUNK_SIZE*CHUNK_SIZE_RATIO);
	this_reverb->delay6.buffer_base		= this_reverb->bigDelayBuffer;
	this_reverb->delay6.index_mask		= BIG_DELAY_BUFFER_SIZE-1;
	this_reverb->delay6.input_index		= current_assigned_index;
	this_reverb->delay6.delay_samples	= 1090;
	
	current_assigned_index	+= ceil(79*CHUNK_SIZE*CHUNK_SIZE_RATIO);
	this_reverb->delay7.buffer_base		= this_reverb->bigDelayBuffer;
	this_reverb->delay7.index_mask		= BIG_DELAY_BUFFER_SIZE-1;
	this_reverb->delay7.input_index		= current_assigned_index;
	this_reverb->delay7.delay_samples	= 1029;
	
	for(int i=0; i<BIG_DELAY_BUFFER_SIZE; i++)
	{
		this_reverb->bigDelayBuffer[i] = 0;
	}
	
	this_reverb->LPF0.a1 = -1.0;
	this_reverb->LPF0.b0 = -ONE_OVER_SQRT8;
	this_reverb->LPF0.y1 = 0.0;
	for (int i=0; i<CHUNK_SIZE; i++)
		this_reverb->LPF0.output[i] = 0.0;
	
	this_reverb->LPF1.a1 = -1.0;
	this_reverb->LPF1.b0 = -ONE_OVER_SQRT8;
	this_reverb->LPF1.y1 = 0.0;
	for (int i=0; i<CHUNK_SIZE; i++)
		this_reverb->LPF1.output[i] = 0.0;
	
	this_reverb->LPF2.a1 = -1.0;
	this_reverb->LPF2.b0 = -ONE_OVER_SQRT8;
	this_reverb->LPF2.y1 = 0.0;
	for (int i=0; i<CHUNK_SIZE; i++)
		this_reverb->LPF2.output[i] = 0.0;
	
	this_reverb->LPF3.a1 = -1.0;
	this_reverb->LPF3.b0 = -ONE_OVER_SQRT8;
	this_reverb->LPF3.y1 = 0.0;
	for (int i=0; i<CHUNK_SIZE; i++)
		this_reverb->LPF3.output[i] = 0.0;
	
	this_reverb->LPF4.a1 = -1.0;
	this_reverb->LPF4.b0 = -ONE_OVER_SQRT8;
	this_reverb->LPF4.y1 = 0.0;
	for (int i=0; i<CHUNK_SIZE; i++)
		this_reverb->LPF4.output[i] = 0.0;
	
	this_reverb->LPF5.a1 = -1.0;
	this_reverb->LPF5.b0 = -ONE_OVER_SQRT8;
	this_reverb->LPF5.y1 = 0.0;
	for (int i=0; i<CHUNK_SIZE; i++)
		this_reverb->LPF5.output[i] = 0.0;
	
	this_reverb->LPF6.a1 = -1.0;
	this_reverb->LPF6.b0 = -ONE_OVER_SQRT8;
	this_reverb->LPF6.y1 = 0.0;
	for (int i=0; i<CHUNK_SIZE; i++)
		this_reverb->LPF6.output[i] = 0.0;
	
	this_reverb->LPF7.a1 = -1.0;
	this_reverb->LPF7.b0 = -ONE_OVER_SQRT8;
	this_reverb->LPF7.y1 = 0.0;
	for (int i=0; i<CHUNK_SIZE; i++)
		this_reverb->LPF7.output[i] = 0.0;
	
	this_reverb->left_reverb_state = 0.0;
	this_reverb->right_reverb_state = 0.0;
	
	BuildPrimeTable(this_reverb->primeNumberTable);
}


void reverbSetParam(reverbBlock* this_reverb, float fSampleRate, float fPercentWet, float fReverbTime, float fRoomSize, float fCutOffAbsorbsion, float fPreDelay)
{
	float wetCoef = fPercentWet/100.0;
	if (wetCoef > 1.0)
		wetCoef = 1.0;
	if (wetCoef < 0.0)
		wetCoef = 0.0;
	
	float fReverbTimeSamples = fReverbTime*fSampleRate;		// fReverbTime (expressed in seconds if fSampleRate is Hz) is the RT60 for the room
	if (fReverbTimeSamples > MAX_REVERB_TIME)
		fReverbTimeSamples = MAX_REVERB_TIME;
	if (fReverbTimeSamples < MIN_REVERB_TIME)
		fReverbTimeSamples = MIN_REVERB_TIME;
    
	float fRoomSizeSamples = fRoomSize*fSampleRate;			// fRoomSize is expressed in seconds = (room length)/(speed of sound)
	if (fRoomSizeSamples > MAX_ROOM_SIZE)
		fRoomSizeSamples = MAX_ROOM_SIZE;
	if (fRoomSizeSamples < MIN_ROOM_SIZE)
		fRoomSizeSamples = MIN_ROOM_SIZE;
	
	float fCutOff = fCutOffAbsorbsion/fSampleRate;
	if (fCutOff > MAX_CUTOFF)
		fCutOff = MAX_CUTOFF;
	if (fCutOff < MIN_CUTOFF)
		fCutOff = MIN_CUTOFF;
    
	float fPreDelaySamples = fPreDelay*fSampleRate;			// fPreDelay is expressed in seconds if fSampleRate is Hz
	if (fPreDelaySamples > MAX_ROOM_SIZE)
		fPreDelaySamples = MAX_ROOM_SIZE;
	if (fPreDelaySamples < 0.0)
		fPreDelaySamples = 0.0;
	
	
	float	fCutoffCoef		= exp(-6.28318530717959*fCutOff);
	
	this_reverb->dry_coef	= 1.0 - wetCoef;
	
	wetCoef	*= SQRT8 * (1.0 - exp(-13.8155105579643*fRoomSizeSamples/fReverbTimeSamples));			// additional attenuation for small room and long reverb time  <--  exp(-13.8155105579643) = 10^(-60dB/10dB)
    //  toss in whatever fudge factor you need here to make the reverb louder
	this_reverb->wet_coef0	= wetCoef;
	this_reverb->wet_coef1	= -fCutoffCoef*wetCoef;
	
	fCutoffCoef /=  (float)FindNearestPrime(this_reverb->primeNumberTable, (int)fRoomSizeSamples);
	
	float	fDelaySamples	= fRoomSizeSamples;
	float	beta			= -6.90775527898214/fReverbTimeSamples;			// 6.90775527898214 = log(10^(60dB/20dB))  <-- fReverbTime is RT60
	float	f_prime_value;
	int		prime_value;
	
	this_reverb->left_predelay.delay_samples = (int)fPreDelaySamples;
	this_reverb->right_predelay.delay_samples = (int)fPreDelaySamples;
	
	prime_value				= FindNearestPrime(this_reverb->primeNumberTable, (int)fDelaySamples);
	this_reverb->delay0.delay_samples	= prime_value - CHUNK_SIZE;									// we subtract 1 CHUNK of delay, because this signal feeds back, causing an extra CHUNK delay
	f_prime_value			= (float)prime_value;
	this_reverb->LPF0.a1	= f_prime_value*fCutoffCoef - 1.0;
	this_reverb->LPF0.b0	= ONE_OVER_SQRT8*exp(beta*f_prime_value)*(this_reverb->LPF0.a1);
	fDelaySamples *= ALPHA;
	
	prime_value				= FindNearestPrime(this_reverb->primeNumberTable, (int)fDelaySamples);
	this_reverb->delay1.delay_samples	= prime_value - CHUNK_SIZE;
	f_prime_value			= (float)prime_value;
	this_reverb->LPF1.a1	= f_prime_value*fCutoffCoef - 1.0;
	this_reverb->LPF1.b0	= ONE_OVER_SQRT8*exp(beta*f_prime_value)*(this_reverb->LPF1.a1);
	fDelaySamples *= ALPHA;
	
	prime_value				= FindNearestPrime(this_reverb->primeNumberTable, (int)fDelaySamples);
	this_reverb->delay2.delay_samples	= prime_value - CHUNK_SIZE;
	f_prime_value			= (float)prime_value;
	this_reverb->LPF2.a1	= f_prime_value*fCutoffCoef - 1.0;
	this_reverb->LPF2.b0	= ONE_OVER_SQRT8*exp(beta*f_prime_value)*(this_reverb->LPF2.a1);
	fDelaySamples *= ALPHA;
	
	prime_value				= FindNearestPrime(this_reverb->primeNumberTable, (int)fDelaySamples);
	this_reverb->delay3.delay_samples	= prime_value - CHUNK_SIZE;
	f_prime_value			= (float)prime_value;
	this_reverb->LPF3.a1	= f_prime_value*fCutoffCoef - 1.0;
	this_reverb->LPF3.b0	= ONE_OVER_SQRT8*exp(beta*f_prime_value)*(this_reverb->LPF3.a1);
	fDelaySamples *= ALPHA;
	
	prime_value				= FindNearestPrime(this_reverb->primeNumberTable, (int)fDelaySamples);
	this_reverb->delay4.delay_samples	= prime_value - CHUNK_SIZE;
	f_prime_value			= (float)prime_value;
	this_reverb->LPF4.a1	= f_prime_value*fCutoffCoef - 1.0;
	this_reverb->LPF4.b0	= ONE_OVER_SQRT8*exp(beta*f_prime_value)*(this_reverb->LPF4.a1);
	fDelaySamples *= ALPHA;
	
	prime_value				= FindNearestPrime(this_reverb->primeNumberTable, (int)fDelaySamples);
	this_reverb->delay5.delay_samples	= prime_value - CHUNK_SIZE;
	f_prime_value			= (float)prime_value;
	this_reverb->LPF5.a1	= f_prime_value*fCutoffCoef - 1.0;
	this_reverb->LPF5.b0	= ONE_OVER_SQRT8*exp(beta*f_prime_value)*(this_reverb->LPF5.a1);
	fDelaySamples *= ALPHA;
	
	prime_value				= FindNearestPrime(this_reverb->primeNumberTable, (int)fDelaySamples);
	this_reverb->delay6.delay_samples	= prime_value - CHUNK_SIZE;
	f_prime_value			= (float)prime_value;
	this_reverb->LPF6.a1	= f_prime_value*fCutoffCoef - 1.0;
	this_reverb->LPF6.b0	= ONE_OVER_SQRT8*exp(beta*f_prime_value)*(this_reverb->LPF6.a1);
	fDelaySamples *= ALPHA;
	
	prime_value				= FindNearestPrime(this_reverb->primeNumberTable, (int)fDelaySamples);
	this_reverb->delay7.delay_samples	= prime_value - CHUNK_SIZE;
	f_prime_value			= (float)prime_value;
	this_reverb->LPF7.a1	= f_prime_value*fCutoffCoef - 1.0;
	this_reverb->LPF7.b0	= ONE_OVER_SQRT8*exp(beta*f_prime_value)*(this_reverb->LPF7.a1);
}


void Delay(delayBlock* this_delay, float* input)
{
	register float*		output		= &(this_delay->output[0]);
	register float*		delay_ptr	= this_delay->buffer_base;
	register int		index_mask	= this_delay->index_mask;
	register int		index		= this_delay->input_index;
    
	delay_ptr += index;
	
	for(register int i=CHUNK_SIZE; i>0; i--)
	{
		*delay_ptr++ = *input++;						// no wrapping nor masking necessary because input index should always start as a multiple of CHUNK_SIZE
	}
	
	index -=  this_delay->delay_samples;				// go to first delayed sample
	delay_ptr = this_delay->buffer_base;
	
	for(register int i=CHUNK_SIZE; i>0; i--)
	{
		index &= index_mask;							// must wrap index and
		*output++ = delay_ptr[index++];					//                     reference from buffer base every sample
	}
	
	index = this_delay->input_index;
	index += CHUNK_SIZE;								// advance input index to pick up where we left off
	index &= index_mask;								// this might need to wrap
	this_delay->input_index = index;					// save state
}


//
//	transfer function:			H(z) = b0/(1 - (a1+1)*z^(-1))
//
//	for the nth delay line:		a1   = delay[n]/size * exp(-2*pi*fcutoff/Fs) - 1  =   pole - 1
//								b0   = a1/sqrt(8) * 10^(-(60dB*delay[n]/RT60)/20dB)
//
void Filter(filterBlock* this_filter, float* input)
{
	register float*	output	= &(this_filter->output[0]);
    
	register float	b0 = this_filter->b0;				// feedforward coefficient
	register float	a1 = this_filter->a1;				// feedback coefficient
	
	register float 	y1 = this_filter->y1;				// previous output
    
	register float	output_sample = y1;					// now is previous output sample, y[n-1]
	
	for (register int i=CHUNK_SIZE; i>0; i--)
	{
		y1 += b0 * (*input++);							// y[n-1] + b0*x[n]
		y1 += a1 * output_sample;						// y[n-1] + b0*x[n] + a1*y[n-1]
		output_sample = y1;
		*output++ = output_sample;
	}
	
	this_filter->y1 = y1;								// save state
}


#define BEGIN_ROW(source_signal)						\
input_ptr = source_signal;						        \
for(register int i=0; i<CHUNK_SIZE; i++)		                        \
{										\
register float	acc = *input_ptr++;

#define	PLUS_ONE(in_ptr)	acc += *in_ptr++;
#define	MINUS_ONE(in_ptr)	acc -= *in_ptr++;

#define	END_ROW(node)								\
node[i] = acc;								        \
}										\
x0 -= CHUNK_SIZE;								\
x1 -= CHUNK_SIZE;								\
x2 -= CHUNK_SIZE;								\
x3 -= CHUNK_SIZE;								\
x4 -= CHUNK_SIZE;								\
x5 -= CHUNK_SIZE;								\
x6 -= CHUNK_SIZE;								\
x7 -= CHUNK_SIZE;


void JotReverb(reverbBlock* this_reverb, float* left_input, float* right_input)
{
	Delay(&(this_reverb->left_predelay), left_input);
	Delay(&(this_reverb->right_predelay), right_input);
	
	register float*	x0 = &(this_reverb->LPF0.output[0]);
	register float*	x1 = &(this_reverb->LPF1.output[0]);
	register float*	x2 = &(this_reverb->LPF2.output[0]);
	register float*	x3 = &(this_reverb->LPF3.output[0]);
	register float*	x4 = &(this_reverb->LPF4.output[0]);
	register float*	x5 = &(this_reverb->LPF5.output[0]);
	register float*	x6 = &(this_reverb->LPF6.output[0]);
	register float*	x7 = &(this_reverb->LPF7.output[0]);
	
	register float*	input_ptr = 0;							// needed for macro expansions below
	
	BEGIN_ROW(&(this_reverb->left_predelay.output[0]))
	PLUS_ONE(x0)
	PLUS_ONE(x1)
	PLUS_ONE(x2)
	PLUS_ONE(x3)
	MINUS_ONE(x4)
	MINUS_ONE(x5)
	MINUS_ONE(x6)
	MINUS_ONE(x7)
	END_ROW(this_reverb->node0)
	
	BEGIN_ROW(&(this_reverb->right_predelay.output[0]))
	PLUS_ONE(x0)
	PLUS_ONE(x1)
	MINUS_ONE(x2)
	MINUS_ONE(x3)
	PLUS_ONE(x4)
	PLUS_ONE(x5)
	MINUS_ONE(x6)
	MINUS_ONE(x7)
	END_ROW(this_reverb->node1)
	
	BEGIN_ROW(&(this_reverb->right_predelay.output[0]))
	PLUS_ONE(x0)
	PLUS_ONE(x1)
	MINUS_ONE(x2)
	MINUS_ONE(x3)
	MINUS_ONE(x4)
	MINUS_ONE(x5)
	PLUS_ONE(x6)
	PLUS_ONE(x7)
	END_ROW(this_reverb->node2)
	
	BEGIN_ROW(&(this_reverb->left_predelay.output[0]))
	PLUS_ONE(x0)
	MINUS_ONE(x1)
	PLUS_ONE(x2)
	MINUS_ONE(x3)
	PLUS_ONE(x4)
	MINUS_ONE(x5)
	PLUS_ONE(x6)
	MINUS_ONE(x7)
	END_ROW(this_reverb->node3)
	
	BEGIN_ROW(&(this_reverb->right_predelay.output[0]))
	PLUS_ONE(x0)
	MINUS_ONE(x1)
	PLUS_ONE(x2)
	MINUS_ONE(x3)
	MINUS_ONE(x4)
	PLUS_ONE(x5)
	MINUS_ONE(x6)
	PLUS_ONE(x7)
	END_ROW(this_reverb->node4)
	
	BEGIN_ROW(&(this_reverb->left_predelay.output[0]))
	PLUS_ONE(x0)
	MINUS_ONE(x1)
	MINUS_ONE(x2)
	PLUS_ONE(x3)
	PLUS_ONE(x4)
	MINUS_ONE(x5)
	MINUS_ONE(x6)
	PLUS_ONE(x7)
	END_ROW(this_reverb->node5)
	
	BEGIN_ROW(&(this_reverb->left_predelay.output[0]))
	PLUS_ONE(x0)
	MINUS_ONE(x1)
	MINUS_ONE(x2)
	PLUS_ONE(x3)
	MINUS_ONE(x4)
	PLUS_ONE(x5)
	PLUS_ONE(x6)
	MINUS_ONE(x7)
	END_ROW(this_reverb->node6)
	
	BEGIN_ROW(&(this_reverb->right_predelay.output[0]))
	PLUS_ONE(x0)
	PLUS_ONE(x1)
	PLUS_ONE(x2)
	PLUS_ONE(x3)
	PLUS_ONE(x4)
	PLUS_ONE(x5)
	PLUS_ONE(x6)
	PLUS_ONE(x7)
	END_ROW(this_reverb->node7)
    
	
	register float*	input = left_input;
	register float*	output = &(this_reverb->left_output[0]);
	register float	reverb_output_state = this_reverb->left_reverb_state;
	for (register int i=CHUNK_SIZE; i>0; i--)
	{
		register float reverb_output = *(x0++) + *(x2++) + *(x4++) + *(x6++);
		register float output_acc = this_reverb->dry_coef * (*input++);
		output_acc += this_reverb->wet_coef0 * reverb_output;
		output_acc += this_reverb->wet_coef1 * reverb_output_state;
		*output++ = output_acc;
		reverb_output_state = reverb_output;
	}
	this_reverb->left_reverb_state = reverb_output_state;
	
	if(left_input != right_input){
	  input = right_input;
	  output = &(this_reverb->right_output[0]);
	  reverb_output_state = this_reverb->right_reverb_state;
	  for (register int i=CHUNK_SIZE; i>0; i--)
	    {
	      register float reverb_output = *(x1++) + *(x3++) + *(x5++) + *(x7++);
	      register float output_acc = this_reverb->dry_coef * (*input++);
	      output_acc += this_reverb->wet_coef0 * reverb_output;
	      output_acc += this_reverb->wet_coef1 * reverb_output_state;
	      *output++ = output_acc;
	      reverb_output_state = reverb_output;
	    }
	  this_reverb->right_reverb_state = reverb_output_state;
	}	
	
	Delay(&(this_reverb->delay0), this_reverb->node0);
	Delay(&(this_reverb->delay1), this_reverb->node1);
	Delay(&(this_reverb->delay2), this_reverb->node2);
	Delay(&(this_reverb->delay3), this_reverb->node3);
	Delay(&(this_reverb->delay4), this_reverb->node4);
	Delay(&(this_reverb->delay5), this_reverb->node5);
	Delay(&(this_reverb->delay6), this_reverb->node6);
	Delay(&(this_reverb->delay7), this_reverb->node7);
	
	Filter(&(this_reverb->LPF0), &(this_reverb->delay0.output[0]));
	Filter(&(this_reverb->LPF1), &(this_reverb->delay1.output[0]));
	Filter(&(this_reverb->LPF2), &(this_reverb->delay2.output[0]));
	Filter(&(this_reverb->LPF3), &(this_reverb->delay3.output[0]));
	Filter(&(this_reverb->LPF4), &(this_reverb->delay4.output[0]));
	Filter(&(this_reverb->LPF5), &(this_reverb->delay5.output[0]));
	Filter(&(this_reverb->LPF6), &(this_reverb->delay6.output[0]));
	Filter(&(this_reverb->LPF7), &(this_reverb->delay7.output[0]));
}


// Owl Patch that wraps the previous functions
class JotReverbPatch : public Patch {
public:
    JotReverbPatch(){
        registerParameter(PARAMETER_A, "roomSize"); //  Reverb Time and Size of room
        registerParameter(PARAMETER_B, "preDelay"); //  preDelay between direct sound and reverb
        registerParameter(PARAMETER_C, "cutoff"); //    Tone control of the reverberant part
        registerParameter(PARAMETER_D, "dryWet"); //    dry/wet mixing
        theReverbBlock.bigDelayBuffer = createMemoryBuffer(1, BIG_DELAY_BUFFER_SIZE)->getSamples(0);
        reverbInitialize(&theReverbBlock);
        setParams();
    }
    
    void processAudio(AudioBuffer &buffer){
        setParams();
        int numSamples = buffer.getSize(); // works for numSamples being a multiple of CHUNK_SIZE
        // Case Mono
        float* buf = buffer.getSamples(0);
        int i=0;
        while (i<numSamples+1-CHUNK_SIZE)
        {
            JotReverb(&theReverbBlock, buf+i,buf+i);
            for (int k=0;k<CHUNK_SIZE;k++)
                {
                    buf[i+k]=theReverbBlock.left_output[k];
                }
            i += CHUNK_SIZE;
        }
    }
    
    void setParams(){
        roomSizeSeconds = 0.15 + 0.45*getParameterValue(PARAMETER_A)*getParameterValue(PARAMETER_A); // betw. 0.15 and 0.6s
        reverbTimeSeconds = 1+getParameterValue(PARAMETER_A)*getParameterValue(PARAMETER_A)*9; // betw. 1 and 10s
        predelaySeconds = getParameterValue(PARAMETER_B)*0.1; // betw. 0 and 0.1s
        cutoffFrequency = 1000+getParameterValue(PARAMETER_C)*15000; // betw. 1000 and 16000 Hz
        dryWet = getParameterValue(PARAMETER_D)*100;    // betw. 0 and 100%
        reverbSetParam(&theReverbBlock, getSampleRate(), dryWet, reverbTimeSeconds, roomSizeSeconds, cutoffFrequency, predelaySeconds);
    }
    
private:
    reverbBlock theReverbBlock;
    float cutoffFrequency;
    float roomSizeSeconds;
    float reverbTimeSeconds;
    float dryWet;
    float predelaySeconds;
};

#endif // __JotReverbPatch_hpp__
