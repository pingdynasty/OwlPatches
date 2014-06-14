// WahWah with lfo modulation
import("music.lib");
import("effect.lib");

phasor(freq) = freq/SR : (+, 1.0 : fmod) ~ _;
oscy(freq) = phasor(freq)*2*PI : sin;

//lfo = oscy(hslider("lfo-freq[OWL:PARAMETER_C]", 1,0.05,1,0.01)) + 1.0 : *(0.5);
//lfo = oscy(0.25); //hslider("hi-lo[OWL:PARAMETER_C]", 1,0.05,1,0.01);
lfo = phasor(0.25); //hslider("hi-lo[OWL:PARAMETER_C]", 1,0.05,1,0.01);

// LFO with a triangular waveform : 0..1..0..1..
// lfo(freq) = freq/SR : (+ , 2.0 : fmod) ~ _ : -(1.0) : abs ;
//lfo(freq) = freq : int : (+, 48000 : %) ~ _ : /(24000.0) : -(1.0) <: *;
//lfo(freq) = freq : int : (+, 480000 : %) ~ _ : -(240000) : abs : /(240000.0);

// mono wah-wah based on crybaby
wahwah = crybaby(wah) with {
   lo = hslider("low[OWL:PARAMETER_A]",0.8,0,1,0.01);
   hi = hslider("high[OWL:PARAMETER_B]",0.8,0,1,0.01);
   wah = lo*lfo + hi*(1.0-lfo);
};

// add dry wet control
stereodrywet (monofx) = _,_ <: *(1-dw), *(1-dw), monofx*dw, monofx*dw :> _,_
	with {
		dw = hslider("dry wet[OWL:PARAMETER_D]",0.5,0,1,0.01);
	};
	
// add dry wet control
monodrywet (monofx) = _ <: *(1-dw), monofx*dw :> _
	with {
		dw = hslider("dry wet[OWL:PARAMETER_D]",0.5,0,1,0.01);
	};
	
// process = stereodrywet(wahwah);
process = monodrywet(wahwah); // 3960 ARM cycles
