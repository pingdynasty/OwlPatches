declare name 	"SmoothDelay";
declare author 	"Yann Orlarey";
declare copyright "Grame";
declare version "1.0";
declare license "STK-4.3";

//--------------------------process----------------------------
//
// 	A stereo smooth delay with a feedback control
//  
//	This example shows how to use sdelay, a delay that doesn't
//  click and doesn't transpose when the delay time is changed
//-------------------------------------------------------------

import("stdfaust.lib");

smoothdelay =  (+ :de.sdelay(N, interp, dtime)) ~ *(fback)
	with 
	{ 
		s = 0.999; // smoothing parameter (one-fi.pole fi.pole location)
		N	= int(2^17); 
		dtime	= hslider("Delay[unit:ms][style:knob][OWL:PARAMETER_A]", 1, 0.0001, 500, 0.1)*ma.SR/1000.0 : si.smooth(s);
		fback 	= hslider("Feedback[style:knob][OWL:PARAMETER_B]",0,0,100,0.1)/100.0;
		interp 	= hslider("Interpolation[unit:ms][style:knob][OWL:PARAMETER_C]",10,1,100,0.1)*ma.SR/1000.0;

	};
	
dry     = 1 - hslider("Dry/Wet[style:knob][OWL:PARAMETER_D]", 0.3333, 0, 1, 0.025);
wet     = 1 - dry + hslider("Super Wet[style:knob][OWL:PARAMETER_E]", 0.0, 0, 0.5, 0.025);
		
process = _<:*(dry),(*(wet) : smoothdelay ) :>_;
	
