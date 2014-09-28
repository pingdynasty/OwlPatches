#ifndef __BiquadFilter_h__
#define __BiquadFilter_h__

/** Direct Form 1 Cascaded Biquad Filter 
 * Implemented using CMSIS DSP Library
 * Each cascaded stage implements a second order filter
 */
class BiquadFilter {
private:
  arm_biquad_casd_df1_inst_f32 df1;
  float* coefficients;
  /*
   * The <code>pState</code> is a pointer to state array.   
   * Each Biquad stage has 4 state variables <code>x[n-1], x[n-2], y[n-1],</code> and <code>y[n-2]</code>.   
   * The state variables are arranged in the <code>pState</code> array as:   
   * <pre>   
   *     {x[n-1], x[n-2], y[n-1], y[n-2]}   
   * </pre>   
   * The 4 state variables for stage 1 are first, then the 4 state variables for stage 2, and so on.   
   * The state array has a total length of <code>4*numStages</code> values.   
   * The state variables are updated after each block of data is processed; the coefficients are untouched.   
   */
  float* state;
  int stages;
protected:
  void init(int s){
    stages = s;
    coefficients = NULL;
    state = (float*)malloc(stages*4*sizeof(float));
  }
public:
  BiquadFilter() {
    init(1);
  }
  BiquadFilter(int stages) {
    init(stages);
  }
  ~BiquadFilter(){
    free(state);
  }
  /*
   * The coefficients are stored in the array <code>pCoeffs</code> in the following order:
   * <pre>
   *     {b10, b11, b12, a11, a12, b20, b21, b22, a21, a22, ...}
   * </pre>
   * where <code>b1x</code> and <code>a1x</code> are the coefficients for the first stage,
   * <code>b2x</code> and <code>a2x</code> are the coefficients for the second stage,
   * and so on.  The <code>coeffs</code> array must contain a total of <code>5*stages</code> values.   
   */
  void setCoefficents(float* coeffs){
    coefficients = coeffs;
    // note: this also clears the state buffer
    arm_biquad_cascade_df1_init_f32(&df1, stages, coefficients, state);
  }
  /* perform in-place processing */
  void process(float* buf, int size){
    arm_biquad_cascade_df1_f32(&df1, buf, buf, size);
  }
  /* process filter, leaving input intact */
  void process(float* input, float* output, int size){
    arm_biquad_cascade_df1_f32(&df1, input, output, size);
  }
  /* process a single sample and return the result */
  float process(float input){
    float output;
    arm_biquad_cascade_df1_f32(&df1, &input, &output, 1);    
  }
};

#endif // __BiquadFilter_h__
