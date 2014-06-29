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

import("music.lib");

smoothdelay =  (+ :sdelay(N, interp, dtime)) ~ *(fback)
	with 
	{ 
		N	= int(2^17); 
		interp 	= hslider("Interpolation[unit:ms][style:knob][OWL:PARAMETER_A]",10,1,100,0.1)*SR/1000.0; 
		dtime	= hslider("Delay[unit:ms][style:knob][OWL:PARAMETER_B]", 0, 0, 500, 0.1)*SR/1000.0;
		fback 	= hslider("Feedback[style:knob][OWL:PARAMETER_C]",0,0,100,0.1)/100.0;

	};
	
dry     = 1 - hslider("Dry/Wet[style:knob][OWL:PARAMETER_D]", 0.3333, 0, 1, 0.025);
wet     = 1 - dry + hslider("Super Wet[style:knob][OWL:PARAMETER_E]", 0.0, 0, 0.5, 0.025);
		
process = _<:*(dry),(*(wet) : smoothdelay ) :>_;
	


