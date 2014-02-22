#ifndef FFTSETUP_H
#define FFTSETUP_H

#ifdef __cplusplus
 extern "C" {
#endif

// Variables for FFT routine
typedef struct
{
	int nfft;        // size of FFT
	int numfreqs;    // number of frequencies represented (nfft/2 + 1)
	float* fft_data; // array for writing/reading to/from FFT function
} fft_vars;

// Constructor for FFT routine
fft_vars* fft_con(int nfft);

// Destructor for FFT routine
void fft_des(fft_vars* membvars);

// Perform forward FFT of real data
// Accepts:
//   membvars - pointer to struct of FFT variables
//   input - pointer to an array of (real) input values, size nfft
//   output_re - pointer to an array of the real part of the output,
//     size nfft/2 + 1
//   output_im - pointer to an array of the imaginary part of the output,
//     size nfft/2 + 1
void fft_forward(fft_vars* membvars, float* input, float* output_re, float* output_im);

// Perform inverse FFT, returning real data
// Accepts:
//   membvars - pointer to struct of FFT variables
//   input_re - pointer to an array of the real part of the output,
//     size nfft/2 + 1
//   input_im - pointer to an array of the imaginary part of the output,
//     size nfft/2 + 1
//   output - pointer to an array of (real) input values, size nfft
void fft_inverse(fft_vars* membvars, float* input_re, float* input_im, float* output);

#ifdef __cplusplus
}
#endif

#endif /* FFTSETUP_H */
