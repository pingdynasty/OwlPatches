import("effect.lib");
wahwah = crybaby(wah) with {
   wah = hslider("Wah[OWL:PARAMETER_A]",0.8,0,1,0.01);
};
stereodrywet (monofx) = _,_ <: *(1-dw), *(1-dw), monofx*dw, monofx*dw :> _,_
	with {
		dw = hslider("dry wet[OWL:PARAMETER_B]",0.5,0,1,0.01);
	};	
process = stereodrywet(wahwah);