import("effect.lib");
wahwah = crybaby(wah) with {
    wah = vslider("Wah[style:knob][OWL:PARAMETER_A]", 0, 0, 1, 0.01) +
          vslider("WahWah[style:knob][OWL:PARAMETER_E]", 0, 0, 1, 0.01) ;
};
stereodrywet (monofx) = _,_ <: *(1-dw), *(1-dw), monofx*dw, monofx*dw :> _,_
	with {
		dw = hslider("Dry/Wet[OWL:PARAMETER_D]",0.5,0,1,0.01);
	};	
process = stereodrywet(wahwah);