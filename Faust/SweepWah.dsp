// Wah-wah with lfo modulation
import("music.lib");
import("effect.lib");
import("oscillator.lib");

lfo = 
oscrs(freq) + 1.0 : *(0.5)
// lf_rawsaw(freq) + 1.0 : *(0.5)
    with {
      freq  = hslider("Frequency[unit:Hz][OWL:PARAMETER_C]", 4, 0, 12, 0.001);
    };

// mono wah-wah based on crybaby
wahwah = crybaby(wah) with {
   lo = hslider("Low[OWL:PARAMETER_A]",0.8,0,1,0.01);
   hi = hslider("High[OWL:PARAMETER_B]",0.8,0,1,0.01);
   wah = lo*lfo + hi*(1.0-lfo);
};

fxctrl(w,Fx) =  _ <: (Fx : *(w)), *(1-w) +> _;

process = fxctrl(wet, wahwah)
	with {
		wet = hslider("Dry/Wet[OWL:PARAMETER_D]",0.5,0,1,0.01);
	};

