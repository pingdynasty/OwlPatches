
#include "mayer_fft.h"

// Variables for FFT routine
typedef struct
{
	int nfft;        // size of FFT
	int numfreqs;    // number of frequencies represented (nfft/2 + 1)
	float* fft_data; // array for writing/reading to/from FFT function
    
} fft_vars;

fft_vars* fft_init(fft_vars* membvars, int nfft, float* fft_data){
	membvars->nfft = nfft;
	membvars->numfreqs = nfft/2 + 1;
	membvars->fft_data = fft_data; //(float*) calloc(nfft, sizeof(float));
    return membvars;
}

// Perform forward FFT of real data
// Accepts:
//   membvars - pointer to struct of FFT variables
//   input - pointer to an array of (real) input values, size nfft
//   output_re - pointer to an array of the real part of the output,
//     size nfft/2 + 1
//   output_im - pointer to an array of the imaginary part of the output,
//     size nfft/2 + 1
void fft_forward(fft_vars* membvars, float* input, float* output_re, float* output_im)
{
	int ti;
	int nfft;
	int hnfft;
	int numfreqs;
	
	nfft = membvars->nfft;
	hnfft = nfft/2;
	numfreqs = membvars->numfreqs;
	
	for (ti=0; ti<nfft; ti++) {
		membvars->fft_data[ti] = input[ti];
	}
	
	mayer_realfft(nfft, membvars->fft_data);
	
	output_im[0] = 0;
	for (ti=0; ti<hnfft; ti++) {
		output_re[ti] = membvars->fft_data[ti];
		output_im[ti+1] = membvars->fft_data[nfft-1-ti];
	}
	output_re[hnfft] = membvars->fft_data[hnfft];
	output_im[hnfft] = 0;
}

// Perform inverse FFT, returning real data
// Accepts:
//   membvars - pointer to struct of FFT variables
//   input_re - pointer to an array of the real part of the output,
//     size nfft/2 + 1
//   input_im - pointer to an array of the imaginary part of the output,
//     size nfft/2 + 1
//   output - pointer to an array of (real) input values, size nfft
void fft_inverse(fft_vars* membvars, float* input_re, float* input_im, float* output)
{
	int ti;
	int nfft;
	int hnfft;
	int numfreqs;
	
	nfft = membvars->nfft;
	hnfft = nfft/2;
	numfreqs = membvars->numfreqs;
	
	for (ti=0; ti<hnfft; ti++) {
		membvars->fft_data[ti] = input_re[ti];
		membvars->fft_data[nfft-1-ti] = input_im[ti+1];
	}
	membvars->fft_data[hnfft] = input_re[hnfft];
	
	mayer_realifft(nfft, membvars->fft_data);
	
	for (ti=0; ti<nfft; ti++) {
		output[ti] = membvars->fft_data[ti];
	}
}

